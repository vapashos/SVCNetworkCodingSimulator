/*
 * decoder.h
 *
 *  Created on: Mar 16, 2014
 *      Author: sportbilly
 */

#ifndef DECODER_H_
#define DECODER_H_
#include "ffNumber.h"
#include "Packet.h"
class decoder {
public:
	deque<deque<ffNumber> > matrix;
	finiteField *fp;
	decoder(finiteField *p);
	decoder(deque<deque<unsigned char> > &m,finiteField *p);
	decoder(deque<deque<ffNumber> > &m,finiteField *p);
	~decoder();
	/*functions*/
	void setMatrix(deque<deque<ffNumber> > &m);
	void setMatrix(deque<deque<unsigned char> > &m);
	void showMatrix();
	void gaussianElimination(deque<Packet> &pList);
	void decodePackets(deque<Packet> &pList);

};

#endif /* DECODER_H_ */
