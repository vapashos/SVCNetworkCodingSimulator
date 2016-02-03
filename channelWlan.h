/*
 * channelWlan.h
 *
 *  Created on: Feb 17, 2014
 *      Author: vapashos
 */

#ifndef CHANNELWLAN_H_
#define CHANNELWLAN_H_
#include "Packet.h"
#include "message.h"
class channelWlan {
public:
	static int counter;
	int id;
	float lossProb;
	deque<Packet> buffer;
	deque<message*> msgBuffer;
	void showChannelContents();
	void showMsgBuffer();
	/*Constructors*/
	channelWlan();
	channelWlan(float prob);
	virtual ~channelWlan();

};

#endif /* CHANNELWLAN_H_ */
