//============================================================================
// Name        : NetworkSimulator.cpp
// Author      : vasileios pashos
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Simulator.h"
#include <cstdlib>
#include <fstream>
#include <bitset>

int main() {

		string u=getenv("USER");
		string filename="/home/"+u+"/SVC/foreman-Temporal/QCIF/L4/SVCL4.264"; // Path including the SVC bistream file
		string traceFile="/home/"+u+"/SVC/foreman-Temporal/QCIF/L4/traceL4.txt"; //Path including trace file for the corresponding bitstream using BitStreamExtractor (JSVM)
		pathToStore="/home/"+u+"/SVC/foreman-Temporal/QCIF/L4/probSVC02/"; //Path storing the output files after simulation finishes

		int rateRatio3GtoWLAN=4;//We assume the transmission over wifi is 4 times more than transmissions over 3G
		int numberOfNodes=5;
		Simulator sim=Simulator(numberOfNodes,filename,traceFile);
		sim.createMeshTopology();

		for(int simTime=0;simTime<100000;simTime++){
			cout<<"###############################################################"<<endl;
			cout<<"#                                    TIMESLOT "<<simTime+1<<"                           							#"<<endl;
			cout<<"###############################################################"<<endl;
			if(simTime%rateRatio3GtoWLAN==0)
				sim.simulate3GTransmissions(simTime);
			sim.simulateWIFIBroadcasts(simTime);

			if(channel3G::status==0 ){
				cout<<"All downloads on 3G finished "<<endl;
				if(sim.checkNodeQueues(simTime)){
					cout<<"There is nothing to be transmitted inside the lan"<<endl;
					break;
				}
			}

			cout<<"********************Enf OF TimeSlot "<<simTime+1<<" ***********************"<<endl;
		}
		cout.flush();

		cout<<"finished"<<endl;
		cout<<"-------------------------- "<<endl;
	 	cout<<"Check nodes number of incoming packets "<<endl;
		sim.showNodesIncomingPacketList();

		return 0;
}
