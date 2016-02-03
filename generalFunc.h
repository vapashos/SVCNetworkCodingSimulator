/*
 * generalFunc.h
 *
 *  Created on: Feb 18, 2014
 *      Author: vapashos
 */

#ifndef GENERALFUNC_H_
#define GENERALFUNC_H_
#include <iostream>
#include <fstream>
#include <bitset>
#include "ffNumber.h"
using namespace std;
int randomNumber(int min,int max);
float randomNumber(int limit);
int getLinesNumber(ifstream &f);
void moveInsideFile(ifstream &f,int moveStep);
void showTableInBytes(char *p,int size);
bool checkLinearDependencyForVector(deque<deque <ffNumber> > &x,const deque <ffNumber> &coefV,deque<deque <ffNumber> > &y,int generationSize=10);
void showTableCoef(const deque<deque <ffNumber> > &x);
string int2Hex(int x);
extern finiteField *ff;
extern int MTUSIZE;
extern string pathToStore;
#endif /* GENERALFUNC_H_ */
