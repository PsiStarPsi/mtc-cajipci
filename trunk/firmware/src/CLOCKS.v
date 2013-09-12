`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    12:16:10 02/25/2013 
// Design Name: 
// Module Name:    CLOCKS 
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
module CLOCKS(
	input wire BOARD_CLOCK,
	input wire RST,
	output CLK_80MHZ,
	output CLK_20MHZ,
	output reg CLK_1MHZ,
	output LOCKED
    );

CLOCK_GEN u_clock_gen (
    .CLKIN_IN(BOARD_CLOCK), 
    .RST_IN(RST), 
    .CLKFX_OUT(CLK_20MHZ), 
    .CLKIN_IBUFG_OUT(CLK_80MHZ), 
    .LOCKED_OUT(LOCKED)
    );
	 
reg [4:0] counter;

initial CLK_1MHZ = 0;

always @(posedge CLK_20MHZ) begin
	if(RST) 
		counter <= 0;
	else begin	
		if(counter > 19) begin
			CLK_1MHZ <= ~CLK_1MHZ;
			counter <= 0;
		end
		else begin
			counter <= counter + 1;
		end
	end
end
	 
endmodule
