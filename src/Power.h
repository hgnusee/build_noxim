/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the power model
 */

#ifndef __NOXIMPOWER_H__
#define __NOXIMPOWER_H__

#include <cassert>
#include <map>
#include "DataStructs.h"

#include "yaml-cpp/yaml.h"



using namespace std;

class Power {

  public:

    Power();


    void configureRouter(int link_width,
	                 int buffer_depth,
			 int buffer_item_size,
			 string routing_function,
			 string selection_function);

    void bufferRouterPush(); 
    void bufferRouterPop(); 
    void bufferRouterFront(); 
    // HG: Commented out bufferTo/From functions ###### Sat Feb 8 11:21:29 SGT 2025
    // Hub related items removed
    // void bufferToTilePush(); 
    // void bufferToTilePop(); 
    // void bufferToTileFront(); 
    // void bufferFromTilePush(); 
    // void bufferFromTilePop(); 
    // void bufferFromTileFront(); 

    void routing();
    void selection(); 
    void crossBar(); 
    // void r2hLink(); 
    void r2rLink(); 
    void networkInterface();
    // TODO: Consider MemNetworkInterface Power Draw

    void leakageBufferRouter();
    // void leakageBufferToTile();
    // void leakageBufferFromTile();

    void leakageLinkRouter2Router();
    
    void leakageRouter();
    void leakageTransceiverRx();
    void leakageTransceiverTx();
    void biasingRx();
    void biasingTx();

    double getDynamicPower();
    double getStaticPower();

    double getTotalPower() {
	return (getDynamicPower() + getStaticPower());
    } 


    void printBreakDown(std::ostream & out);


    PowerBreakdown* getDynamicPowerBreakDown(){ return &power_dynamic;}
    PowerBreakdown* getStaticPowerBreakDown(){ return &power_static;}

    void rxSleep(int cycles);
    bool isSleeping();

  private:

    double total_power_s;

    double buffer_router_push_pwr_d;
    double buffer_router_pop_pwr_d;
    double buffer_router_front_pwr_d;
    double buffer_router_pwr_s;
    
    double buffer_to_tile_push_pwr_d;
    double buffer_to_tile_pop_pwr_d;
    double buffer_to_tile_front_pwr_d;
    double buffer_to_tile_pwr_s;

    double buffer_from_tile_push_pwr_d;
    double buffer_from_tile_pop_pwr_d;
    double buffer_from_tile_front_pwr_d;
    double buffer_from_tile_pwr_s;

    double antenna_buffer_push_pwr_d;
    double antenna_buffer_pop_pwr_d;
    double antenna_buffer_front_pwr_d;
    double antenna_buffer_pwr_s;

    double wireless_rx_pwr;
    double transceiver_tx_pwr_s;
    double transceiver_rx_pwr_s;
    double transceiver_tx_pwr_biasing;
    double transceiver_rx_pwr_biasing;
    double wireless_snooping;

    double default_tx_energy;

    double routing_pwr_d;
    double routing_pwr_s;

    double selection_pwr_d;
    double selection_pwr_s;

    double crossbar_pwr_d;
    double crossbar_pwr_s;

    double link_r2r_pwr_d;
    double link_r2r_pwr_s;
    double link_r2h_pwr_s;
    double link_r2h_pwr_d;

    double ni_pwr_d;
    double ni_pwr_s;

    map< pair<int, int> , double>  attenuation_map;
    double attenuation2power(double);


    void printBreakDown(string label, const map<string,double> & m,std::ostream & out) const;

    PowerBreakdown power_dynamic;
    PowerBreakdown power_static;

    void initPowerBreakdownEntry(PowerBreakdownEntry* pbe,string label);
    void initPowerBreakdown();





    int sleep_end_cycle;


    
};

#endif
