/*
 * segment.h
 *
 *  Created on: Feb 27, 2014
 *      Author: vapashos
 */

#ifndef SEGMENT_H_
#define SEGMENT_H_
#include "Packet.h"

class segment {
public:
	static int counter;
	int id,size;/*size=size of segment in number of packets*/
	deque<Packet> packets;

	segment();
	segment(int id,deque<Packet> &x);
	segment(const segment &s);
	virtual ~segment();
	/*functions*/
	void addPacket(Packet &x);
	void showSegmentPackets();
	void showSegmentPackets(ofstream &f);
	int getSegmentLastPacketID();
	int getID();
	int getSize();
};

#endif /* SEGMENT_H_ */
