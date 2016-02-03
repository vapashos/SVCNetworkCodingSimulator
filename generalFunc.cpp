/*
 * generalFunc.cpp
 *
 *  Created on: Feb 18, 2014
 *      Author: vapashos
 */

#include "generalFunc.h"
#include <cstdlib>
#include <sys/time.h>
#include <sstream>
using namespace std;


finiteField *ff=new finiteField(2,8);
int MTUSIZE=255;
string pathToStore="";
int randomNumber(int min,int max){
    //Generates the random
    struct timeval t;
    gettimeofday(&t,NULL);
    srand(t.tv_usec);
    return min+rand()%(max-min);
}

float randomNumber(int limit){

    //Generates floating random number less than the limit
    struct timeval t;
    gettimeofday(&t,NULL);
    srand(t.tv_usec);
    return float(rand()%(limit))/10.0;
}


int getLinesNumber(ifstream &f){
    int i=0;
    string line;
    while(getline(f,line)) i++;

    f.clear();//Clear the value of the flag eof
    f.seekg(0,ios::beg);//move back to the beginning of the file
    return i;
}

void moveInsideFile(ifstream &f,int moveStep)
{
    //Use this function to move inside the file
    //per line number
    int i=0;
    string line;
    f.clear();
    f.seekg(0,ios::beg);
    while(i<moveStep && f.eof()!=true)
    {
        getline(f,line);
        i++;
    }
}

void showTableInBytes(char *p,int size){
	for(int i=0;i<size;i++){
		if(i%15==0)
			cout<<endl;
		cout<<i+1<<")"<<bitset<8>(p[i])<<" ";
	}
	cout<<endl;
}

bool checkLinearDependencyForVector(deque<deque <ffNumber> > &x,const deque <ffNumber> &coefV,deque<deque <ffNumber> > &y,int generationSize){
	int numOfRows=0;
	int checkZeros=0;
	if (x.empty()){
		x.push_back(coefV);
		y.push_back(coefV);
		return true;
	}
	if(x.size()>=generationSize){/*10 is the generation size is the maximum number of packets a segment can have*/
		cout<<"This extra vector cannot be added something went wrong the segment should be already decoded."<<endl;
		return false;
	}
	else{
		x.push_back(coefV);
		numOfRows=x.size();
	}
	/*Operation to present zeros as the first element*/
	/*1.Find the number that multiplying we get the opposite from the previous row*/
	int z=0;
	int currentCoef;
	ffNumber ffn;
	cout<<"check if vector is linear independent"<<endl;
	while(z<numOfRows-1){
		/*find number with which if we multiply the z element of the new vector we 'll get the
		 * same number with the z element of the z line*/
		currentCoef=x[numOfRows-1][z].number;
		for(unsigned int p=0;p<ff->mul[currentCoef].size();p++){
			if(x[z][z].number==ff->mul[currentCoef][p]){
			/*Found the multiplier*/
		//		cout<<"current Coeff"<<currentCoef<<" multiplier found"<<p<<endl;
				ffn=ffNumber(p,ff);
				break;
			}
		}
		/*Multiplication all the elements of the row*/
	//	cout<<"After eliminating with "<<z+1<<"row results"<<endl;
		for(unsigned int i=z;i<x[numOfRows-1].size();i++){
			x[numOfRows-1][i]=ffNumber(ff->mul[x[numOfRows-1][i].number][ffn.number],ff);
			x[numOfRows-1][i]=ffNumber(ff->sum[x[numOfRows-1][i].number][x[z][i].number],ff);
			cout<<x[numOfRows-1][i].number<<" ";
			checkZeros+=x[numOfRows-1][i].number;
		}
		if(checkZeros==0){
			cout<<"found zero vector we have to remove it"<<endl;
			x.pop_back();
			return false;
		}
		cout<<endl;
		z++;
	}

	/*The vector inserted is linear dependent if after elimination the numOfRows x numOfRows element
	 * is equal to zero.*/
	/*Το μικρότερο απο 10 μπαινει ωστε να ελέγχονται μονο τα διανύσματα που αφορούν
	 * του τυχαίους συντελεστές των κωδικοποιημένων πακέτων*/
	//Αφαιρεθηκε γιατι δεν βρισκόταν σωστά τα Linear dependent διανύσματα
	//if(x[numOfRows-1][numOfRows-1].number==0 && numOfRows-1<10){
	//	cout<<"============vector=========="<<endl;
   //	for(unsigned int i=0;i<coefV.size();i++){
	//	cout<<" "<<coefV[i].number<<" ";
//		}
//		cout<<endl<<"found to be non linear independent"<<endl;
//		x.pop_back();
//		return false;
//	}
	//Προστέθηκε 11-11-2015 για να βρίσκει τα linear dependent διανύσματα
	for(int j=0;j<numOfRows-1;j++){
		if(x[numOfRows-1][j].number!=0){
			cout<<"vector that was just added is linear dependent "<<endl;
			x.pop_back();
			return false;
		}
	}
	if(x[numOfRows-1][numOfRows-1].number==0){
		cout<<"vector that was just added is linear dependent "<<endl;
		x.pop_back();
		return false;
	}


	y.push_back(coefV);
	return true;
}


void showTableCoef(const deque<deque <ffNumber> > &x){
	cout<<"Print the Table contents"<<endl;
	for(unsigned int i=0;i<x.size();i++){
		for(unsigned int j=0;j<x[i].size();j++){
			cout<<x[i][j].number<<" ";
		}
		cout<<endl;
	}
}



string int2Hex(int x){

	stringstream stream;
	stream << hex << x;
	string result1( stream.str() );
	string result2;
	for(unsigned int i=0;i<10-result1.size();i++){
		if(i==1)
			result2+="x";
		else
			result2+="0";
	}
	result2+=result1;
	return result2;
}
