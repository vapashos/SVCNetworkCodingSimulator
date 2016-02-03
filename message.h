/*
 * message.h
 *
 *  Created on: Feb 22, 2014
 *      Author: vapashos
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <iostream>
#include "segment.h"
using namespace std;
class message {
public:
	static int counter;
	int id,senderID,receiverID,segmentID,timeSend,timeCreated;
	segment *segmentPtr;
	string type;
	message(int segID,int sID,int rID,int timeSlot,int id=message::counter++);
	message(segment *x,int sID,int rID,int timeSlot,int id=message::counter++);
	message(const message &m);
	message(message *m);

	/*functions*/
	void setSenderID(int sID);
	void setReceiverID(int rID);
	void setTimeSend(int timeSlot);
	/*Pure virtual functions*/
	virtual bool isRequest();
	virtual bool isAdvertisment();
	virtual int getDimensions();
	virtual string getMessageType();

};

#endif /* MESSAGE_H_ */
