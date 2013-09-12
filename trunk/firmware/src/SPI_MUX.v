`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    16:19:50 07/15/2013 
// Design Name: 
// Module Name:    SPI_MUX 
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
module SPI_MUX(
		output [2:0] SPI_CS_OUT,
		input SPI_CS_IN,
		input [1:0] SLAVE_SELECT
    );

	generate
	genvar SPIIt;
	for (SPIIt = 0; SPIIt < 3; SPIIt = SPIIt + 1) begin : SPI_LOOP
		assign SPI_CS_OUT[ SPIIt ] = (SLAVE_SELECT == SPIIt) ?  SPI_CS_IN : 1;
	end
	endgenerate

endmodule
