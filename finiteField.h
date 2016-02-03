/*
 * finiteField.h
 *
 *  Created on: Jan 29, 2014
 *      Author: vapashos
 */

#ifndef FINITEFIELD_H_
#define FINITEFIELD_H_
#include <deque>
#include <iostream>

using namespace std;

class finiteField
{

public:
	static int primitive_polynomial[];
	deque<deque<int> > sum;
	deque<deque<int> > div;
	deque<deque<int> > mul;
	int q;//cardinality of the field


	finiteField(int b,int m);
	virtual ~finiteField();
	int findIndexOnTable(deque<deque<int> > &table,int x,int res);

};

#endif /* FINITEFIELD_H_ */
