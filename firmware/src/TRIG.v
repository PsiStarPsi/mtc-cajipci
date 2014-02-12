`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    11:54:10 08/28/2013 
// Design Name: 
// Module Name:    TRIG 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module TRIG(
	input RESET,
	input CLK_42MHZ,
   output [11:0] TRG,
   input [11:0] ACK,
	input [11:0] TRG_MASK,
	input [3:0] MIN_SCRODS_REQUIRED,
	output [31:0] TRG_STATISTICS,
	input TRG_SOFT,
 
	output TRG_NEEDS_VETO,
	input TRG_FLOW_CTL_EN,
	input TRG_VETO_RESET,
	
	output [2:0] trg_delay_out,
	output [3:0] TRG_SCROD_COUNT
    );

reg need_veto;
assign TRG_NEEDS_VETO = need_veto;

//Trigger accumulator across scrods. Recomputed each clock cycle
reg [3:0] current_triggers;

assign TRG_SCROD_COUNT = current_triggers;

//Trigger outputs to the lvds
reg [11:0] trg_reg;
assign TRG = trg_reg;

//Number of triggers since startup
reg [31:0] trg_statistics_reg;
assign TRG_STATISTICS = trg_statistics_reg;

//Delay for the scrod TRG signal 
reg [2:0] trg_delay;

//Edge detector for the clear signal
reg TRG_CLR;

initial begin
	trg_statistics_reg = 0;
	trg_reg = 0;
	trg_delay = 7;
end

reg soft_trig_pos_edge;
reg soft_trig_buffered;
reg soft_trig_edge;


always @(posedge TRG_SOFT or posedge TRG_CLR )
begin
	if (TRG_CLR)
		soft_trig_edge <= 0;
	else 
		soft_trig_edge <= 1;
end


always @(posedge CLK_42MHZ) begin
	soft_trig_buffered<= soft_trig_edge;
end


always @(posedge CLK_42MHZ) begin
	TRG_CLR<= soft_trig_buffered;
end


always @(posedge CLK_42MHZ) begin
	soft_trig_pos_edge<= soft_trig_buffered;
end

//Adding up all of the trigger bits
always @(posedge CLK_42MHZ) begin
	current_triggers <= (TRG_MASK[0] & ACK[0]) + 
	(TRG_MASK[1] & ACK[1]) +
	(TRG_MASK[2] & ACK[2]) +
	(TRG_MASK[3] & ACK[3]) +
	(TRG_MASK[4] & ACK[4]) +
	(TRG_MASK[5] & ACK[5]) +
	(TRG_MASK[6] & ACK[6]) +
	(TRG_MASK[7] & ACK[7]) +
	(TRG_MASK[8] & ACK[8]) +
	(TRG_MASK[9] & ACK[9]) +
	(TRG_MASK[10] & ACK[10]) +
	(TRG_MASK[11] & ACK[11]);
end

//Main trigger logic.
reg start_count;
reg [19:0] wait_counter;
always @(posedge CLK_42MHZ) begin
	if(RESET) begin
		trg_statistics_reg <= 0;
		trg_delay <= 7;
		trg_reg <= 0;
		need_veto <= 0;
	end
	else begin
		if (!(need_veto & TRG_FLOW_CTL_EN)) begin
			if(current_triggers >= MIN_SCRODS_REQUIRED || soft_trig_pos_edge == 1) begin
				trg_reg <= 'hFFF;
				if(trg_delay == 7) begin	//Will not increment the counter for the next 7 cycles.
					trg_statistics_reg <= trg_statistics_reg + 32'b1;
					trg_delay <= 0;
					need_veto <= 1;
				end
				else begin
					trg_delay <= 0;
				end
			end		
		end
		else begin
			if(TRG_VETO_RESET | !TRG_FLOW_CTL_EN)
				start_count <=1;
			else 
				start_count <=0;
			if(start_count==1)
				wait_counter <= 660000;
			else begin
				if (wait_counter > 1)
					wait_counter <= wait_counter-1;
				else if (wait_counter==1)
					begin
						need_veto <= 0;
						wait_counter<=0;
					end
			end 
				
		if( trg_delay != 7) begin
			trg_delay <= trg_delay +1;
			trg_reg <= 'hfff;
		end
		else
			trg_reg <= 'h000;	
		end
	end
end

assign trg_delay_out = trg_delay;

endmodule
