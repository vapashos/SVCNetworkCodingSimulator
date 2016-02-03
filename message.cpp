/*
 * message.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: vapashos
 */

#include "message.h"

int message::counter=0;

message::message(int segID,int sID,int rID,int timeSlot,int id){

	this->id=id;
	segmentID=segID;
	receiverID=rID;
	senderID=sID;
	timeSend=-1;
	timeCreated=timeSlot;
}

message::message(segment *x,int sID,int rID,int timeSlot,int id){
	this->id=id;
	segmentPtr=x;
	segmentID=x->id;
	receiverID=rID;
	senderID=sID;
	timeSend=-1;
	timeCreated=timeSlot;

}

message::message(const message &m){
	id=m.id;
	segmentID=m.segmentID;
	receiverID=m.receiverID;
	senderID=m.senderID;
	timeSend=m.timeSend;
	timeCreated=m.timeCreated;
	segmentPtr=m.segmentPtr;
}

message::message(message *m){
	id=m->id;
	segmentID=m->segmentID;
	senderID=m->senderID;
	receiverID=m->receiverID;
	timeSend=m->timeSend;
	timeCreated=m->timeCreated;
	segmentPtr=m->segmentPtr;

}

void message::setSenderID(int sID){
	senderID=sID;
}

void message::setReceiverID(int rID){
	receiverID=rID;
}

void message::setTimeSend(int timeSlot){
	timeSend=timeSlot;
}

bool message::isRequest(){
	return false;
}

bool message::isAdvertisment(){
	return false;
}

int message::getDimensions(){
	return -1;
}

string message::getMessageType(){
	return "-";
}



