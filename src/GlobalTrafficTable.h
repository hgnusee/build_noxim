/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the definition of the global traffic table
 */

#ifndef __NOXIMGLOBALTRAFFIC_TABLE_H__
#define __NOXIMGLOBALTRAFFIC_TABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "DataStructs.h"

using namespace std;

// Structure used to store information into the table
struct Communication {
  int src;			// ID of the source node (PE)
  int dst;			// ID of the destination node (PE)
  double pir;			// Packet Injection Rate for the link
  double por;			// Probability Of Retransmission for the link
  int t_on;			// Time (in cycles) at which activity begins
  int t_off;			// Time (in cycles) at which activity ends
  int t_period;		        // Period after which activity starts again
};

// ###### Wed Feb 5 01:00:05 MYT 2025
// struct for PE Traffic Table (consider all 7 elements )
struct Transaction
{
  int layer;
	
  int src;
	int dst;
  
  int out_h, out_w, out_c;
  
  int op_type;

	int src_type;
	int dst_type;
  
	int operation_type;
	// int data_type;
	int activation_type;

	ControlInfo ctrl_info; // TODO: create ControlInfo under DataStructs.h [DNN-Noxim]

  // TODO: create vector for ifmap, weight, ofmap under DataStructs.h [DNN-Noxim]
	vector<int> ifmap;
	vector<int> weight;
	vector<int> ofmap;

	int consumed_flag;
};

class GlobalTrafficTable {

  public:

    GlobalTrafficTable();

    // Load traffic table from file. Returns true if ok, false otherwise
    bool load(const char *fname);

    // ###### Wed Feb 5 24:46:11 MYT 2025
    // HG: Load PE Traffic Table from file.
    bool loadTransaction(const char *fname);
    // ###### Wed Feb 5 02:02:28 MYT 2025
    // HG: Parse PE Traffic Table into Transactions
    int getTransactionInfo(const int src_type, const int src_id, int dst_type, 
                            int &dst_id, int &op, int &actt,
						                ControlInfo &ctrl, vector<int> &ifm, vector<int> &w);
    //###### Wed Feb 5 20:06:41 SGT 2025
    // HG: Get PE Traffic-specific for  PE=->Memory Tiles
    int getPETransactionInfo(const int src_type, const int src_id, int dst_type, int &dst_id, int &op, int &actt,
							 ControlInfo &ctrl, vector<int> &ifm, vector<int> &w, vector<int> &ofm);
    // ###### Wed Feb 5 20:24:37 SGT 2025
    // HG: Parse PE Traffic Table for PE=->Memory Tiles
    void loadPETransaction(const int src_id, Transaction &pe_trans);
    // ###### Wed Feb 5 21:06:53 SGT 2025
    // HG: Initialize PE Traffic Table
    void initPETransaction();
    



    // Returns the cumulative pir por along with a vector of pairs. The
    // first component of the pair is the destination. The second
    // component is the cumulative shotting probability.
    double getCumulativePirPor(const int src_id,
			       const int ccycle,
			       const bool pir_not_por,
			       vector < pair < int, double > > &dst_prob);

    // Returns the number of occurrences of soruce src_id in the traffic
    // table
    int occurrencesAsSource(const int src_id);

  private:

     vector < Communication > traffic_table;
     // Transaction Table to store PE Traffic Table from txt file
     vector <Transaction> transaction_table;
     // Transaction Table to store PE=->Memory Traffic Table from txt file
     vector < vector<Transaction> > pe_transaction_table;
};

#endif
