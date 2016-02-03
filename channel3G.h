/*
 * channel3G.h
 *
 *  Created on: Feb 23, 2014
 *      Author: vapashos
 */

#ifndef CHANNEL3G_H_
#define CHANNEL3G_H_
#include "Node.h"
#include "server.h"

class channel3G {
public:
	static int counter;
	static int status;/*static variable been used to check if all channels are empty */
	int id;
	float lossProb;
	deque<Packet> buffer;
	Node *receiver;
	server *sender;
	channel3G(server *s,Node *user,float lossProb);
	virtual ~channel3G();
	void addPacket(Packet &p);
	void sendPacket(int timeSlot);
	void receivePacket(int timeSlot);
	void showBuffer();
};

#endif /* CHANNEL3G_H_ */
