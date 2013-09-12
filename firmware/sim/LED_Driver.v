`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    10:19:25 02/25/2013 
// Design Name: 
// Module Name:    LED_Driver 
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
module LED_Driver(
	input wire CLK, 
	input wire [23:0] DATA_IN, 
	input wire EN_IN, 
	output reg RDY,
	output wire LED_CLK,
	output reg LED_OE,
	output wire LED_OUT
   );

reg state;
reg [4:0] state_counter;
reg [23:0] shift;

assign LED_CLK = (RDY == 0 ? CLK : 1'b0);

assign LED_OUT = shift[23];

initial begin
	LED_OE = 0;
	RDY = 1;
	state = 0;
end

always @(posedge CLK) begin
	if(state == 0) begin
		if(EN_IN == 1) begin
			shift[23:0] = DATA_IN[23:0];
			state = 1;
			RDY = 0;
			LED_OE = 1;
			state_counter = 0;
		end
		else begin
			RDY = 1;
			LED_OE = 0;
		end
	end
	else begin
		if(state_counter == 24) begin
			state = 0;
		end
		shift = shift << 1;
		state_counter = state_counter + 1;
	end
end

endmodule
