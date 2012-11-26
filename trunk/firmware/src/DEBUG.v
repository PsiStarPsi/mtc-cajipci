`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    15:46:31 06/12/2012 
// Design Name: 
// Module Name:    DEBUG 
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
module DEBUG(
    input [7:0] trg,
	 input clk
    );
	wire [35:0] control0;
	
	icon U_ICON (
    .CONTROL0(control0) // INOUT BUS [35:0]
	);
	
	ila U_ILA (
    .CONTROL(control0), // INOUT BUS [35:0]
    .CLK(clk), // IN
    .TRIG0(trg) // IN BUS [7:0]
);
endmodule
