`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   18:17:09 08/06/2013
// Design Name:   SPI_MODULE
// Module Name:   /home/tusk/temp/cajipci/src/SPI_MODULE_TEST.v
// Project Name:  cajipci
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: SPI_MODULE
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module SPI_MODULE_TEST;

	// Inputs
	reg CLOCK;
	reg SPI_MISO;
	reg [31:0] SPI_IN;
	reg GO;

	// Outputs
	wire SPI_MOSI;
	wire SPI_CS;
	wire SPI_SCLK;
	wire [31:0] SPI_OUT;
	wire DONE;

	// Instantiate the Unit Under Test (UUT)
	SPI_MODULE uut (
		.CLOCK(CLOCK), 
		.SPI_MISO(SPI_MISO), 
		.SPI_MOSI(SPI_MOSI), 
		.SPI_CS(SPI_CS), 
		.SPI_SCLK(SPI_SCLK), 
		.SPI_IN(SPI_IN), 
		.SPI_OUT(SPI_OUT), 
		.GO(GO), 
		.DONE(DONE)
	);

	always begin
		CLOCK = ~CLOCK;
		#1;
	end
	
	initial begin
		// Initialize Inputs
		CLOCK = 0;
		SPI_MISO = 1;
		SPI_IN = 0;
		GO = 0;

		// Wait 100 ns for global reset to finish
		#10;
      SPI_IN = 'hAAAAAAAE;  
		#1;
		GO = 1;
		// Add stimulus here

	end
      
endmodule

