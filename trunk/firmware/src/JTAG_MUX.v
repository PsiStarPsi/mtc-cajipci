`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    14:13:05 06/07/2012 
// Design Name: 
// Module Name:    JTAG_MUX 
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
module JTAG_MUX(
		input [11:0] TDO,
		output [11:0] TDI,
		output TMS,
		output TCK,
		input [3:0] JTAG_SEL,
		input V_TDI,
		output V_TDO,
		input V_TMS,
		input V_TCK
    );

	assign TMS = ~V_TMS;
	assign TCK = ~V_TCK;

	generate
	genvar JTAGIt;
	for (JTAGIt=0;JTAGIt<12;JTAGIt=JTAGIt+1) begin : JTAG_LOOP
		assign TDI[JTAGIt] = ((JTAG_SEL == JTAGIt) || JTAG_SEL > 'd11) ? ~V_TDI : 0;
	end
	endgenerate

	assign V_TDO = (JTAG_SEL < 'd12) ? ~TDO[JTAG_SEL] : ~TDO[0];

endmodule
