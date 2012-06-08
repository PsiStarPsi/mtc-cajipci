`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    22:17:38 06/05/2012 
// Design Name: 
// Module Name:    WISHBONE_SLAVE 
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
module WISHBONE_SLAVE(
	 input clk_i,
	 input reset_i,
	 //wb master interface
	 input cyc_i,
	 input stb_i,
	 output err_o,
	 output rty_o,
	 output ack_o,
	 input [31:0] dat_i,
	 output [31:0] dat_o,
	 input [31:0] adr_i,
	 input [2:0] cti_i,
	 input [1:0] bte_i,
	 input we_i,
	 input[3:0] sel_i,
	 //SPI_CTL
	 input [31:0] SPI_I,
	 output [31:0] SPI_O,
	 input 	SPI_DONE_I,
	 output SPI_STAR_O,
	 output [1:0] SPI_SEL_O,
	 //Trigger and ack
	 input	[11:0] TRG_BITS_I,
	 output	[11:0] ACK_BITS_O,
	 //JTAG Controlls
	 output [3:0] JTAG_SEL_O
    );

	`define RECEIVER_FSM_BITS 2
	reg [`RECEIVER_FSM_BITS-1:0] state;
	parameter	IDLE=`RECEIVER_FSM_BITS'h0,
			REQ_SINGLE_RECEIVED=`RECEIVER_FSM_BITS'h1,
			REQ_BURST_RECEIVED=`RECEIVER_FSM_BITS'h2,
			REQ_ERROR=`RECEIVER_FSM_BITS'h3;

	reg ack_reg;
	reg [31:0] dat_o_reg;
	reg [31:0] dat_i_reg;
	reg [9:0] adr_i_reg;
	reg [2:0] cti_i_reg;
	reg [1:0] bte_i_reg;
	reg we_i_reg;
	reg [3:0] sel_i_reg;
	assign dat_o=dat_o_reg;
	assign ack_o=ack_reg;
	assign err_o=(state==REQ_ERROR);
	assign rty_o=1'b0;

	
	//PCI registers
	//SPI
	reg [31:0] spi_o_reg;
	reg spi_start;
	reg [2:0] spi_sel_reg;
		
	assign SPI_O=spi_o_reg;
	assign SPI_SEL_O = spi_sel_reg;
	assign SPI_STAR_O = spi_start;

	//TRG and ACK
	reg [11:0]	ack_bit_reg;

	assign ACK_BITS_O = ack_bit_reg;
	
	//JTAG select
	reg [3:0] jtag_sel_reg;
	
	assign JTAG_SEL_O = jtag_sel_reg;
	
	
	always@(posedge clk_i) begin
		if(reset_i)
			state<=IDLE;
		else begin
			case(state)
				IDLE: 	begin
						if(cyc_i==1'b1 && stb_i==1'b1) begin
							if(cti_i==3'b000 || cti_i==3'b111) state<=REQ_SINGLE_RECEIVED;
							else if(cti_i==3'b001 || cti_i==3'b010) state<=REQ_BURST_RECEIVED;
							else state<=REQ_ERROR;
						end else state<=IDLE;
					end
				REQ_SINGLE_RECEIVED:	state<=IDLE;	
				REQ_BURST_RECEIVED:	begin
												if(cti_i==3'b111) state<=IDLE;
												else if(cti_i==3'b001 || cti_i==3'b010) state<=REQ_BURST_RECEIVED;
												else state<=REQ_ERROR;
											end	
				REQ_ERROR: state<=IDLE;	
			endcase
		end
	end

	always@(posedge clk_i) begin
		if(reset_i) begin
			dat_i_reg<=32'b0;
			adr_i_reg<={10{1'b1}};
			cti_i_reg<=3'b0;
			bte_i_reg<=2'b0;
			we_i_reg<=1'b0;
			sel_i_reg<=4'b0;
		end else if(cyc_i==1'b1 && stb_i==1'b1)begin
			dat_i_reg<=dat_i;
			adr_i_reg<={adr_i[11:2]};
			cti_i_reg<=cti_i;
			bte_i_reg<=bte_i;
			we_i_reg<=we_i;
			sel_i_reg<=sel_i;
		end else begin
			dat_i_reg<=32'b0;
			adr_i_reg<={10{1'b1}};
			cti_i_reg<=3'b0;
			bte_i_reg<=2'b0;
			we_i_reg<=1'b0;
			sel_i_reg<=4'b0;
		end
	end
	
	always@(posedge clk_i) begin
		if(reset_i)
			ack_reg<=1'b0;
		else 
			ack_reg<=(cyc_i & stb_i);
	end	
	
	always@(*) begin
		case(adr_i_reg)
		10'd0: dat_o_reg <= spi_o_reg;									//SPI_data_to_be_writen.
		10'd1: dat_o_reg <= SPI_I;											//SPI data reg from the device
		10'd2: dat_o_reg <= {spi_sel_reg, SPI_DONE_I,spi_start}; //SPI_control_register.
		10'd3: dat_o_reg <={4'b0, TRG_BITS_I, 4'b0, ack_bit_reg};			//	Trg and Ack 
		10'd4: dat_o_reg <= {jtag_sel_reg};
		default: dat_o_reg <= 32'b0;
		endcase
	end
	
	
	always@(posedge clk_i) begin
		if(reset_i)
			 spi_o_reg<=32'b0;	
		else if(we_i_reg==1'b1 && (state==REQ_SINGLE_RECEIVED || state==REQ_BURST_RECEIVED) && adr_i_reg==10'd0) begin
								 if(sel_i_reg[0]==1'b1)   spi_o_reg[7:0]<=dat_i_reg[7:0];
								 else   spi_o_reg[7:0]<=  spi_o_reg[7:0];
									
								 if(sel_i_reg[1]==1'b1)   spi_o_reg[15:8]<=dat_i_reg[15:8];
								 else   spi_o_reg[15:8]<=  spi_o_reg[15:8];
								 
								 if(sel_i_reg[2]==1'b1)   spi_o_reg[23:16]<=dat_i_reg[23:16];
								 else   spi_o_reg[23:16]<=  spi_o_reg[23:16];
								 
								 if(sel_i_reg[3]==1'b1)   spi_o_reg[31:24]<=dat_i_reg[31:24];
								 else   spi_o_reg[31:24]<=  spi_o_reg[31:24];
		end else
			 spi_o_reg<= spi_o_reg;
	end

	always@(posedge clk_i) begin
		if(reset_i)
		begin
			spi_sel_reg <= 2'b0;
			spi_start <= 2'b0;
		end
		else if(we_i_reg==1'b1 && (state==REQ_SINGLE_RECEIVED || state==REQ_BURST_RECEIVED) && adr_i_reg==10'd2) begin
			if(sel_i_reg[0]==1'b1) begin
				spi_start <= dat_i_reg[0];
				spi_sel_reg <=dat_i_reg[3:2];
			end
			else  begin
				spi_start <= spi_start;
				spi_sel_reg <= spi_sel_reg;
			end
		end
		else begin
			spi_start <= spi_start;
			spi_sel_reg <= spi_sel_reg;
		end
	end

	always@(posedge clk_i) begin
		if(reset_i)
			 ack_bit_reg<=12'b0;	
		else if(we_i_reg==1'b1 && (state==REQ_SINGLE_RECEIVED || state==REQ_BURST_RECEIVED) && adr_i_reg==10'd3) begin
								 if(sel_i_reg[0]==1'b1)   ack_bit_reg[7:0]<=dat_i_reg[7:0];
								 else   ack_bit_reg[7:0] <=  ack_bit_reg[7:0];
									
								 if(sel_i_reg[1]==1'b1)   ack_bit_reg[11:8]<=dat_i_reg[11:8];
								 else   ack_bit_reg[11:8]<=  ack_bit_reg[11:8];
		end else
			 ack_bit_reg<= ack_bit_reg;
	end

	always@(posedge clk_i) begin
		if(reset_i)
			 jtag_sel_reg<=4'b0;	
		else if(we_i_reg==1'b1 && (state==REQ_SINGLE_RECEIVED || state==REQ_BURST_RECEIVED) && adr_i_reg==10'd4) begin
								 if(sel_i_reg[0]==1'b1)   jtag_sel_reg[3:0]<=dat_i_reg[3:0];
								 else   jtag_sel_reg[3:0] <=  jtag_sel_reg[3:0];
		end else
			 jtag_sel_reg <= jtag_sel_reg;
	end


endmodule
