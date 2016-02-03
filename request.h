/*
 * request.h
 *
 *  Created on: Feb 22, 2014
 *      Author: vapashos
 */

#ifndef REQUEST_H_
#define REQUEST_H_

#include "message.h"

class request: public message {
public:
	int dimensions;
	request(int segID,int sID,int rID,int dim,int timeSlot);
	request(segment *x,int sID,int rID,int dim,int timeSlot);
	request(const request &r);
	request(message *m,int timeSlot);
	virtual ~request();
	/*Virtual functions*/
	virtual bool isRequest();
	virtual bool isAdvertisment();
	virtual int getDimensions();
	virtual string getMessageType() ;
	/*set functions*/
	void setDimensions(int value);

};

#endif /* REQUEST_H_ */
