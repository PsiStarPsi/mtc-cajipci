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
	 input TRG_SOFT
    );

reg [3:0] current_triggers;

reg [11:0] trg_reg;
assign TRG = trg_reg;

reg [31:0] trg_statistics_reg;
assign TRG_STATISTICS = trg_statistics_reg;

reg [2:0] trg_delay;

reg TRG_CLR;

initial begin
	trg_statistics_reg = 0;
	trg_reg = 0;
	trg_delay = 3;
end

reg soft_trig_pos_edge;
reg soft_trig_buffered;
//wire soft_trig_edge;
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

always @(negedge CLK_42MHZ) begin
	current_triggers = 0;
	if(TRG_MASK[0] == 1 && ACK[0] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[1] == 1 && ACK[1] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[2] == 1 && ACK[2] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[3] == 1 && ACK[3] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[4] == 1 && ACK[4] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[5] == 1 && ACK[5] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[6] == 1 && ACK[6] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[7] == 1 && ACK[7] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[8] == 1 && ACK[8] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[9] == 1 && ACK[9] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[10] == 1 && ACK[10] == 1)
			current_triggers = current_triggers + 1;
	if(TRG_MASK[11] == 1 && ACK[11] == 1)
			current_triggers = current_triggers + 1;
end

always @(posedge CLK_42MHZ) begin
	if(RESET) begin
		trg_statistics_reg <= 0;
		trg_delay <= 7;
		trg_reg <= 0;
	end
	else begin
		if(current_triggers >= MIN_SCRODS_REQUIRED || soft_trig_pos_edge == 1) begin
			trg_reg <= 'hFFF;
			if(trg_delay == 7) begin	//Will not increment the counter for the next 7 cycles.
				trg_statistics_reg <= trg_statistics_reg + 32'b1;
				trg_delay <= 0;
			end
		end
		else begin
			if( trg_delay != 7) begin
				trg_delay <= trg_delay +1;
				trg_reg <= 'hFFF;
			end
			else begin
				trg_reg <= 'h000;
			end
		end
	end
end

endmodule
