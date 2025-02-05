/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */

#ifndef __NOXIMPROCESSINGELEMENT_H__
#define __NOXIMPROCESSINGELEMENT_H__

#include <queue>
#include <systemc.h>

#include "DataStructs.h"
#include "GlobalTrafficTable.h"
#include "Utils.h"

// ###### Wed Feb 5 21:42:59 SGT 2025
// HG: libraries to include for adding Buffer feature to ProcessingElement
#include "Buffer.h"
#include "GlobalParams.h"

using namespace std;

SC_MODULE(ProcessingElement)
{

    // I/O Ports
    sc_in_clk clock;		// The input clock for the PE
    sc_in < bool > reset;	// The reset signal for the PE

    sc_in < Flit > flit_rx;	// The input channel
    sc_in < bool > req_rx;	// The request associated with the input channel
    sc_out < bool > ack_rx;	// The outgoing ack signal associated with the input channel
    sc_out < TBufferFullStatus > buffer_full_status_rx;	

    sc_out < Flit > flit_tx;	// The output channel
    sc_out < bool > req_tx;	// The request associated with the output channel
    sc_in < bool > ack_tx;	// The outgoing ack signal associated with the output channel
    sc_in < TBufferFullStatus > buffer_full_status_tx;

    // HG: This is commented out in DNN-Noxim
    sc_in < int >free_slots_neighbor;

    // Registers
    int local_id;		// Unique identification number
    bool current_level_rx;	// Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx;	// Current level for Alternating Bit Protocol (ABP)
    queue < Packet > packet_queue;	// Local queue of packets
    bool transmittedAtPreviousCycle;	// Used for distributions with memory

// ############################## NN Variables #######################################
// ###### Wed Feb 5 21:48:26 SGT 2025
    // HG: Use the 7 usual variables
    int layer_no;
    int output_buffer_addr;
    int tranmitted_flit;

    int OP;

    int O_H;
    int O_W;
    int O_M[OUTPUT_BUFFER][OUTPUT_BUFFER];
    int O_C;

    State state; // HG: added to DataStruct

    sc_uint<32> output_buffer[OUTPUT_BUFFER * OUTPUT_BUFFER];
    int i;
    int makeP_state;
    int compute_state;
    int cycle_cnt;
    
    // [Phase #1] input, weight, psums will be added in future iteration
    /*     
        int input_buffer_addr;
        int weight_buffer_addr;
        int psum_buffer_addr;
        sc_uint<32> input_buffer[INPUT_BUFFER * INPUT_BUFFER];
        sc_uint<32> weight_buffer[WEIGHT_BUFFER * WEIGHT_BUFFER];
        sc_uint<32> psum_buffer[8 * 8];
    */

// ############################## NN Operations #######################################

    // Main Functions
    // ###### Wed Feb 5 22:09:29 SGT 2025
    void process();         // HG: encapsulate rxProcess and txProcess
    void rxProcess();		// The receiving process
    void txProcess();		// The transmitting process
    bool canShot(Packet & packet);	// True when the packet must be shot

    // HG: synthetic traffic all removed, replace with NN traffic actions

// ####################################################################################

    GlobalTrafficTable *traffic_table;	// Reference to the Global traffic Table
    Flit nextFlit();	// Take the next flit of the current packet
    
    // HG: remove PE utilies


    // Constructor
    SC_CTOR(ProcessingElement) {
    // HG: rxProcess() & txProcess() happens simultaneously
    /* 
        SC_METHOD(rxProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(txProcess);
        sensitive << reset;
        sensitive << clock.pos(); 
    */
   
    // HG: DNN-Noxim Prefers this style instead
    // functionally similar to above as proven here: https://www.edaplayground.com/x/QhXT, https://www.edaplayground.com/x/rVGm
    
    SC_METHOD(process);
    sensitive << reset;
    sensitive << clock.pos();
    
    }

};

#endif
