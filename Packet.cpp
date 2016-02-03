/*
 * Packet.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: vapashos
 */

#include "Packet.h"

int Packet::counter=1;
Packet::Packet() {

	id=counter;
	last=false;
}

/*Constructor 1*/
Packet::Packet(char *pload,int size,int *subHeader,int segID,bool lastFlag,int id){
//	cout<<"Inside Constructor 1"<<endl;
	this->id=id;
	last=lastFlag;
	headerSize=8;
	//header[0]=ffNumber(0,ff);
	header[0]=ffNumber(0,ff);
	header[1]=ffNumber(0,ff);
	//Remove this on this version header[1]=ffNumber(size,ff);
	//header[2]=ffNumber(subHeader[0],ff);/*RTP packetID this was only for maximum 255 value we can derive RPTpacketID from sizei of line minus 2*/
	header[2]=ffNumber(0,ff);
	header[3]=ffNumber(subHeader[1],ff);/*lid*/
	header[4]=ffNumber(subHeader[2],ff);/*tid*/
	header[5]=ffNumber(subHeader[3],ff);/*qid*/
	header[7]=ffNumber(subHeader[4]/255,ff);/*NaluID */
	header[6]=ffNumber(subHeader[4]-255*header[7].number,ff);/*Naluid*/
	timeCreated=0;
	for(int i=0;i<size;i++){
		payload.push_back(pload[i]);
	}
	segmentID=segID;
	segLength=0;

}
/*Constructor 2*/
Packet::Packet(char *pload,int from,int to,int *subHeader,int segID,bool lastFlag,int id){

	this->id=id;
	headerSize=8;
	header[0]=ffNumber(0,ff);/*index inside segment*/
	header[1]=ffNumber(0,ff);	/*size*/
	header[2]=ffNumber(0,ff);/*RTP packetID*/
	header[3]=ffNumber(subHeader[1],ff);/*lid*/
	header[4]=ffNumber(subHeader[2],ff);/*tid*/
	header[5]=ffNumber(subHeader[3],ff);/*qid*/
	header[7]=ffNumber(subHeader[4]/255,ff);/*NalunitMultiplier*/
	header[6]=ffNumber(subHeader[4]-255*header[7].number,ff);/*NalUnitID*/
	timeCreated=0;
	cout<<"packet id "<<id<<"payload from "<<from<<" to "<<to<<endl;
	for(int i=0;i<=to-from;i++){
		payload.push_back(pload[i]);
	}
	segmentID=segID;
	segLength=0;
}

/*Constructor 3*/
Packet::Packet(unsigned char *pload,int size,ffNumber *hder,int length,int segID,int id){
	/*This constructor used inside overloaded operators creating coding packets*/
	headerSize=8;
	this->id=id;
	for(int i=0;i<8;i++)
			header[i]=hder[i];

	for(int i=0;i<size;i++){
			payload.push_back(pload[i]);
	}
	segmentID=segID;
	segLength=length;
	timeCreated=0;
}


Packet::Packet(deque<unsigned char> &pload,int size,bool lastFlag,int id){
//Constructor used when creating packets of AVC bitstreams ,array header has no information cause there are no layers and no trace file
	headerSize=8;
	this->id=id;
	for(int i=0;i<8;i++)
			header[i]=ffNumber(0,new finiteField(2,8));//fill with zeros

	for(int i=0;i<size;i++){
			payload.push_back(pload[i]);
	}
	timeCreated=0;
}


Packet::Packet(const Packet &p){

	id=p.id;
	payload=p.payload;
	coefVector=p.coefVector;
	segmentID=p.segmentID;
	for(int i=0;i<8;i++)
		header[i]=p.header[i];
	timeCreated=p.timeCreated;
	timeSend=p.timeSend;
	senderID=p.senderID;
	receiverID=p.receiverID;
	last=p.last;
	segLength=p.segLength;
	headerSize=p.headerSize;

}

Packet::~Packet() {
	payload.clear();
	coefVector.clear();
}

/*Set functions */
void Packet::setID(int id){
	this->id=id;
}

void Packet::setSegmentID(int segID,int packIndex){

	segmentID=segID;
	header[0]=ffNumber(packIndex,ff);
	cout<<"packet"<<id<<" i set segmentid:"<<segmentID<<endl;
}

void Packet::setPayload(deque<unsigned char> &pLoad){
		payload=pLoad;
}

void Packet::setSenderID(int sID){
	senderID=sID;
}

void Packet::setReceiverID(int rID){
	receiverID=rID;
}

void Packet::setLast(bool l){
	last=l;
}

void Packet::setCoefVector(const deque<unsigned char> &coef){
	coefVector=coef;
}
void Packet::setTimeCreated(int timeSlot){
	timeCreated=timeSlot;
}

void Packet::setSegmentLength(int length){
	segLength=length;
}


/*Get functions*/

int Packet::getSegmentID() const{
	return segmentID;
}
int Packet::getIndexInsideSegment() const{
	return header[0].number;
}

/*Show function*/

void Packet::showPayload(){
	cout<<"packet "<<id<<" and size "<<payload.size()<<endl;
	for(unsigned int i=0;i<payload.size();i++){
		if(i%10==0)
			cout<<endl;
		cout<<i+1<<")"<<bitset<8>(payload[i])<<" ";
	}
	cout<<"*---------------------------------------*"<<endl;
}

void Packet::showPayload(ofstream &f){
	f<<"packet "<<id<<" and size "<<payload.size()<<endl;
	f<<"metadata:segmentID"<<this->segmentID<<" index inside segment"<<header[0]<<" size in bytes"<<payload.size()<<" NALUnit "<<header[6].number<<endl;
	for(unsigned int i=0;i<payload.size();i++){
		if(i%15==0)
			f<<endl;
		f<<i+1<<")["<<(int)(payload[i])<<"]"<<bitset<8>(payload[i])<<" ";
		f.flush();
	}
	f<<endl<<"*---------------------------------------*"<<endl;
}


void Packet::showCoefVector(){
	cout<<"packet"<<id<<" coeff Vector:";
	for(unsigned int i=0;i<coefVector.size();i++){
		cout<<(int)coefVector[i]<<" ";
	}
	cout<<endl;
}

bool Packet::comparePackets(const Packet &p1){

	int size=(this->payload.size()<p1.payload.size()) ? this->payload.size():p1.payload.size();
	for(int i=0;i<size;i++){
		if(this->payload[i]!=p1.payload[i])
			return false;
	}
	return true;
}

bool Packet::isLast() const{
	return last;
}

/*Overloaded Operators*/

Packet operator * (const Packet& a,int x){

	/*Update header information */
	ffNumber tempHeader[8];
	/*tempHeader[0] corresponts to segmentID its information that doesn't need to be coded cause we only create
	 * coded packets of packets that belong on the same segment */
	int tempSegID=a.segmentID;
	int sizePacket=a.payload.size();
	for(int i=0;i<8;i++){
		tempHeader[i]=a.header[i]*x;
	}
	unsigned char tempPayload[sizePacket];

	for(int i=0;i<sizePacket;i++){
		tempPayload[i]=ff->mul[a.payload[i]][x];
	}
	return Packet(tempPayload,sizePacket,tempHeader,0,a.segmentID,0);
}

Packet operator * (int x,const Packet &a){
	return a*x;
}


Packet operator + (const Packet& a,const Packet& b){
	ffNumber tempHeader[8];
	int sizePacketA=a.payload.size();
	int sizePacketB=b.payload.size();
	int sizePacketMin;
	int sizePacketMax;
	for(int i=0;i<8;i++){
		tempHeader[i]=a.header[i]+b.header[i];
	}
	//Get the max of the size
	sizePacketMax = sizePacketA>sizePacketB ? sizePacketA:sizePacketB;
	sizePacketMin = sizePacketA>sizePacketB ? sizePacketB:sizePacketA;


	unsigned char tempPayload[sizePacketMax];

	for(int i=0;i<sizePacketMin;i++){
		tempPayload[i]=ff->sum[a.payload[i]][b.payload[i]];
	}

	for(int i=sizePacketMin;i<sizePacketMax;i++){
			if (sizePacketA>sizePacketB)
				tempPayload[i]=ff->sum[a.payload[i]][0];
			else
				tempPayload[i]=ff->sum[0][b.payload[i]];
	}
	return Packet(tempPayload,sizePacketMax,tempHeader,0,a.segmentID,0);
}



