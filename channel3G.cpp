/*
 * channel3G.cpp
 *
 *  Created on: Feb 23, 2014
 *      Author: vapashos
 */

#include "channel3G.h"

int channel3G::counter=0;
int channel3G::status=0;
channel3G::channel3G(server *s,Node *user,float lossProb) {
	counter++;
	id=counter;
	receiver=user;
	sender=s;
	this->lossProb=lossProb;
	cout<<"user"<<receiver->id<<" connected with server on channel"<<id<<"with loss probability"<<lossProb<<endl;

}

channel3G::~channel3G() {
	// TODO Auto-generated destructor stub
}


void channel3G::addPacket(Packet &p){

	buffer.push_back(p);

}

void channel3G::sendPacket(int timeSlot){
	if(buffer.empty()){
		cout<<"channel"<<id<<" all packet's has been sent"<<endl;
		return;
	}
	status=id;
	if(lossProb<randomNumber(10)){
		Packet Temp=buffer.front();
		Temp.timeSend=timeSlot;
		receiver->recv3GPacket(Temp,timeSlot);
		buffer.pop_front();
	}
	else{
		cout<<"channel3G"<<id<<"::sendPacket-transmission of packet with id "<<buffer.front().id<<" failed"<<endl;
	}
	//If packet transmission fails the packet will try to be sent the next time slot
}

void channel3G::showBuffer(){

	cout<<"channel3G id:"<<id<<" |";
	for(unsigned int i=0;i<buffer.size();i++){
		cout<<"p"<<buffer[i].id<<" ";
	}
	cout<<" --------------  ";
	receiver->showPacketQueue(receiver->queue3G,"queue3G");
}

