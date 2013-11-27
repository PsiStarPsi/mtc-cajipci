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
	output CLK_21MHZ,
	output CLK_42MHZ,
	output reg CLK_1MHZ,
	input PCI_CLK,
	output CLK_66MHZ
    );
	 
CLOCK_GEN u_clock_gen (
    .CLKIN_IN(BOARD_CLOCK), 
    .RST_IN(RST), 
    .CLKFX_OUT(CLK_21MHZ), 
    .CLKIN_IBUFG_OUT(CLK_80MHZ)
    );

CLOCK_GEN_2X u_clock_gen_2x (
    .CLKIN_IN(CLK_21MHZ), 
    .RST_IN(rst), 
    .CLKFX_OUT(CLK_42MHZ)
    );

CLOCK_GEN_PCI u_clock_gen_pci (
    .CLKIN_IN(PCI_CLK), 
    .RST_IN(RST), 
    .CLKFX_OUT(CLK_66MHZ)
    );
	 
reg [4:0] counter;

initial counter = 0;
initial CLK_1MHZ = 0;

always @(posedge CLK_21MHZ) begin
	if(RST) 
		counter <= 0;
	else begin	
		if(counter > 20) begin
			CLK_1MHZ <= ~CLK_1MHZ;
			counter <= 0;
		end
		else begin
			counter <= counter + 1;
		end
	end
end
	 
endmodule
