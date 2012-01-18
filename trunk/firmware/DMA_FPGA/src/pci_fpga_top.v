////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 1995-2011 Xilinx, Inc.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//   ____  ____ 
//  /   /\/   / 
// /___/  \  /    Vendor: Xilinx 
// \   \   \/     Version : 13.1
//  \   \         Application : sch2hdl
//  /   /         Filename : DMA_FPGA.vf
// /___/   /\     Timestamp : 07/16/2011 00:54:54
// \   \  /  \ 
//  \___\/\___\ 
//
//Command: sch2hdl -sympath /home/zihangao/DATA_NT/Work/cPCI/firmware/src/cores/DMA_FPGA_cores -intstyle ise -family spartan6 -verilog /home/zihangao/DATA_NT/Work/cPCI/firmware/par/DMA_FPGA.vf -w /home/zihangao/DATA_NT/Work/cPCI/firmware/src/DMA/DMA_FPGA.sch
//Design Name: DMA_FPGA
//Device: spartan6
//Purpose:
//    This verilog netlist is translated from an ECS schematic.It can be 
//    synthesized and simulated, but it should not be modified. 
//
`timescale 1ns / 1ps

module DMA_FPGA(bc0, 
                bc1, 
                bc2, 
                bc3, 
                BOARD_IDENT, 
                BUS_M_RDY_PAD_A, 
                BUS_M_RDY_PAD_B, 
                BUS_STB_PAD_A, 
                BUS_STB_PAD_B, 
                BUS_WE_PAD_A, 
                BUS_WE_PAD_B, 
                channel_0_up_pad, 
                channel_1_up_pad, 
                channel_2_up_pad, 
                channel_3_up_pad, 
                PCI_GNT, 
                PCI_IDSEL, 
                PCI_RST, 
                PCLK, 
                BUS_ABORT_PAD_A, 
                BUS_ABORT_PAD_B, 
                BUS_ACK_PAD_A, 
                BUS_ACK_PAD_B, 
                BUS_REQ_R_1_PAD_A, 
                BUS_REQ_R_1_PAD_B, 
                BUS_REQ_R_2_PAD_A, 
                BUS_REQ_R_2_PAD_B, 
                BUS_REQ_W_1_PAD_A, 
                BUS_REQ_W_1_PAD_B, 
                BUS_REQ_W_2_PAD_A, 
                BUS_REQ_W_2_PAD_B, 
                BUS_S_RDY_PAD_A, 
                BUS_S_RDY_PAD_B, 
                CLOCK_PAD_A, 
                CLOCK_PAD_B, 
                LED_GREEN, 
                LED_RED, 
                PCI_INTA, 
                PCI_REQ, 
                PCI_SERR, 
                pulse_A, 
                pulse_B, 
                rst_channel_0_pad, 
                rst_channel_1_pad, 
                rst_channel_2_pad, 
                rst_channel_3_pad, 
                BUS_AD_PAD_A, 
                BUS_AD_PAD_B, 
                PCI_AD, 
                PCI_CBE, 
                PCI_DEVSEL, 
                PCI_FRAME, 
                PCI_IRDY, 
                PCI_PAR, 
                PCI_PERR, 
                PCI_STOP, 
                PCI_TRDY);

    input bc0;
    input bc1;
    input bc2;
    input bc3;
    input [5:0] BOARD_IDENT;
    input BUS_M_RDY_PAD_A;
    input BUS_M_RDY_PAD_B;
    input BUS_STB_PAD_A;
    input BUS_STB_PAD_B;
    input BUS_WE_PAD_A;
    input BUS_WE_PAD_B;
    input channel_0_up_pad;
    input channel_1_up_pad;
    input channel_2_up_pad;
    input channel_3_up_pad;
    input PCI_GNT;
    input PCI_IDSEL;
    input PCI_RST;
    input PCLK;
   output BUS_ABORT_PAD_A;
   output BUS_ABORT_PAD_B;
   output BUS_ACK_PAD_A;
   output BUS_ACK_PAD_B;
   output BUS_REQ_R_1_PAD_A;
   output BUS_REQ_R_1_PAD_B;
   output BUS_REQ_R_2_PAD_A;
   output BUS_REQ_R_2_PAD_B;
   output BUS_REQ_W_1_PAD_A;
   output BUS_REQ_W_1_PAD_B;
   output BUS_REQ_W_2_PAD_A;
   output BUS_REQ_W_2_PAD_B;
   output BUS_S_RDY_PAD_A;
   output BUS_S_RDY_PAD_B;
   output CLOCK_PAD_A;
   output CLOCK_PAD_B;
   output LED_GREEN;
   output LED_RED;
   output PCI_INTA;
   output PCI_REQ;
   output PCI_SERR;
   output pulse_A;
   output pulse_B;
   output rst_channel_0_pad;
   output rst_channel_1_pad;
   output rst_channel_2_pad;
   output rst_channel_3_pad;
    inout [31:0] BUS_AD_PAD_A;
    inout [31:0] BUS_AD_PAD_B;
    inout [31:0] PCI_AD;
    inout [3:0] PCI_CBE;
    inout PCI_DEVSEL;
    inout PCI_FRAME;
    inout PCI_IRDY;
    inout PCI_PAR;
    inout PCI_PERR;
    inout PCI_STOP;
    inout PCI_TRDY;
   
   wire bc0_in;
   wire bc1_in;
   wire bc2_in;
   wire bc3_in;
   wire [5:0] BOARD_IDENT_IN;
   wire bus_abort_A;
   wire bus_abort_B;
   wire bus_m_rdy_A;
   wire bus_m_rdy_B;
   wire bus_s_rdy_A;
   wire bus_s_rdy_B;
   wire channel_0_up;
   wire channel_1_up;
   wire channel_2_up;
   wire channel_3_up;
   wire [35:0] CONTROL0;
   wire [35:0] CONTROL1;
   wire [35:0] CONTROL2;
   wire [35:0] CONTROL3;
   wire [35:0] CONTROL4;
   wire [35:0] CONTROL5;
   wire [27:0] DMA_fifo_cnt_A1;
   wire [27:0] DMA_fifo_cnt_A2;
   wire [27:0] DMA_fifo_cnt_B1;
   wire [27:0] DMA_fifo_cnt_B2;
   wire [31:0] DMA_fifo_dat_A1;
   wire [31:0] DMA_fifo_dat_A2;
   wire [31:0] DMA_fifo_dat_B1;
   wire [31:0] DMA_fifo_dat_B2;
   wire DMA_fifo_dat_strobe_A1;
   wire DMA_fifo_dat_strobe_A2;
   wire DMA_fifo_dat_strobe_B1;
   wire DMA_fifo_dat_strobe_B2;
   wire DMA_fifo_dat_strobe_current_A1;
   wire DMA_fifo_dat_strobe_current_A2;
   wire DMA_fifo_dat_strobe_current_B1;
   wire DMA_fifo_dat_strobe_current_B2;
   wire DMA_fifo_rd_A1;
   wire DMA_fifo_rd_A2;
   wire DMA_fifo_rd_B1;
   wire DMA_fifo_rd_B2;
   wire dma_in_use;
   wire DMA_merge_fifo_prog_full_A1;
   wire DMA_merge_fifo_prog_full_A2;
   wire DMA_merge_fifo_prog_full_B1;
   wire DMA_merge_fifo_prog_full_B2;
   wire [31:0] DMA_merge_fifo_wr_dat_A1;
   wire [31:0] DMA_merge_fifo_wr_dat_A2;
   wire [31:0] DMA_merge_fifo_wr_dat_B1;
   wire [31:0] DMA_merge_fifo_wr_dat_B2;
   wire DMA_merge_fifo_wr_en_A1;
   wire DMA_merge_fifo_wr_en_A2;
   wire DMA_merge_fifo_wr_en_B1;
   wire DMA_merge_fifo_wr_en_B2;
   wire down_fifo_almost_empty_A1;
   wire down_fifo_almost_empty_A2;
   wire down_fifo_almost_empty_B1;
   wire down_fifo_almost_empty_B2;
   wire down_fifo_empty_A1;
   wire down_fifo_empty_A2;
   wire down_fifo_empty_B1;
   wire down_fifo_empty_B2;
   wire [31:0] down_fifo_rdat_A1;
   wire [31:0] down_fifo_rdat_A2;
   wire [31:0] down_fifo_rdat_B1;
   wire [31:0] down_fifo_rdat_B2;
   wire down_fifo_rd_A1;
   wire down_fifo_rd_A2;
   wire down_fifo_rd_B1;
   wire down_fifo_rd_B2;
   wire [31:0] fifo_data_A1;
   wire [31:0] fifo_data_A2;
   wire [31:0] fifo_data_A3;
   wire [31:0] fifo_data_A4;
   wire fifo_data_valid_A1;
   wire fifo_data_valid_A2;
   wire fifo_data_valid_A3;
   wire fifo_data_valid_A4;
   wire [27:0] fifo_length_A1;
   wire [27:0] fifo_length_A2;
   wire [27:0] fifo_length_B1;
   wire [27:0] fifo_length_B2;
   wire fifo_prog_full_A1;
   wire fifo_prog_full_A2;
   wire fifo_prog_full_A3;
   wire fifo_prog_full_A4;
   wire LED_GREENX;
   wire PCI_CLK;
   wire rst_channel_0;
   wire rst_channel_1;
   wire rst_channel_2;
   wire rst_channel_3;
   wire slave_ack_A;
   wire slave_ack_B;
   wire slave_dat_en_A;
   wire slave_dat_en_B;
   wire [31:0] slave_dat_i_A;
   wire [31:0] slave_dat_i_B;
   wire [31:0] slave_dat_o_A;
   wire [31:0] slave_dat_o_B;
   wire slave_req_r_1_A;
   wire slave_req_r_1_B;
   wire slave_req_r_2_A;
   wire slave_req_r_2_B;
   wire slave_req_w_A1;
   wire slave_req_w_A2;
   wire slave_req_w_B1;
   wire slave_req_w_B2;
   wire slave_stb_A;
   wire slave_stb_B;
   wire slave_we_A;
   wire slave_we_B;
   wire WB_CLK;
   wire WB_CLK_2x;
   wire WB_PCI_RST;
   wire WB_RST;
   wire WB_RST_DELAY;
   wire XLXN_7;
   wire XLXN_11;
   wire XLXN_14;
   wire XLXN_15;
   
   down_fifo_interface_debug  down_fifo_bus_A_debug (.clk_i(WB_CLK_2x), 
                                                    
         .fifo_dat_1(DMA_merge_fifo_wr_dat_A1[31:0]), 
                                                    
         .fifo_dat_2(down_fifo_rdat_A1[31:0]), 
                                                    
         .fifo_empty_1(DMA_merge_fifo_prog_full_A1), 
                                                    
         .fifo_empty_2(down_fifo_empty_A1), 
                                                    
         .fifo_rd_1(DMA_merge_fifo_wr_en_A1), 
                                                    
         .fifo_rd_2(down_fifo_rd_A1), 
                                                    .reset_i(WB_RST_DELAY), 
                                                    .control(CONTROL4[35:0]));
   down_fifo_interface_debug  down_fifo_dma_A_debug (.clk_i(WB_CLK), 
                                                    
         .fifo_dat_1(fifo_data_A1[31:0]), 
                                                    
         .fifo_dat_2(DMA_fifo_dat_A1[31:0]), 
                                                    
         .fifo_empty_1(fifo_prog_full_A1), 
                                                    
         .fifo_empty_2(DMA_fifo_dat_strobe_current_A1), 
                                                    
         .fifo_rd_1(fifo_data_valid_A1), 
                                                    .fifo_rd_2(DMA_fifo_rd_A1), 
                                                    .reset_i(WB_RST_DELAY), 
                                                    .control(CONTROL5[35:0]));
   bus_slave  u_bus_slave_A (.clk_i(WB_CLK_2x), 
                            .dat_i(slave_dat_i_A[31:0]), 
                            .m_rdy_i(bus_m_rdy_A), 
                            .reset_i(WB_RST_DELAY), 
                            
         .r_fifo_almost_empty_i_1(down_fifo_almost_empty_A1), 
                            
         .r_fifo_almost_empty_i_2(down_fifo_almost_empty_A2), 
                            .r_fifo_dat_i_1(down_fifo_rdat_A1[31:0]), 
                            .r_fifo_dat_i_2(down_fifo_rdat_A2[31:0]), 
                            .r_fifo_empty_i_1(down_fifo_empty_A1), 
                            .r_fifo_empty_i_2(down_fifo_empty_A2), 
                            .r_fifo_prog_empty_i_1(), 
                            .r_fifo_prog_empty_i_2(), 
                            .stb_i(slave_stb_A), 
                            .we_i(slave_we_A), 
                            .w_fifo_almost_full_i_1(), 
                            .w_fifo_almost_full_i_2(), 
                            .w_fifo_full_i_1(), 
                            .w_fifo_full_i_2(), 
                            .w_fifo_prog_full_i_1(DMA_merge_fifo_prog_full_A1), 
                            .w_fifo_prog_full_i_2(DMA_merge_fifo_prog_full_A2), 
                            .abort_o(bus_abort_A), 
                            .ack_o(slave_ack_A), 
                            .dat_o(slave_dat_o_A[31:0]), 
                            .dat_o_enable_o(slave_dat_en_A), 
                            .req_r_o_1(slave_req_r_1_A), 
                            .req_r_o_2(slave_req_r_2_A), 
                            .req_w_o_1(slave_req_w_A1), 
                            .req_w_o_2(slave_req_w_A2), 
                            .r_fifo_rd_o_1(down_fifo_rd_A1), 
                            .r_fifo_rd_o_2(down_fifo_rd_A2), 
                            .s_rdy_o(bus_s_rdy_A), 
                            .w_fifo_dat_o_1(DMA_merge_fifo_wr_dat_A1[31:0]), 
                            .w_fifo_dat_o_2(DMA_merge_fifo_wr_dat_A2[31:0]), 
                            .w_fifo_dat_valid_o_1(DMA_merge_fifo_wr_en_A1), 
                            .w_fifo_dat_valid_o_2(DMA_merge_fifo_wr_en_A2));
   bus_slave  u_bus_slave_B (.clk_i(WB_CLK_2x), 
                            .dat_i(slave_dat_i_B[31:0]), 
                            .m_rdy_i(bus_m_rdy_B), 
                            .reset_i(WB_RST_DELAY), 
                            
         .r_fifo_almost_empty_i_1(down_fifo_almost_empty_B1), 
                            
         .r_fifo_almost_empty_i_2(down_fifo_almost_empty_B2), 
                            .r_fifo_dat_i_1(down_fifo_rdat_B1[31:0]), 
                            .r_fifo_dat_i_2(down_fifo_rdat_B2[31:0]), 
                            .r_fifo_empty_i_1(down_fifo_empty_B1), 
                            .r_fifo_empty_i_2(down_fifo_empty_B2), 
                            .r_fifo_prog_empty_i_1(), 
                            .r_fifo_prog_empty_i_2(), 
                            .stb_i(slave_stb_B), 
                            .we_i(slave_we_B), 
                            .w_fifo_almost_full_i_1(), 
                            .w_fifo_almost_full_i_2(), 
                            .w_fifo_full_i_1(), 
                            .w_fifo_full_i_2(), 
                            .w_fifo_prog_full_i_1(DMA_merge_fifo_prog_full_B1), 
                            .w_fifo_prog_full_i_2(DMA_merge_fifo_prog_full_B2), 
                            .abort_o(bus_abort_B), 
                            .ack_o(slave_ack_B), 
                            .dat_o(slave_dat_o_B[31:0]), 
                            .dat_o_enable_o(slave_dat_en_B), 
                            .req_r_o_1(slave_req_r_1_B), 
                            .req_r_o_2(slave_req_r_2_B), 
                            .req_w_o_1(slave_req_w_B1), 
                            .req_w_o_2(slave_req_w_B2), 
                            .r_fifo_rd_o_1(down_fifo_rd_B1), 
                            .r_fifo_rd_o_2(down_fifo_rd_B2), 
                            .s_rdy_o(bus_s_rdy_B), 
                            .w_fifo_dat_o_1(DMA_merge_fifo_wr_dat_B1[31:0]), 
                            .w_fifo_dat_o_2(DMA_merge_fifo_wr_dat_B2[31:0]), 
                            .w_fifo_dat_valid_o_1(DMA_merge_fifo_wr_en_B1), 
                            .w_fifo_dat_valid_o_2(DMA_merge_fifo_wr_en_B2));
   clk_gen_top  u_clk_gen_top (.CLK_IN1(PCLK), 
                              .RESET(WB_PCI_RST), 
                              .CLK_OUT1(), 
                              .CLK_OUT2(), 
                              .LOCKED(), 
                              .PCI_CLK(PCI_CLK), 
                              .WB_CLK(WB_CLK), 
                              .WB_CLK_2x(WB_CLK_2x), 
                              .WB_RST(WB_RST), 
                              .WB_RST_DELAY(WB_RST_DELAY));
   DMA_FIFOs  u_DMA_FIFOs (.DMA_fifo_rd_A1(DMA_fifo_rd_A1), 
                                         .DMA_fifo_rd_A2(DMA_fifo_rd_A2), 
                                         .DMA_fifo_rd_B1(DMA_fifo_rd_B1), 
                                         .DMA_fifo_rd_B2(DMA_fifo_rd_B2), 
                                         
         .DMA_merge_fifo_wr_dat_A1(DMA_merge_fifo_wr_dat_A1[31:0]), 
                                         
         .DMA_merge_fifo_wr_dat_A2(DMA_merge_fifo_wr_dat_A2[31:0]), 
                                         
         .DMA_merge_fifo_wr_dat_B1(DMA_merge_fifo_wr_dat_B1[31:0]), 
                                         
         .DMA_merge_fifo_wr_dat_B2(DMA_merge_fifo_wr_dat_B2[31:0]), 
                                         
         .DMA_merge_fifo_wr_en_A1(DMA_merge_fifo_wr_en_A1), 
                                         
         .DMA_merge_fifo_wr_en_A2(DMA_merge_fifo_wr_en_A2), 
                                         
         .DMA_merge_fifo_wr_en_B1(DMA_merge_fifo_wr_en_B1), 
                                         
         .DMA_merge_fifo_wr_en_B2(DMA_merge_fifo_wr_en_B2), 
                                         .down_fifo_rd_A1(down_fifo_rd_A1), 
                                         .down_fifo_rd_A2(down_fifo_rd_A2), 
                                         .down_fifo_rd_B1(down_fifo_rd_B1), 
                                         .down_fifo_rd_B2(down_fifo_rd_B2), 
                                         .fifo_data_A1(fifo_data_A1[31:0]), 
                                         .fifo_data_A2(fifo_data_A2[31:0]), 
                                         .fifo_data_B1(fifo_data_A3[31:0]), 
                                         .fifo_data_B2(fifo_data_A4[31:0]), 
                                         
         .fifo_data_valid_A1(fifo_data_valid_A1), 
                                         
         .fifo_data_valid_A2(fifo_data_valid_A2), 
                                         
         .fifo_data_valid_B1(fifo_data_valid_A3), 
                                         
         .fifo_data_valid_B2(fifo_data_valid_A4), 
                                         .rst_channel_0(rst_channel_0), 
                                         .rst_channel_1(rst_channel_1), 
                                         .rst_channel_2(rst_channel_2), 
                                         .rst_channel_3(rst_channel_3), 
                                         .WB_CLK(WB_CLK), 
                                         .WB_CLK_2x(WB_CLK_2x), 
                                         .WB_RST(WB_RST), 
                                         
         .DMA_fifo_cnt_A1(DMA_fifo_cnt_A1[27:0]), 
                                         
         .DMA_fifo_cnt_A2(DMA_fifo_cnt_A2[27:0]), 
                                         
         .DMA_fifo_cnt_B1(DMA_fifo_cnt_B1[27:0]), 
                                         
         .DMA_fifo_cnt_B2(DMA_fifo_cnt_B2[27:0]), 
                                         
         .DMA_fifo_dat_A1(DMA_fifo_dat_A1[31:0]), 
                                         
         .DMA_fifo_dat_A2(DMA_fifo_dat_A2[31:0]), 
                                         
         .DMA_fifo_dat_B1(DMA_fifo_dat_B1[31:0]), 
                                         
         .DMA_fifo_dat_B2(DMA_fifo_dat_B2[31:0]), 
                                         
         .DMA_fifo_dat_strobe_A1(DMA_fifo_dat_strobe_A1), 
                                         
         .DMA_fifo_dat_strobe_A2(DMA_fifo_dat_strobe_A2), 
                                         
         .DMA_fifo_dat_strobe_B1(DMA_fifo_dat_strobe_B1), 
                                         
         .DMA_fifo_dat_strobe_B2(DMA_fifo_dat_strobe_B2), 
                                         
         .DMA_fifo_dat_strobe_current_A1(DMA_fifo_dat_strobe_current_A1), 
                                         
         .DMA_fifo_dat_strobe_current_A2(DMA_fifo_dat_strobe_current_A2), 
                                         
         .DMA_fifo_dat_strobe_current_B1(DMA_fifo_dat_strobe_current_B1), 
                                         
         .DMA_fifo_dat_strobe_current_B2(DMA_fifo_dat_strobe_current_B2), 
                                         
         .DMA_merge_fifo_prog_full_A1(DMA_merge_fifo_prog_full_A1), 
                                         
         .DMA_merge_fifo_prog_full_A2(DMA_merge_fifo_prog_full_A2), 
                                         
         .DMA_merge_fifo_prog_full_B1(DMA_merge_fifo_prog_full_B1), 
                                         
         .DMA_merge_fifo_prog_full_B2(DMA_merge_fifo_prog_full_B2), 
                                         
         .down_fifo_almost_empty_A1(down_fifo_almost_empty_A1), 
                                         
         .down_fifo_almost_empty_A2(down_fifo_almost_empty_A2), 
                                         
         .down_fifo_almost_empty_B1(down_fifo_almost_empty_B1), 
                                         
         .down_fifo_almost_empty_B2(down_fifo_almost_empty_B2), 
                                         
         .down_fifo_empty_A1(down_fifo_empty_A1), 
                                         
         .down_fifo_empty_A2(down_fifo_empty_A2), 
                                         
         .down_fifo_empty_B1(down_fifo_empty_B1), 
                                         
         .down_fifo_empty_B2(down_fifo_empty_B2), 
                                         .down_fifo_prog_empty_A1(), 
                                         .down_fifo_prog_empty_A2(), 
                                         .down_fifo_prog_empty_B1(), 
                                         .down_fifo_prog_empty_B2(), 
                                         
         .down_fifo_rdat_A1(down_fifo_rdat_A1[31:0]), 
                                         
         .down_fifo_rdat_A2(down_fifo_rdat_A2[31:0]), 
                                         
         .down_fifo_rdat_B1(down_fifo_rdat_B1[31:0]), 
                                         
         .down_fifo_rdat_B2(down_fifo_rdat_B2[31:0]), 
                                         .fifo_length_A1(fifo_length_A1[27:0]), 
                                         .fifo_length_A2(fifo_length_A2[27:0]), 
                                         .fifo_length_B1(fifo_length_B1[27:0]), 
                                         .fifo_length_B2(fifo_length_B2[27:0]), 
                                         .fifo_prog_full_A1(fifo_prog_full_A1), 
                                         .fifo_prog_full_A2(fifo_prog_full_A2), 
                                         .fifo_prog_full_B1(fifo_prog_full_A3), 
                                         .fifo_prog_full_B2(fifo_prog_full_A4));
   DMA_FPGA_IOBUF  u_DMA_FPGA_IOBUF_A (.bus_abort(bus_abort_A), 
                                      .bus_ack(slave_ack_A), 
                                      .bus_ad_out(slave_dat_o_A[31:0]), 
                                      .bus_ad_out_en(slave_dat_en_A), 
                                      .BUS_M_RDY_PAD(BUS_M_RDY_PAD_A), 
                                      .bus_req_r_1(slave_req_r_1_A), 
                                      .bus_req_r_2(slave_req_r_2_A), 
                                      .bus_req_w_1(slave_req_w_A1), 
                                      .bus_req_w_2(slave_req_w_A2), 
                                      .BUS_STB_PAD(BUS_STB_PAD_A), 
                                      .bus_s_rdy(bus_s_rdy_A), 
                                      .BUS_WE_PAD(BUS_WE_PAD_A), 
                                      .clk_i(WB_CLK_2x), 
                                      .clock(WB_CLK_2x), 
                                      .BUS_ABORT_PAD(BUS_ABORT_PAD_A), 
                                      .BUS_ACK_PAD(BUS_ACK_PAD_A), 
                                      .bus_ad_in(slave_dat_i_A[31:0]), 
                                      .bus_m_rdy(bus_m_rdy_A), 
                                      .BUS_REQ_R_1_PAD(BUS_REQ_R_1_PAD_A), 
                                      .BUS_REQ_R_2_PAD(BUS_REQ_R_2_PAD_A), 
                                      .BUS_REQ_W_1_PAD(BUS_REQ_W_1_PAD_A), 
                                      .BUS_REQ_W_2_PAD(BUS_REQ_W_2_PAD_A), 
                                      .bus_stb(slave_stb_A), 
                                      .BUS_S_RDY_PAD(BUS_S_RDY_PAD_A), 
                                      .bus_we(slave_we_A), 
                                      .CLOCK_PAD(CLOCK_PAD_A), 
                                      .BUS_AD_PAD(BUS_AD_PAD_A[31:0]));
   DMA_FPGA_IOBUF  u_DMA_FPGA_IOBUF_B (.bus_abort(bus_abort_B), 
                                      .bus_ack(slave_ack_B), 
                                      .bus_ad_out(slave_dat_o_B[31:0]), 
                                      .bus_ad_out_en(slave_dat_en_B), 
                                      .BUS_M_RDY_PAD(BUS_M_RDY_PAD_B), 
                                      .bus_req_r_1(slave_req_r_1_B), 
                                      .bus_req_r_2(slave_req_r_2_B), 
                                      .bus_req_w_1(slave_req_w_B1), 
                                      .bus_req_w_2(slave_req_w_B2), 
                                      .BUS_STB_PAD(BUS_STB_PAD_B), 
                                      .bus_s_rdy(bus_s_rdy_B), 
                                      .BUS_WE_PAD(BUS_WE_PAD_B), 
                                      .clk_i(WB_CLK_2x), 
                                      .clock(WB_CLK_2x), 
                                      .BUS_ABORT_PAD(BUS_ABORT_PAD_B), 
                                      .BUS_ACK_PAD(BUS_ACK_PAD_B), 
                                      .bus_ad_in(slave_dat_i_B[31:0]), 
                                      .bus_m_rdy(bus_m_rdy_B), 
                                      .BUS_REQ_R_1_PAD(BUS_REQ_R_1_PAD_B), 
                                      .BUS_REQ_R_2_PAD(BUS_REQ_R_2_PAD_B), 
                                      .BUS_REQ_W_1_PAD(BUS_REQ_W_1_PAD_B), 
                                      .BUS_REQ_W_2_PAD(BUS_REQ_W_2_PAD_B), 
                                      .bus_stb(slave_stb_B), 
                                      .BUS_S_RDY_PAD(BUS_S_RDY_PAD_B), 
                                      .bus_we(slave_we_B), 
                                      .CLOCK_PAD(CLOCK_PAD_B), 
                                      .BUS_AD_PAD(BUS_AD_PAD_B[31:0]));
   DMA_INTERFACE  u_DMA_INTERFACE (.bc0(bc0_in), 
                                                 .bc1(bc1_in), 
                                                 .bc2(bc2_in), 
                                                 .bc3(bc3_in), 
                                                 
         .BOARD_IDENT(BOARD_IDENT_IN[5:0]), 
                                                 .channel_0_up(channel_0_up), 
                                                 .channel_1_up(channel_1_up), 
                                                 .channel_2_up(channel_2_up), 
                                                 .channel_3_up(channel_3_up), 
                                                 
         .DMA_fifo_cnt_A1(DMA_fifo_cnt_A1[27:0]), 
                                                 
         .DMA_fifo_cnt_A2(DMA_fifo_cnt_A2[27:0]), 
                                                 
         .DMA_fifo_cnt_B1(DMA_fifo_cnt_B1[27:0]), 
                                                 
         .DMA_fifo_cnt_B2(DMA_fifo_cnt_B2[27:0]), 
                                                 
         .DMA_fifo_dat_A1(DMA_fifo_dat_A1[31:0]), 
                                                 
         .DMA_fifo_dat_A2(DMA_fifo_dat_A2[31:0]), 
                                                 
         .DMA_fifo_dat_B1(DMA_fifo_dat_B1[31:0]), 
                                                 
         .DMA_fifo_dat_B2(DMA_fifo_dat_B2[31:0]), 
                                                 
         .DMA_fifo_dat_strobe_A1(DMA_fifo_dat_strobe_A1), 
                                                 
         .DMA_fifo_dat_strobe_A2(DMA_fifo_dat_strobe_A2), 
                                                 
         .DMA_fifo_dat_strobe_B1(DMA_fifo_dat_strobe_B1), 
                                                 
         .DMA_fifo_dat_strobe_B2(DMA_fifo_dat_strobe_B2), 
                                                 
         .DMA_fifo_dat_strobe_current_A1(DMA_fifo_dat_strobe_current_A1), 
                                                 
         .DMA_fifo_dat_strobe_current_A2(DMA_fifo_dat_strobe_current_A2), 
                                                 
         .DMA_fifo_dat_strobe_current_B1(DMA_fifo_dat_strobe_current_B1), 
                                                 
         .DMA_fifo_dat_strobe_current_B2(DMA_fifo_dat_strobe_current_B2), 
                                                 
         .fifo_length_A1(fifo_length_A1[27:0]), 
                                                 
         .fifo_length_A2(fifo_length_A2[27:0]), 
                                                 
         .fifo_length_B1(fifo_length_B1[27:0]), 
                                                 
         .fifo_length_B2(fifo_length_B2[27:0]), 
                                                 
         .fifo_prog_full_A1(fifo_prog_full_A1), 
                                                 
         .fifo_prog_full_A2(fifo_prog_full_A2), 
                                                 
         .fifo_prog_full_A3(fifo_prog_full_A3), 
                                                 
         .fifo_prog_full_A4(fifo_prog_full_A4), 
                                                 .PCI_CLK(PCI_CLK), 
                                                 .PCI_GNT(PCI_GNT), 
                                                 .PCI_IDSEL(PCI_IDSEL), 
                                                 .PCI_RST(PCI_RST), 
                                                 .WB_CLK(WB_CLK), 
                                                 .WB_RST_DELAY(WB_RST_DELAY), 
                                                 
         .DMA_fifo_rd_A1(DMA_fifo_rd_A1), 
                                                 
         .DMA_fifo_rd_A2(DMA_fifo_rd_A2), 
                                                 
         .DMA_fifo_rd_B1(DMA_fifo_rd_B1), 
                                                 
         .DMA_fifo_rd_B2(DMA_fifo_rd_B2), 
                                                 
         .fifo_data_A1(fifo_data_A1[31:0]), 
                                                 
         .fifo_data_A2(fifo_data_A2[31:0]), 
                                                 
         .fifo_data_A3(fifo_data_A3[31:0]), 
                                                 
         .fifo_data_A4(fifo_data_A4[31:0]), 
                                                 
         .fifo_data_valid_A1(fifo_data_valid_A1), 
                                                 
         .fifo_data_valid_A2(fifo_data_valid_A2), 
                                                 
         .fifo_data_valid_A3(fifo_data_valid_A3), 
                                                 
         .fifo_data_valid_A4(fifo_data_valid_A4), 
                                                 .PCI_INTA(PCI_INTA), 
                                                 .PCI_REQ(PCI_REQ), 
                                                 .PCI_SERR(PCI_SERR), 
                                                 .pulse_A(XLXN_14), 
                                                 .pulse_B(XLXN_15), 
                                                 .rst_channel_0(rst_channel_0), 
                                                 .rst_channel_1(rst_channel_1), 
                                                 .rst_channel_2(rst_channel_2), 
                                                 .rst_channel_3(rst_channel_3), 
                                                 .WBS_CYC_I(dma_in_use), 
                                                 .WB_PCI_RST(WB_PCI_RST), 
                                                 .CONTROL0(CONTROL0[35:0]), 
                                                 .CONTROL1(CONTROL1[35:0]), 
                                                 .PCI_AD(PCI_AD[31:0]), 
                                                 .PCI_CBE(PCI_CBE[3:0]), 
                                                 .PCI_DEVSEL(PCI_DEVSEL), 
                                                 .PCI_FRAME(PCI_FRAME), 
                                                 .PCI_IRDY(PCI_IRDY), 
                                                 .PCI_PAR(PCI_PAR), 
                                                 .PCI_PERR(PCI_PERR), 
                                                 .PCI_STOP(PCI_STOP), 
                                                 .PCI_TRDY(PCI_TRDY));
   DMA_FPGA_icon_wrapper  u_icon (.CONTROL0(CONTROL0[35:0]), 
                                 .CONTROL1(CONTROL1[35:0]), 
                                 .CONTROL2(CONTROL2[35:0]), 
                                 .CONTROL3(CONTROL3[35:0]), 
                                 .CONTROL4(CONTROL4[35:0]), 
                                 .CONTROL5(CONTROL5[35:0]));
   bus_slave_debug  u_slave_debug_A (.abort(bus_abort_A), 
                                    .ack(slave_ack_A), 
                                    .dat_i(slave_dat_i_A[31:0]), 
                                    .dat_o(slave_dat_o_A[31:0]), 
                                    .m_rdy(bus_m_rdy_A), 
                                    .req_r_1(slave_req_r_1_A), 
                                    .req_r_2(slave_req_r_2_A), 
                                    .req_w_1(slave_req_w_A1), 
                                    .req_w_2(slave_req_w_A2), 
                                    .stb(slave_stb_A), 
                                    .s_rdy(bus_s_rdy_A), 
                                    .wb_clk_2x(WB_CLK_2x), 
                                    .we(slave_we_A), 
                                    .control(CONTROL3[35:0]));
   bus_slave_debug  u_slave_debug_B (.abort(bus_abort_B), 
                                    .ack(slave_ack_B), 
                                    .dat_i(slave_dat_i_B[31:0]), 
                                    .dat_o(slave_dat_o_B[31:0]), 
                                    .m_rdy(bus_m_rdy_B), 
                                    .req_r_1(slave_req_r_1_B), 
                                    .req_r_2(slave_req_r_2_B), 
                                    .req_w_1(slave_req_w_B1), 
                                    .req_w_2(slave_req_w_B2), 
                                    .stb(slave_stb_B), 
                                    .s_rdy(bus_s_rdy_B), 
                                    .wb_clk_2x(WB_CLK_2x), 
                                    .we(slave_we_B), 
                                    .control(CONTROL2[35:0]));
   (* IOSTANDARD = "DEFAULT" *) (* SLEW = "SLOW" *) (* DRIVE = "12" *) 
   OBUF  XLXI_1 (.I(rst_channel_0), 
                .O(rst_channel_0_pad));
   (* IOSTANDARD = "DEFAULT" *) (* SLEW = "SLOW" *) (* DRIVE = "12" *) 
   OBUF  XLXI_2 (.I(rst_channel_1), 
                .O(rst_channel_1_pad));
   (* IOSTANDARD = "DEFAULT" *) (* SLEW = "SLOW" *) (* DRIVE = "12" *) 
   OBUF  XLXI_3 (.I(rst_channel_2), 
                .O(rst_channel_2_pad));
   (* IOSTANDARD = "DEFAULT" *) (* SLEW = "SLOW" *) (* DRIVE = "12" *) 
   OBUF  XLXI_4 (.I(rst_channel_3), 
                .O(rst_channel_3_pad));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_7 (.I(channel_0_up_pad), 
                .O(channel_0_up));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_8 (.I(channel_1_up_pad), 
                .O(channel_1_up));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_9 (.I(channel_2_up_pad), 
                .O(channel_2_up));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_10 (.I(channel_3_up_pad), 
                 .O(channel_3_up));
   DMA_LED_driver  XLXI_11 (.clk(WB_CLK), 
                           .dma_in_use(dma_in_use), 
                           .rdy(XLXN_7), 
                           .green(LED_GREENX), 
                           .red(XLXN_11));
   INV  XLXI_12 (.I(WB_RST_DELAY), 
                .O(XLXN_7));
   (* IOSTANDARD = "DEFAULT" *) (* SLEW = "SLOW" *) (* DRIVE = "12" *) 
   OBUF  XLXI_13 (.I(LED_GREENX), 
                 .O(LED_GREEN));
   (* IOSTANDARD = "DEFAULT" *) (* SLEW = "SLOW" *) (* DRIVE = "12" *) 
   OBUF  XLXI_14 (.I(XLXN_11), 
                 .O(LED_RED));
   (* IOSTANDARD = "DEFAULT" *) (* SLEW = "SLOW" *) (* DRIVE = "12" *) 
   OBUF  XLXI_16 (.I(XLXN_14), 
                 .O(pulse_A));
   (* IOSTANDARD = "DEFAULT" *) (* SLEW = "SLOW" *) (* DRIVE = "12" *) 
   OBUF  XLXI_17 (.I(XLXN_15), 
                 .O(pulse_B));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_18 (.I(BOARD_IDENT[5]), 
                 .O(BOARD_IDENT_IN[5]));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_19 (.I(BOARD_IDENT[4]), 
                 .O(BOARD_IDENT_IN[4]));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_20 (.I(BOARD_IDENT[3]), 
                 .O(BOARD_IDENT_IN[3]));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_21 (.I(BOARD_IDENT[2]), 
                 .O(BOARD_IDENT_IN[2]));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_22 (.I(BOARD_IDENT[1]), 
                 .O(BOARD_IDENT_IN[1]));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_23 (.I(BOARD_IDENT[0]), 
                 .O(BOARD_IDENT_IN[0]));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_24 (.I(bc0), 
                 .O(bc0_in));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_25 (.I(bc1), 
                 .O(bc1_in));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_26 (.I(bc2), 
                 .O(bc2_in));
   (* IOSTANDARD = "DEFAULT" *) (* IBUF_DELAY_VALUE = "0" *) (* 
         IFD_DELAY_VALUE = "AUTO" *) 
   IBUF  XLXI_27 (.I(bc3), 
                 .O(bc3_in));
endmodule
