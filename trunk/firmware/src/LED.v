`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    20:11:09 10/07/2013 
// Design Name: 
// Module Name:    ledDriver 
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
module LED(
    input [11:0] TRG_MASK,
    input [3:0] JTAG_MUX,
    input CLK_1MHZ,
	 output LED_CLK, 
    output LED_OE, 
    output LED_OUT
    );


reg go;
reg [23:0] data_in;
reg [11:0] mux_temp;
reg [23:0] data_old;
wire rdy;

initial begin
	data_old = 0;
	go = 0;
end

always @(posedge CLK_1MHZ) begin
	if(JTAG_MUX < 12)
		mux_temp = 1 << JTAG_MUX;
	else
		mux_temp = 'hFFF;
	
	data_in = {mux_temp, TRG_MASK};
	
	if(data_in != data_old ) begin
		if(rdy == 1) begin
			go = 1;
			data_old = data_in;
		end
		else begin
			go = 0;
		end
	end
	else begin
		go = 0;
	end
	
end

LED_Driver u_led_driver (
    .CLK(CLK_1MHZ),
    .DATA_IN({
		mux_temp[8], TRG_MASK[8],
		mux_temp[9], TRG_MASK[9],
		mux_temp[10], TRG_MASK[10],
		mux_temp[11], TRG_MASK[11],
	 
	 	mux_temp[4], TRG_MASK[4],
		mux_temp[5], TRG_MASK[5],
		mux_temp[6], TRG_MASK[6],
		mux_temp[7], TRG_MASK[7],
	 
		mux_temp[0], TRG_MASK[0],
		mux_temp[1], TRG_MASK[1], 
		mux_temp[2], TRG_MASK[2],
		mux_temp[3], TRG_MASK[3]}), 
    .EN_IN(go), 
    .RDY(rdy), 
    .LED_CLK(LED_CLK), 
    .LED_OE(LED_OE), 
    .LED_OUT(LED_OUT)
    );

endmodule
