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
		input BOARD_CLOCK,			//Board_clock		
		
		input PCI_CLK, // PCI clock
		input PCI_RST, // PCI RST#
		output PCI_INTA, // PCI INTA#
		output PCI_REQ, // PCI REQ#
		input PCI_GNT, // PCI GNT#
		inout PCI_FRAME, // PCI FRAME#
		inout PCI_IRDY, // PCI IRDY#
		inout PCI_DEVSEL, // PCI DEVSEL#
		inout PCI_TRDY, // PCI TRDY#
		inout PCI_STOP, // PCI STOP#
		inout [31:0] PCI_AD, // PCI address/data bus
		inout [3:0] PCI_CBE, // PCI command/byte enable bus
		input PCI_IDSEL, // PCI IDSEL
		inout PCI_PAR, // PCI PAR
		inout PCI_PERR, // PCI PERR#
		output PCI_SERR, // PCI SERR#
		
		input SPI_MISO,
		output SPI_MOSI,
		output SPI_SCLK,
		output [2:0] SPI_CLK_CS,

		output [2:0] SYNC_CLK,
				
		output RF_CLK3_N,
		output RF_CLK3_P,

		output RF_CLK1_N,
		output RF_CLK1_P,

		output RF_CLK2_N,
		output RF_CLK2_P,
		
		//JTAG
		output TMS,
		output TMS_EN,
		output TCK,
		output TCK_EN,

		input [11:0] TDO,
		output [11:0] TDI,

		//Trigger and Ack
		
		output [11:0] TRG,
		input [11:0] ACK,
		
		//Virtual Jtag
		input V_TDI,
		output V_TDO,
		input V_TMS,
		input V_TCK
    );
	 
wire CLK_80MHZ;
wire CLK_20MHZ;
wire CLK_1MHZ;

OBUFDS #(
	.IOSTANDARD("LVDS_25")
) OBUFDS_CLK3 (
	.O(RF_CLK3_P),
	.OB(RF_CLK3_N),
	.I(CLK_20MHZ)
	);

OBUFDS #(
	.IOSTANDARD("LVDS_25")
) OBUFDS_CLK2 (
	.O(RF_CLK2_P),
	.OB(RF_CLK2_N),
	.I(CLK_20MHZ)
	);

OBUFDS #(
	.IOSTANDARD("LVDS_25")
) OBUFDS_CLK1 (
	.O(RF_CLK1_P),
	.OB(RF_CLK1_N),
	.I(CLK_20MHZ)
	);
	
//CLOCK
CLOCKS u_clocks (
		.BOARD_CLOCK(BOARD_CLOCK), 
		.RST(0),
		.CLK_80MHZ(CLK_80MHZ), 
		.CLK_20MHZ(CLK_20MHZ), 
		.CLK_1MHZ(CLK_1MHZ), 
		.LOCKED(LOCKED)
		);

//SPI for jitter cleaners
wire [1:0] SPI_SLAVE_SELECT;
wire [31:0] SPI_IN;
wire [31:0] SPI_OUT;
wire SPI_GO;
wire SPI_DONE;
wire SYNC;

assign SYNC_CLK[0] = ~SYNC;
assign SYNC_CLK[1] = ~SYNC;
assign SYNC_CLK[2] = ~SYNC;

SPI_MUX u_spi_mux (
    .SPI_CS_OUT(SPI_CLK_CS), 
    .SPI_CS_IN(SPI_CS_IN), 
    .SLAVE_SELECT(SPI_SLAVE_SELECT)
    );

SPI_MODULE u_spi_module (
    .CLOCK(CLK_1MHZ), 
    .SPI_MISO(SPI_MISO), 
    .SPI_MOSI(SPI_MOSI), 
    .SPI_CS(SPI_CS_IN), 
    .SPI_SCLK(SPI_SCLK), 
    .SPI_IN(SPI_IN), 
    .SPI_OUT(SPI_OUT), 
    .GO(SPI_GO), 
    .DONE(SPI_DONE)
    );

//JTAG
wire [3:0] JTAG_MUX_SEL;

assign TMS_EN = 1;
assign TCK_EN = 1;

JTAG_MUX U_JTAG_MUX (
	.TDO(TDO),
	.TDI(TDI),
	.TMS(TMS),
	.TCK(TCK),
	.JTAG_SEL(JTAG_MUX_SEL),
	.V_TDI(V_TDI),
	.V_TDO(V_TDO),
	.V_TMS(V_TMS),
	.V_TCK(V_TCK)
	);
	
//Trigger
wire [11:0] TRG_MASK;
wire [3:0] MIN_SCRODS_REQUIRED;
wire [31:0] TRG_STATISTICS;
wire TRG_SOFT;

TRIG U_TRG(
    .CLK_80MHZ(CLK_80MHZ),
	 .RESET(0),
    .TRG(TRG), 
    .ACK(ACK), 
	 .TRG_MASK(TRG_MASK), 
    .MIN_SCRODS_REQUIRED(MIN_SCRODS_REQUIRED), 
    .TRG_STATISTICS(TRG_STATISTICS),
	 .TRG_SOFT(TRG_SOFT)
    );

//Debug
wire [35:0] CONTROL0;
wire [35:0] CONTROL1;

icon u_icon (
    .CONTROL0(CONTROL0), // INOUT BUS [35:0]
    .CONTROL1(CONTROL1) // INOUT BUS [35:0]
);

ila u_ila1 (
    .CONTROL(CONTROL0), // INOUT BUS [35:0]
    .CLK(CLK_80MHZ), // IN
    .TRIG0(ACK[7:0]) // IN BUS [7:0]
);

ila u_ila2 (
    .CONTROL(CONTROL1), // INOUT BUS [35:0]
    .CLK(CLK_80MHZ), // IN
    .TRIG0({ACK[11:8], TRG[0]}) // IN BUS [7:0]
);

//Wishbone Interconnect
wire WBM_ACK_I;
wire [31:0] WBM_ADR_O;
wire [1:0] WBM_BTE_O;
wire WBM_CAB_O;
wire [2:0] WBM_CTI_O;
wire WBM_CYC_O;
wire [31:0] WBM_DAT_I;
wire [31:0] WBM_DAT_O;
wire WBM_ERR_I;
wire WBM_RTY_I;
wire [3:0] WBM_SEL_O;
wire WBM_STB_O;
wire WBM_WE_O;
wire WBS_ACK_O;
wire [31:0] WBS_ADR_I;
wire [1:0] WBS_BTE_I;
wire [2:0] WBS_CTI_I;
wire [31:0] WBS_DAT_I;
wire [31:0] WBS_DAT_O;
wire WBS_ERR_O;
wire WBS_RTY_O;
wire [3:0] WBS_SEL_I;
wire WBS_STB_I;
wire WBS_WE_I;
wire WB_INT;
	
//PCI Controll
WISHBONE_SLAVE U_WISHBONE_SLAVE (
	.clk_i(CLK_80MHZ),
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
	.SPI_I(SPI_OUT),
	.SPI_O(SPI_IN),
	.SPI_DONE_I(SPI_DONE),
	.SPI_START_O(SPI_GO),
	.SPI_SEL_O(SPI_SLAVE_SELECT),
	//JTAG
	.JTAG_MUX_SEL(JTAG_MUX_SEL),
	.SYNC(SYNC),
	//TRIGGER
	.TRG_MASK(TRG_MASK), 
	.MIN_SCRODS_REQUIRED(MIN_SCRODS_REQUIRED), 
	.TRG_STATISTICS(TRG_STATISTICS), 
	.TRG_SOFT(TRG_SOFT)
	);

//PCI MASTER
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
	.WB_CLK(CLK_80MHZ),
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
//Wishbone master (blank)
WISHBONE_MASTER U_WISHBONE_MASTER (
.wb_clk_i(CLK_80MHZ),
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

endmodule
