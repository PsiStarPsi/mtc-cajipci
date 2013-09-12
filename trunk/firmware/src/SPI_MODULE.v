`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:35:07 07/05/2013 
// Design Name: 
// Module Name:    SPI_MODULE 
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
module SPI_MODULE(
    input CLOCK,
    input SPI_MISO,
    output reg SPI_MOSI,
    output reg SPI_CS,
    output SPI_SCLK,
    input [31:0] SPI_IN,
    output reg [31:0] SPI_OUT,
    input GO,
    output reg DONE
    );

initial SPI_MOSI = 0;
initial SPI_CS = 1;
initial DONE = 1;

reg [31:0] SPI_IN_INT;
initial SPI_OUT = 0;

reg clockEn;
initial clockEn = 0;

assign SPI_SCLK = clockEn == 0 ? 0 : CLOCK;

reg [6:0] state;
initial state = 0;

reg goInt;
initial goInt = 0;
reg goDone;
initial goDone = 0;

always @(posedge CLOCK) begin
	if(GO == 1) begin
		if(goInt == 1) begin
			goDone <= 1;
			goInt <= 0;
		end
		else if(goDone == 0) begin
			goInt <= 1;
		end
	end
	else begin
			goDone <= 0;
			goInt <= 0;		
	end
end

always @(negedge CLOCK) begin
	if(state == 0) begin
		DONE = 1;
		SPI_MOSI = 0;
		if(goInt == 1) begin
			clockEn = 1;
			SPI_IN_INT = SPI_IN;
			SPI_CS = 0;
			SPI_MOSI = SPI_IN_INT[0];
			DONE = 0;
			state = 1;
		end
	end
	else if(state < 32) begin
		SPI_MOSI = SPI_IN_INT[state];
		state = state +1;
	end
	else if(state == 32) begin
		clockEn = 0;
		SPI_CS = 1;
		SPI_MOSI = 0;
		state = state +1;
	end
	else if(SPI_IN_INT[3:0] == 'b1110) begin
		if(state < 65) begin
			clockEn = 1;
			SPI_CS = 0;
			state = state +1;
		end
		else begin
			clockEn = 0;
			SPI_CS = 1;
			state = 0;
		end
	end
	else begin
		clockEn = 0;
		SPI_CS = 1;
		state = 0;
	end
end

always @(posedge CLOCK) begin
	if(state >33) begin
		if(state < 66) begin
			SPI_OUT[state - 34] = SPI_MISO;
		end
	end
end

endmodule
