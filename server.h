/*
 * server.h
 *
 *  Created on: Feb 25, 2014
 *      Author: sportbilly
 */

#ifndef SERVER_H_
#define SERVER_H_
#include "segment.h"
#include "generalFunc.h"
#include "Node.h"
#include <fstream>

class server {
public:
	ifstream bitstream;
	ifstream trace;
	deque <deque<int> > packetMetaData;/*2 Dimension Array with number of trace file Lines rows and 4 columns(packet size in bytes,Lid,Tid,Qid) */
	deque<segment> segments;
	deque<Packet> packets;
	int RTPpackets;
	int generationSize;/*Generation size*/
	server(int genSize,string bitStreamFile,string traceFile);
	~server();
	/*functions*/
	void loadBitstreamFile(string bitstreamFile);
	void loadTraceFile(ifstream &f,string fileName);/*This function opens the trace file and fills the packetMetaData table*/
	void streamRTPPacketization(ifstream &bitStream,string fileName);/*Create a sequence of packet with the data of the stream*/
	void streamAVCPacketization(ifstream &bitStream,string fileName);
	void segmentation();/*After creating packets we create segments of the file combining packets*/
	void segmentation2();
	void createPackets();
	void createSegments();
	void showSegmentLastPacketIDs();/*it prints the id of the packets for each segment*/
	/*Help function*/
	void checkPayloadSize();
	void createCodedPacket(const segment &s);
	/*functions to write result on file*/
	void writeSegmentationDataOnFile(string filename);
};

#endif /* SERVER_H_ */
