/*
 * ffNumber.cpp
 *
 *  Created on: Feb 11, 2014
 *      Author: vapashos
 */

#include "ffNumber.h"


ffNumber::ffNumber(){

	number=0;
	ff=NULL;

}



ffNumber::ffNumber(int n,finiteField *x) {
	number=n;
	ff=x;

}

ffNumber::ffNumber(const ffNumber &x){
	number=x.number;
	ff=x.ff;
}



ffNumber::~ffNumber() {

}


ffNumber operator *(const ffNumber &a,int x){
		return ffNumber(a.ff->mul[a.number][x],a.ff);
}

ffNumber operator *(int x,const ffNumber &a){
	return ffNumber(a.ff->mul[a.number][x],a.ff);
}

ffNumber operator *(const ffNumber &a,const ffNumber &b){
	char c;
	if(a.number>255 || b.number>255){
		cout<<"Error in finite numbers"<<a.number<<" and "<<b.number<<endl;
		cin>>c;
	}
	return(ffNumber(a.ff->mul[a.number][b.number],a.ff));
}

ffNumber operator +(const ffNumber &a,const ffNumber &b){
	return(ffNumber(a.ff->sum[a.number][b.number],a.ff));
}

ffNumber operator /(const ffNumber &a,const ffNumber &b){
	return(ffNumber(a.ff->div[a.number][b.number],a.ff));
}


ffNumber operator /(int x,const ffNumber &a){
	return(ffNumber(a.ff->div[x][a.number],a.ff));
}

ostream& operator <<(ostream &os,const ffNumber &a){
	os<<a.number;
	return os;
}


