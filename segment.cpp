/*
 * segment.cpp
 *
 *  Created on: Feb 27, 2014
 *      Author: vapashos
 */

#include "segment.h"

int segment::counter=0;


segment::segment() {
	counter++;
	id=counter;
	cout<<"segment with ID"<<id<<" created"<<endl;
}

segment::segment(const segment &s){
	counter++;
	id=s.id;
	packets=s.packets;
}

segment::segment(int id,deque<Packet> &x){
	this->id=id;
	packets=x;
}

segment::~segment() {
	counter--;
	packets.clear();
	// TODO Auto-generated destructor stub
}



void segment::addPacket(Packet &x){

	if(!packets.empty())
		packets.back().setLast(false);

	x.setLast(true);
	packets.push_back(x);
	size=packets.size();
}

void segment::showSegmentPackets(){
	cout<<"segment"<<id<<":"<<endl;
	for(unsigned int i=0;i<packets.size();i++){
		cout<<"p"<<packets[i].id<<" ";
	}
	cout<<endl;
}

void segment::showSegmentPackets(ofstream &f){
	for(unsigned int i=0;i<packets.size();i++){
			f<<"p"<<packets[i].id<<" ";
	}
	f<<endl;
}

int segment::getSegmentLastPacketID(){
	for(unsigned int i=0;i<packets.size();i++){
		if(packets[i].isLast()){
			return packets[i].id;
		}
	}
	return -1;
}

int segment::getID(){
	return id;
}

int segment::getSize(){
	return packets.size();
}

