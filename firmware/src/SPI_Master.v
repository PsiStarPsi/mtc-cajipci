`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    01:29:58 06/06/2012 
// Design Name: 
// Module Name:    SPI_Master 
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
module SPI_Master(
	input BOARD_CLOCK,
	input RST,
	//SPI
	input SPI_MISO,
	output SPI_MOSI,
	output SPI_CLK,
	output [2:0] SPI_CSS,
	//From PCI
	output [31:0] SPI_O,
	input [31:0] SPI_I,
	output SPI_DONE_O,
	input SPI_STAR_I,
	input [1:0] SPI_SEL_I
   );

	//State machine
	`define SPI_FSM_BITS 2
	reg [`SPI_FSM_BITS-1:0] state;
	parameter	READY=`SPI_FSM_BITS'h0,
			IDLE=`SPI_FSM_BITS'h1,
			SEND=`SPI_FSM_BITS'h2,
			SEND_RECIEVE=`SPI_FSM_BITS'h3;

	//Clock generation
	reg [2:0] clk_div_reg;
	
	initial begin
		clk_div_reg = 3'b0;
	end

	always @(posedge BOARD_CLOCK) begin
		if(RST) clk_div_reg = 0;
		else	clk_div_reg = clk_div_reg + 1;
	end

	assign spi_clk_int = clk_div_reg[2];	
	assign SPI_CLK = (state == READY) ? 0 : spi_clk_int;
	
	//CHIP Select logic
	
	reg [1:0] spi_sel_reg;
	reg spi_cs;
		
	generate
	genvar SPI_CSIt;
	for (SPI_CSIt=0;SPI_CSIt<3;SPI_CSIt=SPI_CSIt+1) begin : SPI_CS_LOOP
		assign SPI_CSS[SPI_CSIt] = (spi_sel_reg == SPI_CSIt) ? spi_cs : 1;
	end
	endgenerate
	
	always @(posedge spi_clk_int) begin
		if(RST)
			spi_sel_reg <= 0;
		else if(state == READY)
			spi_sel_reg <= SPI_SEL_I;
		else
			spi_sel_reg <=spi_sel_reg;
	end
	
	//SHIFT_REGISTER
	reg [31:0] spi_i_reg;
	reg [31:0] spi_o_reg;
	reg mosi_reg;
	
	assign SPI_MOSI = mosi_reg;
	assign SPI_O = spi_o_reg;
	
	initial begin
		spi_i_reg <= 32'b0;
		spi_o_reg <= 32'b0;
		mosi_reg <= 0;
	end
	
	always @(posedge spi_clk_int) begin
		if(state == READY) begin
			spi_i_reg = SPI_I;
		end
		else if(state == SEND) begin
			mosi_reg = spi_i_reg[0];
			spi_i_reg = spi_i_reg >> 1;
		end
		else if(state == SEND_RECIEVE) begin
			mosi_reg = spi_i_reg[0];
			spi_i_reg = spi_i_reg >> 1;
			spi_o_reg = spi_o_reg >> 1;
			spi_o_reg[31] = SPI_MISO;	
		end
	end
	
	//State_Machine
	reg [7:0] spi_state_counter;
	reg spi_done_reg;

	assign SPI_DONE_O = spi_done_reg;
	
	initial begin
		spi_state_counter = 8'b0;
		state = READY;
		spi_done_reg = 0;
		spi_cs = 1;
	end
	
	always @(posedge spi_clk_int) begin
		if(RST) begin
			state <= READY;
			spi_done_reg <= 1;
			spi_state_counter <= 8'b0;
			spi_cs <= 1;
		end
		else begin
			case(state) 
				READY: begin
					spi_cs <= 1;
					spi_state_counter <= 8'b0;
					spi_done_reg <= 0;
					if(SPI_STAR_I) begin
						if(SPI_I[3:0] == 4'b1110)
							state <= SEND_RECIEVE;
						else
							state <= SEND;
					end
				end
				IDLE: begin
					spi_cs <= 1;
					spi_done_reg <= 1;
					spi_state_counter <= 8'b0;
					if(SPI_STAR_I == 0) begin
						state <= READY;
					end
				end
				SEND : begin
					spi_cs <= 0;
					spi_state_counter <= spi_state_counter +1;
					if(spi_state_counter > 32)
						state <= IDLE;
				end
				SEND_RECIEVE : begin
					case(spi_state_counter)
						32: begin
							spi_cs <= 1;
							spi_state_counter <= spi_state_counter +1;
						end
						33: begin
							spi_cs <= 1;
							spi_state_counter <= spi_state_counter +1;
						end
						65: begin
							state <= IDLE;
						end
						default: begin
							spi_cs <= 0;
							spi_state_counter <= spi_state_counter +1;
						end
					endcase
				end
			endcase
		end
	end

endmodule
