`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   21:55:12 09/11/2013
// Design Name:   TRIG
// Module Name:   /home/tusk/temp/cajipci/src/TRG_DRIVER.v
// Project Name:  cajipci
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: TRIG
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module TRG_DRIVER;

	// Inputs
	reg [11:0] ACK;
	reg [11:0] TRG_MASK;
	reg [3:0] MIN_SCRODS_REQUIRED;
	reg CLK_80MHZ;
	reg RESET;
	reg TRG_SOFT;
	// Outputs
	wire [11:0] TRG;
	wire [31:0] TRG_STATISTICS;

	// Instantiate the Unit Under Test (UUT)
	TRIG uut (
		.TRG(TRG), 
		.ACK(ACK), 
		.TRG_MASK(TRG_MASK), 
		.MIN_SCRODS_REQUIRED(MIN_SCRODS_REQUIRED), 
		.TRG_STATISTICS(TRG_STATISTICS), 
		.CLK_80MHZ(CLK_80MHZ),
		.RESET(RESET),
		.TRG_SOFT
	);
	
	//clock
	always begin
		#1;
		CLK_80MHZ = ~CLK_80MHZ;
	end
	
	initial begin
		// Initialize Inputs
		ACK = 0;
		TRG_MASK = 'hF;
		MIN_SCRODS_REQUIRED = 'h4;
		CLK_80MHZ = 1;
		TRG_SOFT = 0;
		RESET = 0;
		#2;
		ACK = 'hF;
		#2;
		ACK = 'h8;
		#2;
		ACK = 'h1e;
		#2
		ACK = 0;
		#2
		TRG_SOFT = 1;
		#4
		TRG_SOFT = 0;
		
		
		
	end
      
endmodule

