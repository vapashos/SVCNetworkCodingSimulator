/*
 * Simulator.h
 *
 *  Created on: Feb 17, 2014
 *      Author: sportbilly
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_
#include "channel3G.h"
#include "server.h"

class Simulator {
public:
	deque<Node> nodeList;
	deque<channel3G> channel3GList;
	Node *broadcastNode;
	server *s;
	Simulator(int nodesNum,string bitStreamfileName,string traceFileName);
	void initNodeList();
	void createMeshTopology();
	bool setBroadcastNode();
	void allocateSegmentsOnUsers(server *s);
	void simulate3GTransmissions(int timeSlot);
	void simulateWIFIBroadcasts(int timeSlot);
	bool checkNodeQueues(int timeSlot);
	void showNodesBuffer();
	void showChannelBuffer();
	void showNodeSegmentList();
	void showNodesIncomingPacketList();
	void showOutQueue();
	~Simulator();
};

#endif /* SIMULATOR_H_ */
