/*
 * Packet.h
 *
 *  Created on: Feb 17, 2014
 *      Author: vapashos
 */

#ifndef PACKET_H_
#define PACKET_H_
#include <deque>
#include <map>
#include <iostream>
#include <string>
#include "generalFunc.h"

using namespace std;

class Packet {
public:
	static int counter;
	int id,timeSend,senderID,receiverID,timeCreated,segLength;
	bool last;
	int segmentID;/*Segment id information is an information that doesnot need to be decoded*/
	//int header[7];/*coded information :segmentid,index inside segment,size in bytes,RTP packet id,lid,tid,qid*/
	ffNumber header[8];/*coded information :indexinsideSegment,size in bytes,RTP packet id,lid,tid,qid,NALUnitID,NALUnitIDMultiplier(is used for NALUintValues>255)*/
	/*header[1] carrying size information abou the packet is removed on this version*/
	/*Nal unit id is the number of record inside the traceFile.txt*/
	int headerSize;
	deque<unsigned char> payload;/*Maximum array length is MTUSIZE=255*/
	deque<unsigned char> coefVector;/*Coefficient vector*/


	Packet();
	Packet(char *pload,int size,int *subHeader,int segID,bool lastFlag=false,int id=Packet::counter++);
	Packet(char *pload,int from,int to,int *subHeader,int segID,bool lastFlag=false,int id=Packet::counter++);
	//Packet(char *pload,int from,int to,int *subHeader,int segID,bool lastFlag=false,int id=Packet::counter++);
	Packet(unsigned char *pload,int size,ffNumber *hder,int length,int segID,int id=Packet::counter++);
	Packet(deque<unsigned char> &pload,int size,bool lastFlag=false,int id=Packet::counter++);
	Packet(const Packet &p);

	virtual ~Packet();
	/*set Functions*/
	void setID(int id);
	void setSegmentID(int segID,int packIndex);
	void setPayload(deque<unsigned char> &pLoad);
	void setSenderID(int sID);
	void setReceiverID(int rID);
	void setLast(bool l=true);
	void setFiniteField(finiteField *f);
	void setCoefVector(const deque<unsigned char> &coef);
	void setTimeCreated(int timeSlot);
	void setSegmentLength(int length);
	/*Get Functions*/
	int getSegmentID() const;
	int getIndexInsideSegment() const;
	/**/
	bool comparePackets(const Packet &p1);
	bool isLast() const;
	/*show functions*/
	void showPayload();
	void showPayload(ofstream &f);
	void showSegmentInfo();
	void showCoefVector();
};


/*operator overloading*/
Packet operator * (const Packet& a,int x);
Packet operator * (int x,const Packet &a);
Packet operator * (ffNumber &x,const Packet &a);
Packet operator * (const Packet &a,ffNumber &x);
Packet operator + (const Packet& a,const Packet& b);



#endif /* PACKET_H_ */
