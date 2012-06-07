//////////////////////////////////////////////////////////////////////
////                                                              ////
////  File name "pci_parity_check.v"                              ////
////                                                              ////
////  This file is part of the "PCI bridge" project               ////
////  http://www.opencores.org/cores/pci/                         ////
////                                                              ////
////  Author(s):                                                  ////
////      - Miha Dolenc (mihad@opencores.org)                     ////
////                                                              ////
////  All additional information is avaliable in the README       ////
////  file.                                                       ////
////                                                              ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
////                                                              ////
//// Copyright (C) 2001 Miha Dolenc, mihad@opencores.org          ////
////                                                              ////
//// This source file may be used and distributed without         ////
//// restriction provided that this copyright statement is not    ////
//// removed from the file and that any derivative work contains  ////
//// the original copyright notice and the associated disclaimer. ////
////                                                              ////
//// This source file is free software; you can redistribute it   ////
//// and/or modify it under the terms of the GNU Lesser General   ////
//// Public License as published by the Free Software Foundation; ////
//// either version 2.1 of the License, or (at your option) any   ////
//// later version.                                               ////
////                                                              ////
//// This source is distributed in the hope that it will be       ////
//// useful, but WITHOUT ANY WARRANTY; without even the implied   ////
//// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      ////
//// PURPOSE.  See the GNU Lesser General Public License for more ////
//// details.                                                     ////
////                                                              ////
//// You should have received a copy of the GNU Lesser General    ////
//// Public License along with this source; if not, download it   ////
//// from http://www.opencores.org/lgpl.shtml                     ////
////                                                              ////
//////////////////////////////////////////////////////////////////////
//
// CVS Revision History
//
// $Log: not supported by cvs2svn $
// Revision 1.5  2003/01/27 16:49:31  mihad
// Changed module and file names. Updated scripts accordingly. FIFO synchronizations changed.
//
// Revision 1.4  2002/08/13 11:03:53  mihad
// Added a few testcases. Repaired wrong reset value for PCI_AM5 register. Repaired Parity Error Detected bit setting. Changed PCI_AM0 to always enabled(regardles of PCI_AM0 define), if image 0 is used as configuration image
//
// Revision 1.3  2002/02/01 15:25:12  mihad
// Repaired a few bugs, updated specification, added test bench files and design document
//
// Revision 1.2  2001/10/05 08:14:30  mihad
// Updated all files with inclusion of timescale file for simulation purposes.
//
// Revision 1.1.1.1  2001/10/02 15:33:47  mihad
// New project directory structure
//
//

// synopsys translate_off
`include "timescale.v"
// synopsys translate_on
`include "pci_constants.v"
`include "bus_commands.v"

module pci_parity_check
(
    reset_in,
    clk_in,
    pci_par_in,
    pci_par_out,
    pci_par_en_out,
    pci_perr_in,
    pci_perr_out,
    pci_perr_out_in,
    pci_perr_en_out,
    pci_serr_en_in,
    pci_serr_out,
    pci_serr_out_in,
    pci_serr_en_out,
    pci_frame_reg_in,
    pci_frame_en_in,
    pci_irdy_en_in,
    pci_irdy_reg_in,
    pci_trdy_reg_in,
    pci_trdy_en_in,
    pci_par_en_in,
    pci_ad_out_in,
    pci_ad_reg_in,
    pci_cbe_in_in,
    pci_cbe_reg_in,
    pci_cbe_out_in,
    pci_cbe_en_in,
    pci_ad_en_in,
    par_err_response_in,
    par_err_detect_out,
    perr_mas_detect_out,

    serr_enable_in,
    sig_serr_out

);

// system inputs
input       reset_in ;
input       clk_in ;

// pci signals that are monitored or generated by parity error checker
input           pci_par_in ;            // pci PAR input
output          pci_par_out ;           // pci_PAR output
output          pci_par_en_out ;        // pci PAR enable output
input           pci_perr_in ;           // PERR# input
output          pci_perr_out ;          // PERR# output
output          pci_perr_en_out ;       // PERR# buffer enable output
input           pci_serr_en_in ;        // SERR enable input
output          pci_serr_out ;          // SERR# output
input           pci_serr_out_in ;       // SERR# output value input
input           pci_perr_out_in ;       // PERR# output value input
output          pci_serr_en_out ;       // SERR# buffer enable output
input           pci_frame_reg_in ;       // frame from pci bus input
input           pci_frame_en_in ;       // frame enable driven by master state machine
input           pci_irdy_en_in ;        // irdy enable input from PCI master
input           pci_irdy_reg_in ;        // irdy from PCI bus
input           pci_trdy_reg_in ;        // target ready from PCI bus
input           pci_trdy_en_in ;        // target ready output enable
input           pci_par_en_in ;         // par enable input
input [31:0]    pci_ad_out_in ;         // data driven by bridge to PCI
input [31:0]    pci_ad_reg_in ;          // data driven by other agents on PCI
input [3:0]     pci_cbe_in_in ;         // cbe driven by outside agents
input [3:0]     pci_cbe_reg_in ;        // registered cbe driven by outside agents
input [3:0]     pci_cbe_out_in ;        // cbe driven by pci master state machine
input           pci_ad_en_in ;          // ad enable input
input           par_err_response_in ;   // parity error response bit from conf.space
output          par_err_detect_out ;    // parity error detected signal out
output          perr_mas_detect_out ;   // master asserted PERR or sampled PERR asserted
input           serr_enable_in ;        // system error enable bit from conf.space
output          sig_serr_out ;          // signalled system error output for configuration space
input           pci_cbe_en_in ;

// FFs for frame input - used for determining whether PAR is sampled for address phase or for data phase
reg     frame_dec2 ;
reg check_perr ;

/*=======================================================================================================================
CBE lines' parity is needed for overall parity calculation
=======================================================================================================================*/
wire par_cbe_out = pci_cbe_out_in[3] ^ pci_cbe_out_in[2] ^ pci_cbe_out_in[1] ^ pci_cbe_out_in[0] ;
wire par_cbe_in  = pci_cbe_reg_in[3] ^ pci_cbe_reg_in[2] ^ pci_cbe_reg_in[1] ^ pci_cbe_reg_in[0] ;

/*=======================================================================================================================
Parity generator - parity is generated and assigned to output on every clock edge. PAR output enable is active
one clock cycle after data output enable. Depending on whether master is performing access or target is responding,
apropriate cbe data is included in parity generation. Non - registered CBE is used during reads through target SM
=======================================================================================================================*/

// generate appropriate par signal
wire data_par = (pci_ad_out_in[31] ^ pci_ad_out_in[30] ^ pci_ad_out_in[29] ^ pci_ad_out_in[28]) ^
                (pci_ad_out_in[27] ^ pci_ad_out_in[26] ^ pci_ad_out_in[25] ^ pci_ad_out_in[24]) ^
                (pci_ad_out_in[23] ^ pci_ad_out_in[22] ^ pci_ad_out_in[21] ^ pci_ad_out_in[20]) ^
                (pci_ad_out_in[19] ^ pci_ad_out_in[18] ^ pci_ad_out_in[17] ^ pci_ad_out_in[16]) ^
                (pci_ad_out_in[15] ^ pci_ad_out_in[14] ^ pci_ad_out_in[13] ^ pci_ad_out_in[12]) ^
                (pci_ad_out_in[11] ^ pci_ad_out_in[10] ^ pci_ad_out_in[9]  ^ pci_ad_out_in[8])  ^
                (pci_ad_out_in[7]  ^ pci_ad_out_in[6]  ^ pci_ad_out_in[5]  ^ pci_ad_out_in[4])  ^
                (pci_ad_out_in[3]  ^ pci_ad_out_in[2]  ^ pci_ad_out_in[1]  ^ pci_ad_out_in[0]) ;

wire par_out_only = data_par ^ par_cbe_out ;

pci_par_crit par_gen
(
    .par_out        (pci_par_out),
    .par_out_in     (par_out_only),
    .pci_cbe_en_in  (pci_cbe_en_in),
    .data_par_in    (data_par),
    .pci_cbe_in     (pci_cbe_in_in)
) ;

// PAR enable = ad output enable delayed by one clock
assign pci_par_en_out = pci_ad_en_in ;

/*=======================================================================================================================
Parity checker - parity is checked on every clock cycle. When parity error is detected, appropriate action is taken
to signal address parity errors on SERR if enabled and data parity errors on PERR# if enabled. Logic also drives
outputs to configuration space to set appropriate status bits if parity error is detected. PAR signal is checked on
master read operations or writes through pci target. Master read is performed when master drives irdy output and
doesn't drive ad lines. Writes through target are performed when target is driving trdy and doesn't drive ad lines.
=======================================================================================================================*/

// equation indicating whether to check and generate or not PERR# signal on next cycle
wire perr_generate =  ~pci_par_en_in && ~pci_ad_en_in                   // par was not generated on this cycle, so it should be checked
                      && ((pci_irdy_en_in && ~pci_trdy_reg_in) ||       // and master is driving irdy and target is signaling ready
                          (pci_trdy_en_in && ~pci_irdy_reg_in)) ;       // or target is driving trdy and master is signaling ready

wire data_in_par = (pci_ad_reg_in[31] ^ pci_ad_reg_in[30] ^ pci_ad_reg_in[29] ^ pci_ad_reg_in[28]) ^
                   (pci_ad_reg_in[27] ^ pci_ad_reg_in[26] ^ pci_ad_reg_in[25] ^ pci_ad_reg_in[24]) ^
                   (pci_ad_reg_in[23] ^ pci_ad_reg_in[22] ^ pci_ad_reg_in[21] ^ pci_ad_reg_in[20]) ^
                   (pci_ad_reg_in[19] ^ pci_ad_reg_in[18] ^ pci_ad_reg_in[17] ^ pci_ad_reg_in[16]) ^
                   (pci_ad_reg_in[15] ^ pci_ad_reg_in[14] ^ pci_ad_reg_in[13] ^ pci_ad_reg_in[12]) ^
                   (pci_ad_reg_in[11] ^ pci_ad_reg_in[10] ^ pci_ad_reg_in[9]  ^ pci_ad_reg_in[8])  ^
                   (pci_ad_reg_in[7]  ^ pci_ad_reg_in[6]  ^ pci_ad_reg_in[5]  ^ pci_ad_reg_in[4])  ^
                   (pci_ad_reg_in[3]  ^ pci_ad_reg_in[2]  ^ pci_ad_reg_in[1]  ^ pci_ad_reg_in[0]) ;

//wire perr = (cbe_par_reg ^ pci_par_in ^ data_in_par) ;
wire perr ;
wire perr_n ;
wire perr_en ;

assign pci_perr_out = perr_n ;

// parity error output assignment
//assign pci_perr_out = ~(perr && perr_generate) ;

wire non_critical_par = par_cbe_in ^ data_in_par ;

pci_perr_crit perr_crit_gen
(
    .perr_out           (perr),
    .perr_n_out         (perr_n),
    .non_critical_par_in(non_critical_par),
    .pci_par_in         (pci_par_in),
    .perr_generate_in   (perr_generate)
) ;

// PERR# enable
wire pci_perr_en_reg ;
pci_perr_en_crit perr_en_crit_gen
(
    .reset_in               (reset_in),
    .clk_in                 (clk_in),
    .perr_en_out            (pci_perr_en_out),
    .perr_en_reg_out        (pci_perr_en_reg),
    .non_critical_par_in    (non_critical_par),
    .pci_par_in             (pci_par_in),
    .perr_generate_in       (perr_generate),
    .par_err_response_in    (par_err_response_in)
) ;

// address phase decoding
always@(posedge reset_in or posedge clk_in)
begin
    if (reset_in)
        frame_dec2 <= #`FF_DELAY 1'b0 ;
    else
        frame_dec2 <= #`FF_DELAY pci_frame_reg_in ;
end

// address phase parity error checking - done after address phase is detected - which is - when bridge's master is not driving frame,
// frame was asserted on previous cycle and was not asserted two cycles before.
wire check_for_serr_on_first = ~pci_frame_reg_in && frame_dec2  && ~pci_frame_en_in ;

reg  check_for_serr_on_second ;
always@(posedge reset_in or posedge clk_in)
begin
    if ( reset_in )
        check_for_serr_on_second <= #`FF_DELAY 1'b0 ;
    else
        check_for_serr_on_second <= #`FF_DELAY check_for_serr_on_first && ( pci_cbe_reg_in == `BC_DUAL_ADDR_CYC ) ;
end

wire check_for_serr = check_for_serr_on_first || check_for_serr_on_second ;

wire serr_generate = check_for_serr && serr_enable_in && par_err_response_in ;

pci_serr_en_crit serr_en_crit_gen
(
    .serr_en_out        (pci_serr_en_out),
    .pci_par_in         (pci_par_in),
    .non_critical_par_in(non_critical_par),
    .serr_generate_in   (serr_generate)
);


// serr is enabled only for reporting errors - route this signal to configuration space
assign sig_serr_out = pci_serr_en_in ;

// SERR# output is always 0, just enable is driven apropriately
pci_serr_crit serr_crit_gen
(
    .serr_out               (pci_serr_out),
    .non_critical_par_in    (non_critical_par),
    .pci_par_in             (pci_par_in),
    .serr_check_in          (check_for_serr)
);

/*=======================================================================================================================================
    Synchronizing mechanism detecting what is supposed to be done - PERR# generation or PERR# checking
=======================================================================================================================================*/
// perr should be checked one clock after PAR is generated
always@(posedge reset_in or posedge clk_in)
begin
    if ( reset_in )
        check_perr <= #`FF_DELAY 1'b0 ;
    else
        check_perr <= #`FF_DELAY pci_par_en_in ;
end

wire perr_sampled_in = ~pci_perr_in && check_perr ;
reg perr_sampled ;
always@(posedge reset_in or posedge clk_in)
begin
    if (reset_in)
        perr_sampled <= #`FF_DELAY 1'b0 ;
    else
        perr_sampled <= #`FF_DELAY perr_sampled_in ;
end

// assign output for parity error detected bit
assign par_err_detect_out = ~pci_serr_out_in || ~pci_perr_out_in ;//|| perr_sampled ; MihaD - removed - detected parity error is set only during Master Reads or Target Writes

// FF indicating that that last operation was done as bus master
reg frame_and_irdy_en_prev      ;
reg frame_and_irdy_en_prev_prev ;
reg master_perr_report ;
always@(posedge reset_in or posedge clk_in)
begin
    if ( reset_in )
    begin
        master_perr_report          <= #`FF_DELAY 1'b0 ;
        frame_and_irdy_en_prev      <= #`FF_DELAY 1'b0 ;
        frame_and_irdy_en_prev_prev <= #`FF_DELAY 1'b0 ;
    end
    else
    begin
        master_perr_report          <= #`FF_DELAY frame_and_irdy_en_prev_prev ;
        frame_and_irdy_en_prev      <= #`FF_DELAY pci_irdy_en_in && pci_frame_en_in ;
        frame_and_irdy_en_prev_prev <= #`FF_DELAY frame_and_irdy_en_prev ;
    end
end

assign perr_mas_detect_out = master_perr_report && ( (par_err_response_in && perr_sampled) || pci_perr_en_reg ) ;

endmodule
