/*
 * channelWlan.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: vapashos
 */

#include "channelWlan.h"

int channelWlan::counter=0;

channelWlan::channelWlan() {
	counter++;
	id=counter;
	lossProb=0.0;

}

channelWlan::channelWlan(float prob){
	counter++;
	id=counter;
	lossProb=prob;
	cout<<"Created channel"<<id<<" Wlan with probability"<<prob<<endl;
}


channelWlan::~channelWlan() {
	counter--;
	// TODO Auto-generated destructor stub
}

void channelWlan::showChannelContents(){
	if(buffer.empty())
		return;
	cout<<"----------------------------"<<endl;
	for(unsigned int i=0;i<buffer.size();i++){
		cout<<"p"<<buffer[i].id<<" ";
	}
	cout<<endl;
	for(unsigned int i=0;i<buffer.size();i++){
			cout<<"tS:"<<buffer[i].timeSend<<" ";
	}
	cout<<endl;
	for(unsigned int i=0;i<buffer.size();i++){
		cout<<"sID"<<buffer[i].senderID<<" ";
	}
	cout<<endl;
	for(unsigned int i=0;i<buffer.size();i++){
			cout<<"rID"<<buffer[i].receiverID<<" ";
	}
	cout<<endl<<"----------------------------"<<endl;
}

void channelWlan::showMsgBuffer(){
	if(msgBuffer.empty())
			return;
		cout<<"----------------------------"<<endl;
		for(unsigned int i=0;i<msgBuffer.size();i++){
			cout<<"msg"<<msgBuffer[i]->id<<" \t";
		}
		cout<<endl;
		for(unsigned int i=0;i<msgBuffer.size();i++){
				cout<<"tS:"<<msgBuffer[i]->timeSend<<" \t";
		}
		cout<<endl;
		for(unsigned int i=0;i<msgBuffer.size();i++){
			cout<<"type:"<<msgBuffer[i]->getMessageType()<<" ";
		}
		cout<<endl;
		for(unsigned int i=0;i<msgBuffer.size();i++){
			cout<<"sndr:"<<msgBuffer[i]->senderID<<" ";
			cout<<"rcvr:"<<msgBuffer[i]->receiverID<<" \t";
		}
		cout<<endl;
		for(unsigned int i=0;i<msgBuffer.size();i++){
			cout<<"segPtr"<<msgBuffer[i]->segmentPtr->id<<" \t";
		}
		cout<<endl<<"----------------------------"<<endl;
}


