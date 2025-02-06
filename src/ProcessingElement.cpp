/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the processing element
 */

#include "ProcessingElement.h"
// ###### Wed Feb 5 22:09:29 SGT 2025
// HG: encapsulate rxProcess and txProcess (so that they happen simultaneously)
void ProcessingElement::process()
{
    txProcess();
    // computeProcess();
    rxProcess();
}

void ProcessingElement::rxProcess()
{
    if (reset.read())
    {
        ack_rx_pe.write(0);
        current_level_rx_pe = 0;

        OP = -1;  // HG: -1 ==> NO Operation
        O_H = 0;
        O_W = 0;
        O_C = 0;
        layer_no = 0; // TODO: Is this necessary?
        output_buffer_addr = 0;
        tranmitted_flit = 0;
        state = LOAD;
        compute_state = 0;
        cycle_cnt = 0;
    } else 
    {
        if (state == LOAD)
        {
            if (req_rx_pe.read() == 1 - current_level_rx_pe)
            {
                Flit flit_tmp = flit_rx_pe.read();

                // HG: Based on the received flits, decide what to do with them
                // TODO: Decide how to process the received flits...
                if (flit_tmp.data_type == INSTRUCTION && flit_tmp.flit_type != FLIT_TYPE_HEAD)
                {
                    // Initialize the PE when first receiving 'instructions'
                    OP = flit_tmp.op_type;
                    layer_no = flit_tmp.layer_no;

                    output_buffer_addr = 0;
                    tranmitted_flit = 0;
                    cout << "Instruction" << endl;

                    // [Phase #1] Compute the OFM size based on IFM in the future
                }
                else if (flit_tmp.data_type == INPUT_DATA && flit_tmp.flit_type != FLIT_TYPE_HEAD)
                {
                    // Increment no of flits received and store it 
                    input_buffer[input_buffer_addr] = flit_tmp.payload.data;
                    input_buffer_addr++;
                    cout << "INPUT_DATA" << endl;
                }

                // TODO: Read the OP type, and update state to BUSY [DNN-Noxim]

                // std::cout << " PE: => "
                //           << "Flit: " << flit_tmp.payload.data
                //           << " Local id : " << local_id << endl;
                // TODO: Update ABP current level at LOAD state only
                current_level_rx_pe = 1 - current_level_rx_pe; // Negate the old value for Alternating Bit Protocol (ABP)
                ack_rx_pe.write(current_level_rx_pe);
            }
        }
        else if (state == BUSY)
        {
            // computeProcess();
        }
        else if (state == WB)
        {
            // writeBack();
            state = LOAD;

            // I_H = 0;
            // I_W = 0;
            // K_H = 0;
            // K_W = 0;
            // S = 0;
            OP = -1;
            O_H = 0;
            O_W = 0;
            // WB_DST = -1;
            input_buffer_addr = 0;
            weight_buffer_addr = 0;
            psum_buffer_addr = 0;
            output_buffer_addr = 0;
        }
    }


    // Original RxProcess Code, used as concept to adapt NN Traffic
/* 
    if (reset.read()) {
	ack_rx.write(0);
	current_level_rx = 0;
    } else {
	if (req_rx.read() == 1 - current_level_rx) {
	    Flit flit_tmp = flit_rx.read();
	    current_level_rx = 1 - current_level_rx;	// Negate the old value for Alternating Bit Protocol (ABP)
	}
	ack_rx.write(current_level_rx);
    }
     */
}

void ProcessingElement::txProcess()
{   
        if (reset.read())
    {
        req_tx_pe.write(0);
        current_level_tx_pe = 0;
        transmittedAtPreviousCycle = false;

        i = 0;
        makeP_state = 0;
    }
    else
    {

        if (ack_tx_pe.read() == current_level_tx_pe)
        {
        }
    }

    // TODO: Figure out why DNN-Noxim PE txProcess() seems to be non-functional? Likely Router took over its role in Tx

// Original Code
    /* 
    if (reset.read()) {
	req_tx.write(0);
	current_level_tx = 0;
	transmittedAtPreviousCycle = false;
    } else {
	Packet packet;

	if (canShot(packet)) {
	    packet_queue.push(packet);
	    transmittedAtPreviousCycle = true;
	} else
	    transmittedAtPreviousCycle = false;


	if (ack_tx.read() == current_level_tx) {
	    if (!packet_queue.empty()) {
		Flit flit = nextFlit();	// Generate a new flit
		flit_tx->write(flit);	// Send the generated flit
		current_level_tx = 1 - current_level_tx;	// Negate the old value for Alternating Bit Protocol (ABP)
		req_tx.write(current_level_tx);
	    }
	}
    }
    */
}

// ###### Thu Feb 6 11:21:48 SGT 2025
// HG: Not used in DNN-Noxim, but somehow declared in PE header file?
/* 
Flit ProcessingElement::nextFlit()
{
    Flit flit;
    Packet packet = packet_queue.front();

    flit.src_id = packet.src_id;
    flit.dst_id = packet.dst_id;
    flit.vc_id = packet.vc_id;
    flit.timestamp = packet.timestamp;
    flit.sequence_no = packet.size - packet.flit_left;
    flit.sequence_length = packet.size;
    flit.hop_no = 0;
    //  flit.payload     = DEFAULT_PAYLOAD;

    flit.hub_relay_node = NOT_VALID;

    if (packet.size == packet.flit_left)
	flit.flit_type = FLIT_TYPE_HEAD;
    else if (packet.flit_left == 1)
	flit.flit_type = FLIT_TYPE_TAIL;
    else
	flit.flit_type = FLIT_TYPE_BODY;

    packet_queue.front().flit_left--;
    if (packet_queue.front().flit_left == 0)
	packet_queue.pop();

    return flit;
}
 */

// ###### Thu Feb 6 11:21:48 SGT 2025
// HG: Not used in DNN-Noxim, but somehow declared in PE header file?
/* bool ProcessingElement::canShot(Packet & packet)
{
   // assert(false);
    if(never_transmit) return false;
   
    //if(local_id!=16) return false;
    
    // DEADLOCK TEST 
	// double current_time = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;

	// if (current_time >= 4100) 
	// {
	//     //if (current_time==3500)
	//          //cout << name() << " IN CODA " << packet_queue.size() << endl;
	//     return false;
	// }
	
    

#ifdef DEADLOCK_AVOIDANCE
    if (local_id%2==0)
	return false;
#endif
    bool shot;
    double threshold;

    double now = sc_time_stamp().to_double() / GlobalParams::clock_period_ps;

    if (GlobalParams::traffic_distribution != TRAFFIC_TABLE_BASED) {
	if (!transmittedAtPreviousCycle)
	    threshold = GlobalParams::packet_injection_rate;
	else
	    threshold = GlobalParams::probability_of_retransmission;

	shot = (((double) rand()) / RAND_MAX < threshold);
	if (shot) {
	    if (GlobalParams::traffic_distribution == TRAFFIC_RANDOM)
		    packet = trafficRandom();
        else if (GlobalParams::traffic_distribution == TRAFFIC_TRANSPOSE1)
		    packet = trafficTranspose1();
        else if (GlobalParams::traffic_distribution == TRAFFIC_TRANSPOSE2)
    		packet = trafficTranspose2();
        else if (GlobalParams::traffic_distribution == TRAFFIC_BIT_REVERSAL)
		    packet = trafficBitReversal();
        else if (GlobalParams::traffic_distribution == TRAFFIC_SHUFFLE)
		    packet = trafficShuffle();
        else if (GlobalParams::traffic_distribution == TRAFFIC_BUTTERFLY)
		    packet = trafficButterfly();
        else if (GlobalParams::traffic_distribution == TRAFFIC_LOCAL)
		    packet = trafficLocal();
        else if (GlobalParams::traffic_distribution == TRAFFIC_ULOCAL)
		    packet = trafficULocal();
        else {
            cout << "Invalid traffic distribution: " << GlobalParams::traffic_distribution << endl;
            exit(-1);
        }
	}
    } else {			// Table based communication traffic
	if (never_transmit)
	    return false;

	bool use_pir = (transmittedAtPreviousCycle == false);
	vector < pair < int, double > > dst_prob;
	double threshold =
	    traffic_table->getCumulativePirPor(local_id, (int) now, use_pir, dst_prob);

	double prob = (double) rand() / RAND_MAX;
	shot = (prob < threshold);
	if (shot) {
	    for (unsigned int i = 0; i < dst_prob.size(); i++) {
		if (prob < dst_prob[i].second) {
                    int vc = randInt(0,GlobalParams::n_virtual_channels-1);
		    packet.make(local_id, dst_prob[i].first, vc, now, getRandomSize());
		    break;
		}
	    }
	}
    }

    return shot;
}
*/