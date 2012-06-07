`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:30:12 06/06/2012 
// Design Name: 
// Module Name:    WISHBONE_MASTER 
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

module WISHBONE_MASTER(
	// Clock and reset
	wb_clk_i, wb_rst_i,
 
	// WISHBONE Master I/F
	wbm_cyc_o, wbm_stb_o, wbm_sel_o, wbm_we_o,
	wbm_adr_o, wbm_dat_o, wbm_cab_o,
	wbm_dat_i, wbm_ack_i, wbm_err_i, wbm_rty_i
	);
 
//
// I/O ports
//
 
//
// Clock and reset
//
input			wb_clk_i;	// Pixel Clock
input			wb_rst_i;	// Reset
 
//
// WISHBONE Master I/F
//
output			wbm_cyc_o;
output			wbm_stb_o;
output	[3:0]		wbm_sel_o;
output			wbm_we_o;
output	[31:0]		wbm_adr_o;
output	[31:0]		wbm_dat_o;
output			wbm_cab_o;
input	[31:0]		wbm_dat_i;
input			wbm_ack_i;
input			wbm_err_i;
input			wbm_rty_i;
  
 
reg [31:2] wbm_adr ;
always@(posedge wb_clk_i or posedge wb_rst_i)
begin
    if (wb_rst_i)
		wbm_adr <= #1 30'h0000_0000 ;
end

assign wbm_cyc_o = 0;
assign wbm_stb_o = 0;
assign wbm_sel_o = 4'b1111;
assign wbm_we_o = 1'b0;
assign wbm_adr_o = {wbm_adr, 2'b00};
assign wbm_dat_o = 32'h0000_0000;
assign wbm_cab_o = 1'b1;
endmodule
