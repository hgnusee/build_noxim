/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the global traffic table
 */

#include "GlobalTrafficTable.h"

GlobalTrafficTable::GlobalTrafficTable()
{
}

bool GlobalTrafficTable::load(const char *fname)

// ###### Wed Feb 5 24:46:24 MYT 2025
// HG: Load PE Traffic Table from file.
// file format: 
// % layer# src dst out_h out_w out_c op_type (Conv|Pool|FC)

bool GlobalTrafficTable::loadTransaction(const char *fname)
{
  // Open file

  Transaction trans;

  ifstream fin(fname, ios::in);
  if (!fin)
    return false;

  // Initialize variables
  traffic_table.clear();

  // Cycle reading file
  while (!fin.eof()) {
    char line[512];
    fin.getline(line, sizeof(line) - 1);

    if (line[0] != '\0') {
      if (line[0] != '%') {
		int layer, src, dst , out_h, out_w, out_c, op_type;	// Mandatory
		int params =
		sscanf(line, "%d %d %d %d %d %d %d", &layer, &src, &dst, &out_h, &out_w, &out_c, &op_type);
		// assert ensure all 7 elements are present]
		// TODO: Instead of taking 7 elements, create more caterogies e.g. NN struct, PE src-dst, DRAM Memory, ...
		assert(params == 7);

		trans.layer = layer;
		trans.src = src;
		trans.dst = dst;

		trans.out_h = out_h;
		trans.out_w = out_w;
		trans.out_c = out_c;

		trans.op_type = op_type;
		
		trans.consumed_flag = 0;
		// save the trasn in the vector of transaction_table
		transaction_table.push_back(trans);

	
      }
    }
	cout << "File read is complete...proceed to parse traffic!" << std::endl;
  }

  return true;
}

// ###### Wed Feb 5 02:02:28 MYT 2025
// HG: Parse PE Traffic Table into Transactions
// [Phase #1] Use only the first 7 elements as input, expand in future to more categories
	// - Instead of providing real-data, give output feature size for data generation
int getTransactionInfo(const int src_type, const int src_id, int dst_type, 
					int &dst_id, int &op, int &actt,
					ControlInfo &ctrl, vector<int> &ifm, vector<int> &w) {
	
	for (unsigned int i = 0; i < transaction_table.size(); i++)
	{
		// if (transaction_table[i].src == src_id && transaction_table[i].src_type == src_type && !transaction_table[i].consumed_flag)
		if (transaction_table[i].src == src_id && !transaction_table[i].consumed_flag) // [Phase #1] src_type is temporarily removed removed
		{
			//dst_type = transaction_table[i].dst_type;
			dst_id = transaction_table[i].dst;
			//op = transaction_table[i].operation_type;
			op = transaction_table[i].op_type;

			// provide output feature size for data generation by PE
			out_h = transaction.table[i].out_h;
			out_w = transaction.table[i].out_w;
			out_c = transaction.table[i].out_c;

			//actt = transaction_table[i].activation_type;
			//ctrl = transaction_table[i].ctrl_info;
			
			//ifm = transaction_table[i].ifmap;
			//w = transaction_table[i].weight;

			transaction_table[i].consumed_flag = 1;
			return 1;
		}
		else
			continue;
	}

	ifm.clear();
	w.clear();
	return -1;

					
}
//###### Wed Feb 5 20:06:41 SGT 2025
// HG: Parse PE Traffic-specific for  PE=->Memory Tiles
int GlobalTrafficTable::getPETransactionInfo(const int src_type, const int src_id, 
	int dst_type, int &dst_id, int &op, int &actt,
	ControlInfo &ctrl, vector<int> &ifm, vector<int> &w, vector<int> &ofm) {

	for (unsigned int i = 0; i < pe_transaction_table[src_id].size(); i++)
	{
		// if (pe_transaction_table[src_id][i].src == src_id && pe_transaction_table[src_id][i].src_type == src_type && !pe_transaction_table[src_id][i].consumed_flag)
		if (pe_transaction_table[src_id][i].src == src_id && !pe_transaction_table[src_id][i].consumed_flag) // [Phase #1] src_type is temporarily removed removed
		{
			dst_type = 1; // Memory Tile is '1' in DNN-Noxim
			dst_id = pe_transaction_table[src_id][i].dst;

			// op = pe_transaction_table[src_id][i].operation_type;
			op = pe_transaction_table[src_id][i].op_type;

			// provide output feature size for data generation by PE
			out_h = pe_transaction_table[src_id].out_h;
			out_w = pe_transaction_table[src_id].out_w;
			out_c = pe_transaction_table[src_id].out_c;
/* 			
			actt = pe_transaction_table[src_id][i].activation_type;
			ctrl = pe_transaction_table[src_id][i].ctrl_info;
			ifm = pe_transaction_table[src_id][i].ifmap;
			w = pe_transaction_table[src_id][i].weight;
			ofm = pe_transaction_table[src_id][i].ofmap;
 */
			pe_transaction_table[src_id][i].consumed_flag = 1;
			return 1;
		}
		else
			continue;
	}

	ofm.clear();
	return -1;
}

// ###### Wed Feb 5 20:24:37 SGT 2025
// HG: Parse PE Traffic Table for  PE=->Memory Tiles
void loadPETransaction(const int src_id, Transaction &pe_trans) {

	cout << "loadPETransaction" << endl;
	cout << "dst_type: " << pe_trans.dst_type << "dst: " << pe_trans.dst << endl;
	cout << "src_type: " << pe_trans.src_type << "src: " << pe_trans.src << endl;

	for (long unsigned int i = 0; i < pe_trans.ofmap.size(); i++)
		cout << "OM: " << pe_trans.ofmap[i] << " ";

	cout << endl;

	// wb_trans.consumed_flag = 0;



}

// ###### Wed Feb 5 21:06:53 SGT 2025
// HG: Initialize PE Traffic Table
void GlobalTrafficTable::initPETransaction()
{
	vector<Transaction> empty_vector;
	pe_transaction_table.push_back(empty_vector);
}

double GlobalTrafficTable::getCumulativePirPor(const int src_id, const int ccycle, const bool pir_not_por, vector < pair < int, double > > &dst_prob)
{
  double cpirnpor = 0.0;

  dst_prob.clear();

  for (unsigned int i = 0; i < traffic_table.size(); i++) {
    Communication comm = traffic_table[i];
    if (comm.src == src_id) {
      int r_ccycle = ccycle % comm.t_period;
      if (r_ccycle > comm.t_on && r_ccycle < comm.t_off) {
	cpirnpor += pir_not_por ? comm.pir : comm.por;
	pair < int, double >dp(comm.dst, cpirnpor);
	dst_prob.push_back(dp);
      }
    }
  }

  return cpirnpor;
}

int GlobalTrafficTable::occurrencesAsSource(const int src_id)
{
  int count = 0;

  for (unsigned int i = 0; i < traffic_table.size(); i++)
    if (traffic_table[i].src == src_id)
      count++;

  return count;
}
