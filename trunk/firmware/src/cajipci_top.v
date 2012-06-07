`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    22:13:30 06/05/2012 
// Design Name: 
// Module Name:    cajipci_top 
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
module cajipci_top(
		// PCI
		input PCI_CLK,				// PCI clock
		input PCI_RST,				// PCI RST#
		output PCI_INTA,			// PCI INTA#
		output PCI_REQ,			// PCI REQ#
		input PCI_GNT,				// PCI GNT#
		inout PCI_FRAME,		 	// PCI FRAME#
		inout PCI_IRDY,		 	// PCI IRDY#
		inout PCI_DEVSEL,		 	// PCI DEVSEL#
		inout PCI_TRDY,	 	 	// PCI TRDY#
		inout PCI_STOP,		 	// PCI STOP#
		inout [31:0] PCI_AD,  	// PCI address/data bus
		inout [3:0] PCI_CBE,  	// PCI command/byte enable bus
		input PCI_IDSEL,		 	// PCI IDSEL
		inout PCI_PAR,			 	// PCI PAR
		inout PCI_PERR,		 	// PCI PERR#
		output PCI_SERR,		 	// PCI SERR#
		
		input BOARD_CLOCK,			//Board_clock
		
		//SPI
		input  SPI_MISO,			//Master input 
		output SPI_MOSI,			//Master output
		output SPI_SCLK,			//SPI clock
		output [2:0] SPI_CLK_CS,		//Chip_select lines for CDCE62005
		
		//CDCE CLOCKS
		output RF_CLK3_N,
		output RF_CLK3_P,
		
		//TRG and ACK
		input [11:0] TRG,			//Trigger bits
		output [11:0] ACK			//ACK bits
    );
	
	//Wishbone Interconnect
	wire 			WBM_ACK_I;
   wire [31:0] WBM_ADR_O;
   wire [1:0] 	WBM_BTE_O;
   wire 			WBM_CAB_O;
   wire [2:0] 	WBM_CTI_O;
   wire 			WBM_CYC_O;
   wire [31:0] WBM_DAT_I;
   wire [31:0] WBM_DAT_O;
   wire 			WBM_ERR_I;
   wire 			WBM_RTY_I;
   wire [3:0] 	WBM_SEL_O;
   wire 			WBM_STB_O;
   wire 			WBM_WE_O;
   wire 			WBS_ACK_O;
   wire [31:0] WBS_ADR_I;
   wire [1:0] 	WBS_BTE_I;
   wire [2:0] 	WBS_CTI_I;
   wire [31:0] WBS_DAT_I;
   wire [31:0] WBS_DAT_O;
   wire 			WBS_ERR_O;
   wire 			WBS_RTY_O;
   wire [3:0] 	WBS_SEL_I;
   wire 			WBS_STB_I;
   wire 			WBS_WE_I;
   wire 			WB_INT;

	//SPI_INTERCONNECT
   wire [31:0]	SPI_I;
   wire [31:0] SPI_O; 
   wire 			SPI_DONE; 
   wire 			SPI_STAR;
   wire [1:0]	SPI_SEL;
	
PCI_TOP U_PCI_TOP (
    .PCI_CLK(PCI_CLK), 
    .PCI_RST(PCI_RST), 
    .PCI_INTA(PCI_INTA), 
    .PCI_REQ(PCI_REQ), 
    .PCI_GNT(PCI_GNT), 
    .PCI_FRAME(PCI_FRAME), 
    .PCI_IRDY(PCI_IRDY), 
    .PCI_DEVSEL(PCI_DEVSEL), 
    .PCI_TRDY(PCI_TRDY), 
    .PCI_STOP(PCI_STOP), 
    .PCI_AD(PCI_AD), 
    .PCI_CBE(PCI_CBE), 
    .PCI_IDSEL(PCI_IDSEL), 
    .PCI_PAR(PCI_PAR), 
    .PCI_PERR(PCI_PERR), 
    .PCI_SERR(PCI_SERR), 
    .WB_CLK(BOARD_CLOCK), 
    .WB_RST(WB_RST), 
    .WB_INT(WB_INT), 
    .WBS_ADR_I(WBS_ADR_I), 
    .WBS_DAT_I(WBS_DAT_I), 
    .WBS_DAT_O(WBS_DAT_O), 
    .WBS_SEL_I(WBS_SEL_I), 
    .WBS_CYC_I(WBS_CYC_I), 
    .WBS_STB_I(WBS_STB_I), 
    .WBS_WE_I(WBS_WE_I), 
    .WBS_CAB_I(WBS_CAB_I), 
    .WBS_CTI_I(WBS_CTI_I), 
    .WBS_BTE_I(WBS_BTE_I), 
    .WBS_ACK_O(WBS_ACK_O), 
    .WBS_RTY_O(WBS_RTY_O), 
    .WBS_ERR_O(WBS_ERR_O), 
    .WBM_ADR_O(WBM_ADR_O), 
    .WBM_DAT_I(WBM_DAT_I), 
    .WBM_DAT_O(WBM_DAT_O), 
    .WBM_SEL_O(WBM_SEL_O), 
    .WBM_CYC_O(WBM_CYC_O), 
    .WBM_STB_O(WBM_STB_O), 
    .WBM_WE_O(WBM_WE_O), 
    .WBM_CAB_O(WBM_CAB_O), 
    .WBM_CTI_O(WBM_CTI_O), 
    .WBM_BTE_O(WBM_BTE_O), 
    .WBM_ACK_I(WBM_ACK_I), 
    .WBM_RTY_I(WBM_RTY_I), 
    .WBM_ERR_I(WBM_ERR_I)
    );

//Wishbone slaves
WISHBONE_SLAVE U_WISHBONE_SLAVE (
    .clk_i(BOARD_CLOCK), 
    .reset_i(WB_RST), 
    .cyc_i(WBM_CYC_O), 
    .stb_i(WBM_STB_O), 
    .err_o(WBM_ERR_I), 
    .rty_o(WBM_RTY_I), 
    .ack_o(WBM_ACK_I), 
    .dat_i(WBM_DAT_O), 
    .dat_o(WBM_DAT_I), 
    .adr_i(WBM_ADR_O), 
    .cti_i(WBM_CTI_O), 
    .bte_i(WBM_BTE_O), 
    .we_i(WBM_WE_O), 
    .sel_i(WBM_SEL_O), 
	 //SPI
    .SPI_I(SPI_I), 
    .SPI_O(SPI_O), 
    .SPI_DONE_I(SPI_DONE), 
    .SPI_STAR_O(SPI_STAR), 
    .SPI_SEL_O(SPI_SEL),
	 //TRG ACK
	 .TRG_BITS_I(TRG), 
    .ACK_BITS_O(ACK)
    );

SPI_Master U_SPI_Master (
    .BOARD_CLOCK(BOARD_CLOCK), 
    .RST(WB_RST), 
    .SPI_MISO(SPI_MISO), 
    .SPI_MOSI(SPI_MOSI), 
    .SPI_CLK(SPI_SCLK), 
    .SPI_CSS(SPI_CLK_CS), 
    .SPI_O(SPI_I), 
    .SPI_I(SPI_O), 
    .SPI_DONE_O(SPI_DONE), 
    .SPI_STAR_I(SPI_STAR), 
    .SPI_SEL_I(SPI_SEL)
    );

// Instantiate the module
WISHBONE_MASTER U_WISHBONE_MASTER (
    .wb_clk_i(BOARD_CLOCK), 
    .wb_rst_i(WB_RST), 
    .wbm_cyc_o(WBS_CYC_I), 
    .wbm_stb_o(WBS_STB_I), 
    .wbm_sel_o(WBS_SEL_I), 
    .wbm_we_o(WBS_WE_I), 
    .wbm_adr_o(WBS_ADR_I), 
    .wbm_dat_o(WBS_DAT_I), 
    .wbm_cab_o(WBS_CAB_I), 
    .wbm_dat_i(WBS_DAT_O), 
    .wbm_ack_i(WBS_ACK_O), 
    .wbm_err_i(WBS_ERR_O), 
    .wbm_rty_i(WBS_RTY_O)
    );


OBUFDS #(
.IOSTANDARD("LVDS_25")
) OBUFDS_CLK3 (
.O(RF_CLK3_P),
.OB(RF_CLK3_N),
.I(BOARD_CLOCK)
);
endmodule
