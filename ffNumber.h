/*
 * ffNumber.h
 *
 *  Created on: Feb 11, 2014
 *      Author: vapashos
 */

#ifndef FFNUMBER_H_
#define FFNUMBER_H_
#include "finiteField.h"

class ffNumber {
public:
	int number;
	finiteField *ff;
	ffNumber();
	ffNumber(int n,finiteField *x);
	ffNumber(const ffNumber &x);
	virtual ~ffNumber();
};


ffNumber operator *(int x,const ffNumber &a);
ffNumber operator *(const ffNumber &a,int x);
ffNumber operator *(const ffNumber &a,const ffNumber &b);
ffNumber operator +(const ffNumber &a,const ffNumber &b);
ffNumber operator +(const ffNumber &a,int x);
ffNumber operator +(const ffNumber &a,int x);
ffNumber operator /(int x,const ffNumber &a);
ffNumber operator /(const ffNumber &a,int x);
ffNumber operator /(const ffNumber &a,const ffNumber &b);
ostream& operator <<(ostream &os,const ffNumber &a);
#endif /* FFNUMBER_H_ */
