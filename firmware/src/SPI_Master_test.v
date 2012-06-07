`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   03:15:40 06/06/2012
// Design Name:   SPI_Master
// Module Name:   /home/tusk/svn/mtc-cajipci/firmware/src/SPI_Master_test.v
// Project Name:  cajipci
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: SPI_Master
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module SPI_Master_test;

	// Inputs
	reg BOARD_CLOCK;
	reg RST;
	reg SPI_MISO;
	reg [31:0] SPI_I;
	reg SPI_STAR_I;
	reg [1:0] SPI_SEL_I;

	// Outputs
	wire SPI_MOSI;
	wire SPI_CLK;
	wire [2:0] SPI_CSS;
	wire [31:0] SPI_O;
	wire SPI_DONE_O;

	// Instantiate the Unit Under Test (UUT)
	SPI_Master uut (
		.BOARD_CLOCK(BOARD_CLOCK), 
		.RST(RST), 
		.SPI_MISO(SPI_MISO), 
		.SPI_MOSI(SPI_MOSI), 
		.SPI_CLK(SPI_CLK), 
		.SPI_CSS(SPI_CSS), 
		.SPI_O(SPI_O), 
		.SPI_I(SPI_I), 
		.SPI_DONE_O(SPI_DONE_O), 
		.SPI_STAR_I(SPI_STAR_I), 
		.SPI_SEL_I(SPI_SEL_I)
	);

	always begin
		BOARD_CLOCK = ~BOARD_CLOCK;
		#1;
	end
	
	always @(posedge BOARD_CLOCK) begin
		SPI_MISO = 1;
	end;
	
	initial begin
		// Initialize Inputs
		BOARD_CLOCK = 0;
		RST = 0;
		SPI_MISO = 0;
		SPI_I = 0;
		SPI_STAR_I = 0;
		SPI_SEL_I = 2;

		// Wait 100 ns for global reset to finish
		#10;
        
		// Add stimulus here
		SPI_I = 32'b11001100110011001100110011001110;
		SPI_STAR_I = 1;
		#30;
		SPI_STAR_I = 0;
	end
      
endmodule

