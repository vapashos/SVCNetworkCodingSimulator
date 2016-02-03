/*
 * Simulator.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: sportbilly
 */

#include "Simulator.h"



Simulator::Simulator(int nodesNum,string bitStreamfileName,string traceFileName){

	int genSize=10;
	cout<<"Generation Size:"<<genSize<<endl;
	/*The number of 3G channels is equal to the number of users*/
	s=new server(genSize,bitStreamfileName,traceFileName);
	for(int i=0;i<nodesNum;i++){
		Node temp=Node();
		/*Init node list*/
		nodeList.push_back(temp);
		/*Init 3G channel List*/
		channel3GList.push_back(channel3G(s,&nodeList.back(),0.35)); //create 3G channels with loss probability = 0.35
	}
	cout<<"Simulation with "<<nodeList.size()<<" nodes created"<<endl;
	allocateSegmentsOnUsers(s);
	broadcastNode=NULL;
}

Simulator::~Simulator() {
	// TODO Auto-generated destructor stub
}

void Simulator::createMeshTopology(){
	for(unsigned int i=0;i<nodeList.size();i++){
		for(unsigned int j=0;j<nodeList.size();j++)
			nodeList[i].addNeighbor(&nodeList[j]);
	}
}


bool Simulator::setBroadcastNode(){
	//Decide between nodes with no empty outgoing queues

		deque<Node *> tempList;
		for(unsigned int i=0;i<nodeList.size();i++){
			if(!nodeList[i].isEmpty(nodeList[i].outQueue)){
				tempList.push_back(&nodeList[i]);
			}
		}
		segment *broadcastSegment;
		if(tempList.empty()){
			cout<<"There is no node that has data to transmit"<<endl;
			broadcastNode=NULL;
			return false;
		}
		if(tempList.size()==1){
			broadcastNode=tempList.front();
			broadcastSegment=Node::getSegment(broadcastNode->outQueue.back().segmentID);
			if(broadcastSegment==NULL){
				cout<<"problem broadcastSegment is NULL"<<endl;
			}
			broadcastNode->bdSegment=broadcastSegment;
			return true;
		}
		broadcastNode=tempList[randomNumber(0,tempList.size())];
		broadcastSegment=Node::getSegment(broadcastNode->outQueue.front().segmentID);
		broadcastNode->bdSegment=broadcastSegment;
		return true;
}

void Simulator::allocateSegmentsOnUsers(server *s){
	/*I will comment the next lines just for testing purposes to have less traffic on network*/
	for(unsigned int i=0;i<s->segments.size();i++){
		nodeList[i%nodeList.size()].addSegmentToDownloadList(&s->segments[i]);
		for(unsigned int j=0;j<channel3GList.size();j++){
			if(channel3GList[j].receiver->id==nodeList[i%nodeList.size()].id){
				for(unsigned int k=0;k<s->segments[i].packets.size();k++){
					channel3GList[j].addPacket(s->segments[i].packets[k]);
				}
			}
		}
	}
	/*Show packets on channels*/
	showChannelBuffer();
}

void Simulator::simulate3GTransmissions(int timeSlot){

	channel3G::status=0;
	Node::useWlan=true; //Each time slot this flag is set to true so that any user can use the wlan
	for(unsigned int i=0;i<channel3GList.size();i++){
		channel3GList[i].sendPacket(timeSlot);
	}
}

void Simulator::simulateWIFIBroadcasts(int timeSlot){
	cout<<"timeslot "<<timeSlot+1<<"inside simulateWIFIBroadcasts"<<endl;
	if(timeSlot==0){
		broadcastNode=NULL;
	}
	bool flag=true;
	int packetsToRetransmit;
	segment *s ;
	for(unsigned int i=0;i<nodeList.size();i++){
		packetsToRetransmit=0;
		cout<<"----------------  Node "<<nodeList[i].id<<" timeslot="<<timeSlot<<"---------------------------"<<endl;
			flag=flag&nodeList[i].broadcastPacket(timeSlot,broadcastNode);
			nodeList[i].recvPacket(timeSlot);
			cout<<" ************   Checking for Messages  ****************"<<endl;
			nodeList[i].sendMessage(timeSlot);
			nodeList[i].recvMessage(timeSlot);
			cout.flush();
			cout<<"-------- Simulator checking message list -------"<<endl;
		    cout.flush();
			s=(nodeList[i].inMsgQueue.empty() ? NULL:nodeList[i].inMsgQueue.front()->segmentPtr);
			packetsToRetransmit=nodeList[i].checkRequests();

			if(packetsToRetransmit>0 && s!=NULL){	/*Code for adding  coded packets inside outPacketQueue*/
				cout<<"on timeSlot:"<<timeSlot<<"node "<<nodeList[i].id<<" has to send additional "<<packetsToRetransmit<<endl;
				nodeList[i].createPoolOfCodedPackets(s,packetsToRetransmit,timeSlot);
			}

			cout<<"-------- Simulator end checking message list -------"<<endl;
			cout.flush();
			nodeList[i].showSegInProgressMapping();
			nodeList[i].checkIncomingCodedPackets(timeSlot);
			cout.flush();
			cout<<" **********  End Checking for Messages  ****************"<<endl;
		cout<<"-----------------  End Node "<<nodeList[i].id<<" timeslot="<<timeSlot<<"---------------------------"<<endl;
		cout.flush();
	}
	if(flag){
		cout<<"on timeslot "<<timeSlot+1<<" i will set new brodacast Node if any"<<endl;
		setBroadcastNode();
	}
}


bool Simulator::checkNodeQueues(int timeSlot){/*returns true if there is no message to be transmitted or no packet to be transmitted over the network*/
	for(unsigned int i=0;i<nodeList.size();i++){
		if(!nodeList[i].outQueue.empty() || !nodeList[i].outMsgQueue.empty() || !nodeList[i].inMsgQueue.empty())
			return false;
	}
	return true;
}


void Simulator::showNodesBuffer(){
	for(unsigned int i=0;i<nodeList.size();i++){
		cout<<"node "<<nodeList[i].id;
		nodeList[i].showPacketQueue(nodeList[i].queue3G,"queue3G");
	}
}

void Simulator::showChannelBuffer(){

	for(unsigned int i=0;i<channel3GList.size();i++){
		channel3GList[i].showBuffer();
	}
}

void Simulator::showNodeSegmentList(){
	for(unsigned int i=0;i<nodeList.size();i++){
		nodeList[i].showSegQueue(nodeList[i].segQueue,"segments");
	}
}


void Simulator::showNodesIncomingPacketList(){
	int counterPackets[5];
	for(int i=0;i<5;i++)
		counterPackets[i]=0;

	for(unsigned int i=0;i<nodeList.size();i++){
		cout<<"Node"<<i+1<<" : "<<nodeList[i].inQueue.size()<<" ";
		for(unsigned int j=0;j<nodeList[i].inQueue.size();j++){
			cout<<nodeList[i].inQueue[j].id<<" ";
		}
		cout<<endl;
		cout<<"packets from 3G"<<endl;
		for(unsigned int j=0;j<nodeList[i].segQueue.size();j++){
			for(unsigned int z=0;z<nodeList[i].segQueue[j].packets.size();z++){
				counterPackets[i]++;
				cout<<counterPackets[i]<<")"<<nodeList[i].segQueue[j].packets[z].id<<" ";
			}

		}
		cout<<endl;
	}

	cout<<"Decoded packets:"<<endl;
	map<segment*,deque<Packet> >::iterator itTable;
	for(unsigned int i=0;i<nodeList.size();i++){

		cout<<"Node"<<i+1<<" : "<<nodeList[i].decodedMapping.size()<<"  size packets";
			for(itTable=nodeList[i].decodedMapping.begin();itTable!=nodeList[i].decodedMapping.end();itTable++){
				for(unsigned int j=0;j<(itTable->second).size();j++){
					counterPackets[i]++;
					if(itTable->first->getID()==1 && j==0 ){
							cout<<counterPackets[i]<<")"<<(itTable->second)[j].id<<" segID:"<<(itTable->second)[j].getSegmentID()<<" pkID:"<<(itTable->second)[j].getIndexInsideSegment()
							<<" "<<"payload[0]"<<(bitset<8>)((itTable->second)[j].payload[109])<<" ";
					}
					else
						cout<<counterPackets[i]<<")"<<(itTable->second)[j].id<<" segID:"<<(itTable->second)[j].getSegmentID()<<" pkID:"<<(itTable->second)[j].getIndexInsideSegment()<<" ";

				}
				cout<<endl;
				cout.flush();
			}

		cout<<endl;
	}
	cout<<"Total packets for each node"<<endl;
	cout<<"Node 1:"<<counterPackets[0]<<" segments in progress"<<nodeList[0].segInProgress.size()<<endl;
	cout<<"Node 2:"<<counterPackets[1]<<" segments in progress"<<nodeList[1].segInProgress.size()<<endl;
	cout<<"Node 3:"<<counterPackets[2]<<" segments in progress"<<nodeList[2].segInProgress.size()<<endl;
	cout<<"Node 4:"<<counterPackets[3]<<" segments in progress"<<nodeList[3].segInProgress.size()<<endl;
	cout<<"Node 5:"<<counterPackets[4]<<" segments in progress"<<nodeList[4].segInProgress.size()<<endl;
	cout.flush();

	cout<<"segments downloaded from 3G "<<endl;
	for(unsigned int i=0;i<nodeList.size();i++){
		nodeList[i].composeVideoFile();
	}
}

void Simulator::showOutQueue(){
	for(unsigned int i=0;i<nodeList.size();i++){
		cout<<"node "<<nodeList[i].id<<" out queue:"<<endl;
		nodeList[i].showPacketQueue(nodeList[i].outQueue,"out packet Queue");
		nodeList[i].showSegInProgressMapping();
		nodeList[i].showDecodedMapping(0);
	}
}
