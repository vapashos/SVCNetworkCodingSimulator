/*
 * finiteField.cpp
 *
 *  Created on: Jan 29, 2014
 *      Author: vapashos
 */

#include "finiteField.h"
#include <cassert>
#include <cmath>
#include <fstream>
int finiteField::primitive_polynomial[]={ 3, 7, 11, 19, 37, 67, 137,
        285, 529,1033,2053,4179,
        8219,17475, 32771, 69643 };

finiteField::finiteField(int b,int m)
{
	 assert(b > 1 && m > 0);
	 assert(b == 2 && m <= 16);
	 q=static_cast<int> (pow((float)b,(float)m));
	 int c=primitive_polynomial[m-1]-(1<<m);

	 ofstream s("sum.txt");
	 ofstream mu("mul.txt");
	 ofstream d("div.txt");
	 //Create a 256x256 table with the result of operation addition
	 for(int i=0;i<q;i++)
	 {
		 sum.push_back(deque<int>(q));
		 mul.push_back(deque<int>(q));
		 div.push_back(deque<int>(q));
		 for(int j=0;j<q;j++)
		 {
			 sum[i][j]=i^j;
			 s<<sum[i][j]<<" ";
			 int a=i;
			 int b=j;
			 int p=0;

			 /*paesant's algorithm*/
			 for (int k = 0 ; k < m ; k++)
			 {
				 if ( (b & 0x1) == 1)
				 {
					 p = p ^ a;
				 }
				 int r = (a & (0x1 << (m-1))) > 0;
				 a = (a << 1) % q;
				 if (r)
				 {
					 a = a ^ c;
				 }
				 b = b >> 1;
			 }
			 mul[i][j]=p;
			 mu<<mul[i][j]<<" ";
		 }
		 s<<endl;
		 mu<<endl;

	 }

	 for(int i=0;i<q;i++)
	 {
		 for(int j=0;j<q;j++)
		 {
			 div[mul[i][j]][i]=j;
			 div[mul[i][j]][j]=i;
			 d<<div[i][j]<<" ";
		 }
		 d<<endl;
	 }

	 s.close();
	 mu.close();
	 d.close();

}

finiteField::~finiteField() {
	// TODO Auto-generated destructor stub
}


int finiteField::findIndexOnTable(deque<deque<int> > &table,int x,int res)
{
	for(unsigned int i=0;i<table[x].size();i++){
		if(table[x][i]==res)
			return i;
	}
}

