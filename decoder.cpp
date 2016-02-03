/*
 * decoder.cpp
 *
 *  Created on: Mar 16, 2014
 *      Author: sportbilly
 */

#include "decoder.h"


decoder::decoder(finiteField *p) {
	fp=p;
}


decoder::decoder(deque<deque<unsigned char> > &m,finiteField *p) {

	fp=p;
	setMatrix(m);
}


decoder::decoder(deque<deque<ffNumber> > &m,finiteField *p) {
	matrix=m;
	fp=p;
}

decoder::~decoder() {
	matrix.clear();
	delete fp;
}


void decoder::setMatrix(deque<deque<ffNumber> > &m){
	if(!matrix.empty())
		matrix.clear();
	matrix=m;
}

void decoder::setMatrix(deque<deque<unsigned char> > &m){
	/*Convert bytes in ffNumbers for ease of use inside the decoder*/
	if(!matrix.empty())
		matrix.clear();
	deque<ffNumber> Temp;
	for(unsigned char i=0;i<m.size();i++){
		for(unsigned char j=0;j<m[i].size();j++){
			Temp.push_back(ffNumber((int)m[i][j],fp));
		}
		matrix.push_back(Temp);
		Temp.clear();
	}
}

void decoder::showMatrix(){

	cout<<"Show the Matrix inside decoder :"<<endl;
	for(unsigned int i=0;i<matrix.size();i++){
		cout<<i+1<<")";
		for(unsigned int j=0;j<matrix[i].size();j++){
			cout<<matrix[i][j].number<<" ";
		}
		cout<<endl;
	}

}


void decoder::gaussianElimination(deque<Packet> &pList){

	deque<ffNumber> temp;

	cout<<"------------GAUSS ELIMINATION -----"<<endl;
	ofstream f1("gauss.txt");
	ffNumber ffn;
	f1<<"segment "<<pList[0].segmentID<<endl;
	for(unsigned int j=0;j<matrix[0].size();j++){
		f1<<"------  eliminate elements of the  ------"<<j+1<<" column"<<endl;
		for(unsigned int i=j+1;i<matrix.size();i++){//for every row greater than 0
			//Eliminate element matrix[i][j]
			for(unsigned int p=0;p<fp->mul[matrix[j][j].number].size();p++){
				if(fp->mul[matrix[j][j].number][p]==matrix[i][j].number){
					ffn=ffNumber(p,fp);
					break;
				}
			}
			f1<<"multiply with "<<ffn.number<<" to create zeros for row "<<i+1<<endl;
			int payloadIndex=0;
			int packetHeaderIndx=0;
			//Create row vector temp to make the temp operations
			for(unsigned int t=0;t<matrix[0].size()+8+pList[j].payload.size();t++){/*8 einai ta stoixeia tou header dianismatos pou kwdikopoiountai + to mikos tou payload*/
				if(t<matrix[0].size()){
					temp.push_back(matrix[j][t]*ffn);
					matrix[i][t]=matrix[i][t]+temp[t];
					f1<<matrix[i][t].number<<" ";
				}
				else if(t>matrix[0].size()+7){/*6 einai to headerSize - 1*/
					temp.push_back((int)pList[j].payload[payloadIndex]*ffn);
					pList[i].payload[payloadIndex]=(ffNumber((int)pList[i].payload[payloadIndex],fp)+temp[t]).number;
					if(payloadIndex==0)
						f1<<endl<<"payload | "<<endl;
					f1<<payloadIndex+1<<")"<<(int)pList[i].payload[payloadIndex]<<" ";
					payloadIndex++;
				}
				else{
					temp.push_back(pList[j].header[packetHeaderIndx]*ffn);
					pList[i].header[packetHeaderIndx]=(pList[i].header[packetHeaderIndx]+temp[t]);
					if(packetHeaderIndx==0)
						f1<<endl<<"Header| ";
					f1<<pList[i].header[packetHeaderIndx].number<<" ";
					packetHeaderIndx++;
				}
			}
			temp.clear();
		}
		f1<<"------------- eliminate elements of the  "<<j+1<<" column finished"<<endl;
	}
	f1.close();
}

void decoder::decodePackets(deque<Packet> &pList){
		gaussianElimination(pList);
		deque<Packet> decPackets;
		//	Υποθέτουμε ότι το μέγεθος του Payload είναι το ίδιο για όλα τα πακέτα
		int headerIndx;
		cout<<endl<<"?-------------------------------------?"<<endl;
		char c;
		for(int i=matrix.size()-1;i>=0;i--){
			headerIndx=0;
			/*7 is the size of additional data metadata*/
			for(unsigned int j=0;j<pList[i].payload.size()+8;j++){//8 is the size of header
				ffNumber temp=ffNumber(0,fp);
				if(j<pList[i].payload.size()){
					for(int w=matrix.size()-1;w>i;w--){
					/*Compute result that has to with additions and multiplications*/
						temp=temp+((int)pList[w].payload[j])*matrix[i][w];
					}
					pList[i].payload[j]=((ffNumber((int)pList[i].payload[j],fp)+temp)/matrix[i][i]).number;
				}
				else{
					for(int w=matrix.size()-1;w>i;w--){
					/*Compute result that has to with additions and multiplications*/
						temp=temp+pList[w].header[headerIndx]*matrix[i][w];
					}
					pList[i].header[headerIndx]=(pList[i].header[headerIndx]+temp)/matrix[i][i];
					headerIndx++;
				}
			}
			cout.flush();
		}
		ofstream f("decodingResult.txt");
		f<<"Decoding packets of segment "<<pList.front().segmentID<<endl;
		for(unsigned int i=0;i<pList.size();i++){
			f<<"packet with id "<<pList[i].header[0].number<<","<<pList[i].payload.size()<<","<<pList[i].header[2].number<<","<<pList[i].header[3].number<<","<<pList[i].header[4].number<<","<<pList[i].header[5].number<<","<<pList[i].header[6].number<<","<<pList[i].header[7].number<<","<<endl;
			for(unsigned int j=0;j<pList[i].payload.size();j++){
				f<<j+1<<")["<<(int)pList[i].payload[j]<<"]"<<(bitset<8>)pList[i].payload[j]<<" ";
				if(j%13==0 && j!=0)
					f<<endl;
			}
			f<<endl;
			f<<"END OF PACKET with id "<<pList[i].segmentID<<endl;
		}
		f.close();
}
