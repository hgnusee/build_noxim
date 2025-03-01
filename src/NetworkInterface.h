/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the processing element
 */

// ###### Sat Feb 8 14:09:47 SGT 2025
// HG: Imported for DNN-Noxim support


#ifndef __NOXIMNETWORKINTERFACE_H__
#define __NOXIMNETWORKINTERFACE_H__

#include <queue>
#include <systemc.h>

#include "DataStructs.h"
#include "Buffer.h"
#include "GlobalTrafficTable.h"
#include "Utils.h"

using namespace std;

SC_MODULE(NetworkInterface)
{

    // I/O Ports
    sc_in_clk clock;   // The input clock for the PE
    sc_in<bool> reset; // The reset signal for the PE

    sc_in<Flit> flit_rx; // The input channel
    sc_in<bool> req_rx;  // The request associated with the input channel
    sc_out<bool> ack_rx; // The outgoing ack signal associated with the input channel
    sc_out<TBufferFullStatus> buffer_full_status_rx;

    sc_out<Flit> flit_tx; // The output channel
    sc_out<bool> req_tx;  // The request associated with the output channel
    sc_in<bool> ack_tx;   // The outgoing ack signal associated with the output channel
    sc_in<TBufferFullStatus> buffer_full_status_tx;

    sc_in<Flit> flit_rx_pe; // The input channel
    sc_in<bool> req_rx_pe;  // The request associated with the input channel
    sc_out<bool> ack_rx_pe; // The outgoing ack signal associated with the input channel
    sc_out<TBufferFullStatus> buffer_full_status_rx_pe;

    sc_out<Flit> flit_tx_pe; // The output channel
    sc_out<bool> req_tx_pe;  // The request associated with the output channel
    sc_in<bool> ack_tx_pe;   // The outgoing ack signal associated with the output channel
    sc_in<TBufferFullStatus> buffer_full_status_tx_pe;

    sc_in<int> free_slots_neighbor;

    // Registers
    int local_id;                    // Unique identification number
    bool current_level_rx;           // Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx;           // Current level for Alternating Bit Protocol (ABP)
    bool current_level_rx_pe;        // Current level for Alternating Bit Protocol (ABP)
    bool current_level_tx_pe;        // Current level for Alternating Bit Protocol (ABP)
    PeBufferBank pebuffer;           // buffer[direction][virtual_channel]
    queue<Packet> packet_queue;      // Local queue of packets
    bool transmittedAtPreviousCycle; // Used for distributions with memory

    // Functions
    void rxProcess();              // The receiving process from local port of Router
    void txProcess();              // The transmitting process from local port of Router
    void rxPeProcess();            // The receiving process from PE
    void txPeProcess();            // The transmitting process from PE
    bool canShot(Packet & packet); // True when the packet must be shot
    Flit nextFlit();               // Take the next flit of the current packet
    // Packet trafficTest();          // used for testing traffic
    // Packet trafficRandom();        // Random destination distribution
    // Packet trafficTranspose1();    // Transpose 1 destination distribution
    // Packet trafficTranspose2();    // Transpose 2 destination distribution
    // Packet trafficBitReversal();   // Bit-reversal destination distribution
    // Packet trafficShuffle();       // Shuffle destination distribution
    // Packet trafficButterfly();     // Butterfly destination distribution
    // Packet trafficULocal();        // Random with locality

    GlobalTrafficTable *traffic_table;     // Reference to the Global traffic Table
    GlobalTrafficTable *transaction_table; //* Reference to the Global transaction Table
    bool never_transmit;                   // true if the PE does not transmit any packet
    //  (valid only for the table based traffic)

    // void fixRanges(const Coord, Coord &); // Fix the ranges of the destination
    int randInt(int min, int max); // Extracts a random integer number between min and max
    int getRandomSize();           // Returns a random size in flits for the packet
    // void setBit(int &x, int w, int v);
    // int getBit(int x, int w);
    // double log2ceil(double x);

    // int roulett();
    // int findRandomDestination(int local_id, int hops);
    unsigned int getQueueSize() const;

    //* Load below informations from transaction table
    int transaction_dst_type;
    int transaction_dst;
    int transaction_opt;
    int transaction_act;
    ControlInfo transaction_ctrl;
    vector<int> ifmap_data;
    vector<int> weight_data;
    vector<int> ofmap_data;
    int ifm_data_cnt, remain_ifm_size;
    int w_data_cnt, remain_w_size;
    int ofm_data_cnt, remain_ofm_size;
    //* Make packet in different state: INSTRUCTION, INPUT_DATA, WEIGHT_DATA
    int makeP_state;
    //* Make flit in different state (same above)
    int makeF_state;
    //* Keep transaction and produce the packet series
    int remaining_traffic;
    //*
    unsigned int last_received_flit_time = 0;

    // Constructor
    SC_CTOR(NetworkInterface)
    {
        SC_METHOD(rxProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(txProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(rxPeProcess);
        sensitive << reset;
        sensitive << clock.pos();

        SC_METHOD(txPeProcess);
        sensitive << reset;
        sensitive << clock.pos();
    }
};

#endif
