/*
 * advertisment.cpp
 *
 *  Created on: Feb 22, 2014
 *      Author: vapashos
 */

#include "advertisment.h"

advertisment::advertisment(int segID,int sID,int rID,int timeSlot):message(segID,sID,rID,timeSlot){
	type="advertisment";
	cout<<"advertisment"<<id<<" created for seg"<<segmentID<<" sender"<<senderID<<" receiver"<<receiverID<<endl;
}

advertisment::advertisment(segment *x,int sID,int rID,int timeSlot):message(x,sID,rID,timeSlot){
	type="advertisment";
	cout<<"advertisment"<<id<<" created for seg"<<x->id<<" sender"<<senderID<<" receiver"<<receiverID<<endl;
}


advertisment::advertisment(const advertisment &a):message(a.segmentID,a.senderID,a.receiverID,a.timeCreated){
	type="advertisment";
	char c;
	if(a.segmentPtr==NULL)
		cin>>c;
	segmentPtr=a.segmentPtr;
}


advertisment::advertisment(message *m,int timeSlot):message(m){
	type="advertisment";
	segmentID=m->segmentPtr->id;
	timeCreated=timeSlot;
}

advertisment::~advertisment() {
	// TODO Auto-generated destructor stub
}

/*Virtual functions*/
bool advertisment::isRequest(){
	return false;
}
bool advertisment::isAdvertisment(){
	return true;
}

int advertisment::getDimensions(){
	return 0;
}

string advertisment::getMessageType(){
	return type;
}
