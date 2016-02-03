/*
 * advertisment.h
 *
 *  Created on: Feb 22, 2014
 *      Author: vapashos
 */

#ifndef ADVERTISMENT_H_
#define ADVERTISMENT_H_

#include "message.h"

class advertisment: public message {
public:
	advertisment(int segID,int sID,int rID,int timeSlot);
	advertisment(segment *x,int sID,int rID,int timeSlot);
	advertisment(const advertisment &a);
	advertisment(message *m,int timeSlot);
	virtual ~advertisment();
	/*Virtual functions*/
	virtual bool isRequest();
	virtual bool isAdvertisment();
	virtual int getDimensions();
	virtual string getMessageType() ;

};

#endif /* ADVERTISMENT_H_ */
