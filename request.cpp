/*
 * request.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: vapashos
 */

#include "request.h"

request::request(int segID,int sID,int rID,int dim,int timeSlot):message(segID,sID,rID,timeSlot){
	type="request";
	dimensions=dim;
	cout<<"request"<<id<<" created for seg"<<segmentID<<" sender"<<senderID<<" receiver"<<receiverID<<" dimension "<<dimensions<<endl;
}


request::request(segment *x,int sID,int rID,int dim,int timeSlot):message(x,sID,rID,timeSlot){
	type="request";
	dimensions=dim;
	cout<<"request"<<id<<" created for seg"<<x->id<<" sender"<<senderID<<" receiver"<<receiverID<<" dimension "<<dimensions<<endl;

}

request::request(const request &r):message(r.segmentID,r.senderID,r.receiverID,r.timeCreated){
	type="request";
	dimensions=r.dimensions;
	segmentPtr=r.segmentPtr;
}

request::request(message *m,int timeSlot):message(m){
	type="request";
	dimensions=m->getDimensions();
	segmentID=m->segmentPtr->id;
	timeCreated=timeSlot;
}


request::~request() {
	// TODO Auto-generated destructor stub
}

bool request::isRequest(){
	return true;
}
bool request::isAdvertisment(){
	return false;
}

int request::getDimensions(){
	return dimensions;
}

string request::getMessageType(){
	return type;
}

/*Set functions*/
void request::setDimensions(int value){
	dimensions=value;
}

