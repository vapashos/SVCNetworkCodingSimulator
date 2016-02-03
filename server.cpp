/*
 * server.cpp
 *
 *  Created on: Feb 25, 2014
 *      Author: sportbilly
 */

#include "server.h"
#include <cstdlib>
#include <cstring>

server::server(int genSize,string bitStreamFile,string traceFile) {
	generationSize=genSize;
	loadTraceFile(trace,traceFile);
	streamRTPPacketization(bitstream,bitStreamFile);
	//Select type of segmentation
   //streamAVCPacketization(bitstream,bitStreamFile);
	segmentation();
	cout<<"after segmentation";
	writeSegmentationDataOnFile("segmentationData.txt");
	Node::showGlobalSegList();
	cout<<"packets Number counted "<<RTPpackets<<" and size of "<<packets.size()<<endl;

}

server::~server() {
	// TODO Auto-generated destructor stub
}

void server::loadTraceFile(ifstream &f,string fileName){/*This function opens the trace file and fills the packetMetaData table*/

	f.open(fileName.c_str(),ios::binary);
	RTPpackets=getLinesNumber(trace)-2;
	if(!f.good()){
		cout<<"failure opening trace file "<<endl;
		return;
	}
	int lineNumber=2;
	int currPos;/*Holds the current possition inside the file while parsing the file*/
	string line,temp;
	moveInsideFile(trace,2);
    while(getline(trace,line)){
    	lineNumber++;
        currPos=line.find_first_not_of(" ",10);
        deque<int> tempDeque;
        for(int i=0;i<4;i++){
        	tempDeque.push_back(atoi((temp.assign(strtok(&line[currPos]," "))).c_str()));
            currPos=line.find_first_not_of(" ",currPos+temp.length()+1);
        }
        tempDeque.push_back(lineNumber);/*the last is the line number*/
        packetMetaData.push_back(tempDeque);
        tempDeque.clear();
    }
    f.close();
}


void server::streamRTPPacketization(ifstream &bitStream,string fileName){

	if(packetMetaData.empty()){
		cout<<"There are no metadata of packets"<<endl;
		return;
	}

	bitStream.open(fileName.c_str(),ios::binary);
	if(!bitStream.good()){
		cout<<"error while opening bitstream file"<<endl;
		return;
	}

	char *p;
	int ratio,counter=0;
	char c;
	for(unsigned int i=0;i<packetMetaData.size();i++){
		p=new char[packetMetaData[i][0]];
		bitStream.read(p,packetMetaData[i][0]);
		cout<<"line"<<i+1<<" size "<<packetMetaData[i][0]<<endl;
		/*RTP packet size exceeds MTU then break it into more packets packets*/
		ratio=(packetMetaData[i][0]<=MTUSIZE) ? 1:(packetMetaData[i][0]/MTUSIZE)+1;
		int start=0,end=0;
		int temp[5];
		temp[0]=i+1;
		temp[1]=packetMetaData[i][1];
		temp[2]=packetMetaData[i][2];
		temp[3]=packetMetaData[i][3];
		temp[4]=packetMetaData[i][4];/*Line number as NALUnit-ID*/
		cout<<"create packet regarding line "<<i+1<<" of size"<<packetMetaData[i][0]<<" L"<<packetMetaData[i][1]<<"T "<<packetMetaData[i][2]<<" Q "<<packetMetaData[i][3]<<endl;
		packets.push_back(Packet(p,packetMetaData[i][0],temp,4));
		counter++;
		delete[] p;
	}
	cout<<"Total number of packets created "<<counter<<endl;
	cin>>c;
	bitStream.close();
}



void server::streamAVCPacketization(ifstream &bitStream,string fileName){

	bitStream.open(fileName.c_str(),std::ifstream::binary);
	bitStream.seekg(0,bitStream.end);
	int fileSize=bitStream.tellg();
	bitStream.seekg(0,bitStream.beg);
	deque<unsigned char> tempPayload;

	if(!bitStream.good()){
		cout<<"error while opening bitstream file"<<endl;
		bitStream.close();
		return;
	}

	char *p=new char[fileSize];
	bitStream.read(p,fileSize);

	for(int i=0;i<MTUSIZE*(fileSize/MTUSIZE);i++){
		tempPayload.push_back(p[i]);

		if(i%MTUSIZE==0 && i>0){
			packets.push_back(Packet(tempPayload,MTUSIZE));
			tempPayload.clear();
		}

		cout<<i<<")"<<(bitset<8>)p[i]<<endl;
	}
	tempPayload.clear();
	int remainBytes=fileSize-(MTUSIZE*(fileSize/MTUSIZE));
	cout<<"bytes remaining"<<remainBytes<<endl;
	for(int i=0;i<remainBytes;i++){
		tempPayload.push_back(p[i]);
	}
	packets.push_back(Packet(tempPayload,remainBytes));
	bitStream.close();


	cout<<"Number of packets created "<<packets.size();

}


void server::segmentation(){
	//We will use two types of segmentation of data one is that each
	//segment has the same number of packets of same payload size.
	//The other case is when we segment the file according to the
	// type of layer of each NAL U (0,0,0) (0,1,0)
	if(packets.empty()){
		cout<<"packetization process have to run first"<<endl;
		return;
	}
	//case 1 allocate the packets uniformlly on segments.All segments have the same number of packets
	int counter=0;
	for(unsigned int i=0;i<packets.size();i++){

		if(i%generationSize==0)
		{
			counter=0;
			if(!segments.empty())
				segments.back().showSegmentPackets();
			if(i>0)
				Node::globalSegList.push_back(&segments.back());

			segments.push_back(segment());
		}
		packets[i].setSegmentID(segments.back().getID(),counter);
		segments.back().addPacket(packets[i]);
		counter++;
	}
	Node::globalSegList.push_back(&segments.back());
	segments.back().showSegmentPackets();
	cout<<endl<<"segmentation finished num of segments="<<segments.size()<<endl;
}

void server::segmentation2(){
	//We will use two types of segmentation of data one is that each
	//segment has the same number of packets of same payload size.
	//The other case is when we segment the file according to the
	// type of layer of each NAL U (0,0,0) (0,1,0)

	char c;
	int counter2=0;
	bool flag;
	if(packets.empty()){
		cout<<"packetization process have to run first"<<endl;
		return;
	}
	//case 1 allocate the packets uniformlly on segments.All segments have the same number of packets
	int counter=0;
	for(unsigned int i=0;i<packets.size();i++){
		flag=false;
		if(i>0){
			for(unsigned int j=3;j<6;j++){
				if(packets[i].header[j].number!=packets[i-1].header[j].number){
					segments.push_back(segment());
					flag=true;
					counter=0;
					counter2=0;
					Node::globalSegList.push_back(&segments.back());
					cout<<"new packet has different parameters";
					break;
				}
			}
		}

		if(counter2%generationSize==0 && !flag){
			counter=0;
			counter2=0;
			segments.push_back(segment());
			Node::globalSegList.push_back(&segments.back());
		}
		cout<<"LID="<<packets[i].header[3].number<<"TID="<<packets[i].header[4].number<<"QID="<<packets[i].header[5].number<<" ";
		packets[i].setSegmentID(segments.back().getID(),counter);
		segments.back().addPacket(packets[i]);
		counter++;
		counter2++;
	}
	segments.back().showSegmentPackets();
	cout<<endl<<"segmentation finished num of segments="<<segments.size()<<endl;
}



void server::showSegmentLastPacketIDs(){

	for(unsigned int i=0;i<segments.size();i++){
		cout<<"segment ID:"<<segments[i].id<<" last packetID"<<segments[i].getSegmentLastPacketID()<<endl;;
	}
}

/*Help functions*/

void server::checkPayloadSize(){
	char c;
	for(unsigned int i=0;i<packets.size();i++){
		if(packets[i].payload.size()!=(unsigned int)MTUSIZE){
			cout<<"Error in file size packet with id "<<packets[i].id<<" h"<<packets[i].payload.size()/*header[1]*/<<endl;
			cin>>c;
			break;
		}
	}
}


void server::createCodedPacket(const segment &s){
	int r;
	r=randomNumber(0,256);
	Packet Temp=r*s.packets.front();
	for(unsigned int i=1;i<s.packets.size();i++){
			/*Pick up a random number*/
		r=randomNumber(0,256);
		Temp=Temp+r*s.packets[i];
	}
	Temp.id=Packet::counter++;
}

/*Write on File Functions*/
void server::writeSegmentationDataOnFile(string filename){
	ofstream f(filename.c_str());
	if(segments.empty()){
		f<<"file empty"<<endl;
		f.close();
	}
	for(unsigned int i=0;i<segments.size();i++){
		f<<"-------------------------------  segment"<<segments[i].getID()<<" packets size"<<segments[i].packets.size()<<"---------------------------------------"<<endl<<endl;
		for(unsigned int j=0;j<segments[i].packets.size();j++){
			f<<"** packet"<<segments[i].packets[j].id<<" segmentID="<<segments[i].packets[j].segmentID
					<<" indexInSeg="<<segments[i].packets[j].header[0].number
					<<" sizeBytes="<<segments[i].packets[j].payload.size()//header[1].number
					<<" RTPPackID="<<segments[i].packets[j].header[2].number
					<<" lid="<<segments[i].packets[j].header[3].number
					<<" tid="<<segments[i].packets[j].header[4].number
					<<" gid="<<segments[i].packets[j].header[5].number
					<<" NALU-id="<<segments[i].packets[j].header[6].number
					<<" NALU-id Multiplier="<<segments[i].packets[j].header[7].number
					<<" NALU-id="<<segments[i].packets[j].header[6].number+255*segments[i].packets[j].header[7].number<<endl;
			for(unsigned int z=0;z<segments[i].packets[j].payload.size();z++){
				f<<(z+1)<<") ["<<(int)segments[i].packets[j].payload[z]<<"]"<<(bitset<8>) segments[i].packets[j].payload[z]<<" ";
				if(z%13==0 && z!=0)
					f<<endl;
			}
			f<<endl;
			f<<"****************END OF packet"<<segments[i].packets[j].id<<" ***************************"<<endl;
		}
		f<<"--------------------- END OF segment"<<segments[i].getID()<<" ---------------------------------------"<<endl<<endl;
	}
	f.close();

}



