/*
 * Node.cpp
 *
 *  Created on: Feb 17, 2014
 *      Author: vapashos
 */

#include "Node.h"
#include <algorithm>
#include <sstream>
int Node::counter=0;
bool Node::useWlan=true;
float wlanProbability=0.2;
channelWlan Node::wChannel=channelWlan(wlanProbability);
deque<segment*> Node::globalSegList;
ofstream Node::fl("codedPackets.txt");
Node::Node() {
	counter++;
	id=counter;
	index=0;
	cout<<"Create node with id "<<id<<endl;
	initiateDecoder();/*Initialize decoder assign myDecoder pointer on decoder of finite field 2^8*/
	/*initialize all pointer to NULL*/
	decodedData.clear();
	bdSegment=NULL;


}

Node::Node(const Node &n)
{
	id=n.id;
	inQueue=n.inQueue;
	outQueue=n.outQueue;
	index=n.index;
	neighborList=n.neighborList;
	myDecoder=n.myDecoder;

}

Node::~Node() {
	// TODO Auto-generated destructor stub
}

/*functions */
void Node::addNeighbor(Node *x)
{
	if(x->id!=id && find(neighborList.begin(),neighborList.end(),x)==neighborList.end()){
		neighborList.push_back(x);
	}
}


void Node::sendPacket(int timeSlot,int receiverID){

	cout<<"Node "<<id<<" sendPacket:";
	if(outQueue.empty()){
		cout<<"There is no packet to be send"<<endl;
		return;
	}
	cout<<"outQueue size"<<outQueue.size()<<endl;
	Packet Temp=outQueue.front();

	if(randomNumber(10)>Node::wChannel.lossProb){
		Temp.timeSend=timeSlot;
		Temp.setReceiverID(receiverID);
		Temp.setSenderID(id);
		Node::wChannel.buffer.push_back(Temp);
	}
	else{
		cout<<"Failed to send packet "<<Temp.id<<endl;
	}
	outQueue.pop_front();
}


void Node::recvPacket(int timeSlot){

	cout<<"Node"<<id<<" recvPacket:";

	if(Node::wChannel.buffer.empty() ){
		cout<<"there are no packets to download"<<endl;
		return;
	}

	segment *s;
	channelWlan *q=&Node::wChannel;
	cout<<"node "<<id<<" inside RECVPacket "<<q->buffer.size();
	for(deque<Packet>::iterator it=q->buffer.begin();it!=q->buffer.end();it++){
		if(it->receiverID==id){
			cout<<"found packet for me";
			if(it->timeSend<timeSlot){
				Packet myPacket=*it;
				s=getSegment(myPacket.segmentID);
				/*check if the packet to receive is a coded packet that belongs to a previously decoded segment*/
				/*This is the point i have to check if the packet received has coefficient vector that is
				 * linear independent to the vectors allready received*/

				/*Create a deque with the coefficients of packet to be received this will be used as
				 * input to the checkLinearDependencyForVector function*/
				deque <ffNumber > coeffVectorAsffNumber;
				cout<<"coefficient vector to examine linear dependency"<<endl;
				for(unsigned int i=0;i<myPacket.coefVector.size();i++){
					coeffVectorAsffNumber.push_back(ffNumber((unsigned int)myPacket.coefVector[i],ff));
					cout<<coeffVectorAsffNumber[i].number<<" ";
				}
				cout<<endl;

				/*Line replaced to make the extra check about new vectors linear independency*/
				/*if(decodedSegments[s]==false || decodedSegments.find(s)==decodedSegments.end()){*/
				if((decodedSegments[s]==false || decodedSegments.find(s)==decodedSegments.end()) && checkLinearDependencyForVector(coefficientMatrix[s],coeffVectorAsffNumber,coefficientMatrixNoGauss[s],s->packets.size())){
					cout<<"it is a packet that doesn't belong to previously decoded segment"
							"and i will receive it right now"<<endl;
					if(segInProgress[s]++==1){/*Update segInProgress mapping <segment,counter of packets that belong in segment>*/
						decodedSegments[s]=false;
					}
					addCoefVector(myPacket.coefVector,s);//Add coefficient vector on local table in node if result is not null start decoding
					inQueue.push_back(myPacket);
					cout<<"Add packet "<<myPacket.id<<" in my inQueue"<<endl;
				}
				else{
					cout<<"i was previously decoded that segment i don't need that packet i'll drop it"<<endl;
				}
				q->buffer.erase(it);
				return;
			}
			cout<<"but i will receive it next timeslot"<<endl;
		}
	}

	cout<<"there was no packet destined for me"<<endl;
}


void Node::recv3GPacket(Packet &p,int timeslot){

	cout<<"Node"<<id<<" recv3GPacket:";
	queue3G.push_back(p);
	char c;
	if(queue3G.back().isLast()){
		cout<<"!!!!!!!!!!  user"<<id<<" finished downloading segment "<<p.getSegmentID()<<" on timeslot "<<timeslot+1<<endl;
		cout<<" now create list of coded packets for sending "<<endl;
		ofstream fz("3GDownloads.txt",fstream::app);

		deque<Packet> temp;
		for(unsigned int i=index;i<queue3G.size();i++){
			temp.push_back(queue3G[i]);
		}

		/*Create new segment and add it on corresponding queue*/
		int segmentID=queue3G.back().segmentID;

		fz<<"node "<<id<<" downloaded successfully segment "<<segmentID<<" on timeslot "<<timeslot<<endl;
		fz.close();
		segQueue.push_back(segment(segmentID,temp));
		/*Update index variable*/
		cout<<index<<endl;
		index=queue3G.size();
		cout<<index<<"zzzzzzzzzzzzzzzzz "<<queue3G.size()<<" and segment id "<<segmentID<<endl;
		/*After that the user will make initial push of packets on a user
		 * but all the transmissions are pseudo broadcast so it will broadcast
		 * the segment*/
		/*Create coded packets equal to segment's size and add them to outgoing queue*/
		for(unsigned int i=0;i<segQueue.back().packets.size();i++){
			outQueue.push_back(createCodedPacket(segQueue.back(),timeslot));
		}
	}
}


bool Node::broadcastPacket(int timeSlot,Node *brNode){

	bool broadcastFlag=true;
	cout<<"Node"<<id<<" broadcastPacket:";
	if(outQueue.empty()){
		cout<<"Node "<<id<<" has nothing to broadcast"<<endl;
		return broadcastFlag;
	}

	if(brNode==NULL){
		cout<<"broadcast node is not been set yet"<<endl;
		return broadcastFlag;
	}

	if(this!=brNode){
		cout<<"i cannot transmit the channel is in use by node "<<brNode->id<<endl;
		return broadcastFlag;
	}
	int segmentID;
	Packet Temp=outQueue.front();
	segmentID=Temp.getSegmentID();/*This is needed when its the time to create the advertisment*/

	//if(bdSegment!=)


	if(timeSlot>Temp.timeCreated){
		for(unsigned int i=0;i<neighborList.size();i++){
			if(randomNumber(10)>Node::wChannel.lossProb){
				Temp.timeSend=timeSlot;
				Temp.setReceiverID(neighborList[i]->id);
				Temp.setSenderID(id);
				Node::wChannel.buffer.push_back(Temp);
			}
		}
		broadcastFlag=false;
		outQueue.pop_front();
		//if(outQueue.empty() || outQueue.back().segmentID!=bdSegment->id){
		/*Αλλαγή γιατι κάποιος κόμβος μπορει να παραχωρήσει τη σειρά του σε άλλον αν εκεινη τη στιγμή που κάνει
		 * broadcast τύχει και κατεβάσει segment και δημιουργήσει καινούρια coded packets*/
		cout<<"outQueue empty"<<outQueue.empty()<<" outQueue.front.segmentID"<<outQueue.front().segmentID<<endl;//<<" bdSegmentID"<<bdSegment->id<<endl;
		if(bdSegment==NULL)
		{
			cout<<"SEGMENT IS NULL"<<endl;
		}
		if(outQueue.empty() || outQueue.front().segmentID!=bdSegment->id){
			cout<<"Finished sending packets i'll release the flag"<<endl;
			broadcastFlag=true;
			/*After finishing initial push create advertisment one for each of the neighbors*/
			createAdvertisment(segmentID,timeSlot);
		}
	}
	return broadcastFlag;
}


void Node::sendMessage(int timeSlot){

	cout<<"Node"<<id<<" sendMessage:";

	if(outMsgQueue.empty()){
		cout<<"Node"<<id<<" outMsgQueue is empty"<<endl;
		return;
	}
	message *Temp;
	if(timeSlot>outMsgQueue.front()->timeCreated){
		if(outMsgQueue.front()->isAdvertisment()){
			cout<<"This is an Advertisment"<<endl;
			for(unsigned int i=0;i<neighborList.size();i++){
				if(randomNumber(10)>Node::wChannel.lossProb){
					Temp=new advertisment(outMsgQueue.front(),timeSlot);
					Temp->timeSend=timeSlot;
					Temp->receiverID=neighborList[i]->id;
					Node::wChannel.msgBuffer.push_back(Temp);
				}
				else{
					cout<<"Failed to send message "<<Temp->id<<endl;

				}
				cout<<"MessageQueue after advertisment send "<<endl;
			}
		}
		else{
			Temp=new request(outMsgQueue.front(),timeSlot);
			Temp->timeSend=timeSlot;
			Node::wChannel.msgBuffer.push_back(Temp);
		}
		outMsgQueue.pop_front();
	}
	else{
		cout<<"i will send message next timeslot"<<endl;
	}
}

void Node::recvMessage(int timeSlot){

	cout<<"Node"<<id<<" recvMessage:";

	if(Node::wChannel.msgBuffer.empty() ){
		cout<<"empty global message list"<<endl;
		return;
	}

		channelWlan *q=&Node::wChannel;
		cout<<"node "<<id<<" inside RECVMessage "<<q->msgBuffer.size()<<endl;
		deque<message*>::iterator last;
		last=q->msgBuffer.end();
		last--;
		for(deque<message*>::iterator it=q->msgBuffer.begin();it!=q->msgBuffer.end();it++){
			cout<<"(*it)->receiverID"<<(*it)->receiverID<<endl;
			if((*it)->receiverID==id){
				if((*it)->timeSend<timeSlot){
					message *myMsg=*it;
					/*Ο κόμβος απο τη στιγμή που παραλάβει ένα message δεν το τοποθετεί
					 * στην inMsgQueue παίρνει το advertisment και αμέσως δημιουργεί
					 * request προσθέτοντας το στην outMsgQueue έχοντας υπολογίσει τον
					 * αριθμό των πακέτων που χρειάζεται*/
					cout<<"message is "<<myMsg->getMessageType()<<endl;
					if(myMsg->isAdvertisment()){
						/*Advertisment Case*/
						/*Υπολογισμός Πακέτων που λείπουν για να συμπληρώσει για την κωδικοποίηση του segment*/
						int packetsToRequest=myMsg->segmentPtr->getSize()-segInProgress[myMsg->segmentPtr];
						if(packetsToRequest>0){/*Create request message only if packetsToRequest is more than 0*/
							cout<<"create request after accepting the advertisment,Request is for "<<packetsToRequest<<" packets"<<endl;
							cout<<"before creating request"<<endl;
							createRequest(myMsg->senderID,myMsg->segmentID,packetsToRequest,timeSlot);
							cout<<"getType message"<<outMsgQueue.back()->getMessageType()<<endl;
						}
					}
					else{
						/*Request case*/
						cout<<"found request for segment "<<myMsg->segmentPtr->id<<"dimensions required:"<<myMsg->getDimensions()<<endl;
						inMsgQueue.push_back(myMsg);
					}

					cout<<"Add message "<<myMsg->id<<"with receiver id"<< myMsg->receiverID<<" in my inQueue"<<endl;
					if(q->msgBuffer.size()==1){
						q->msgBuffer.clear();
						break;
					}
					else if(it==last){
						q->msgBuffer.pop_back();
						break;
					}
					else{
						q->msgBuffer.erase(it);
					}
				}
				cout<<"timeslot:"<<timeSlot<<"wait for next timeslot to get the message"<<endl;
			}
		}
}


bool Node::isEmpty(const deque<Packet> &queue){
	return queue.empty();
}

void Node::createRequest(int receiverID,int segID,int dimensions,int timeSlot){
	cout<<"Node"<<id<<" createRequest:";
	outMsgQueue.push_back(new request(getSegment(segID),id,receiverID,dimensions,timeSlot));
	cout<<"After creating request check Type "<<outMsgQueue.back()->isRequest()<<"request receiver "<<outMsgQueue.back()->getDimensions()<<endl;

}

void Node::createAdvertisment(int segID,int timeSlot){

	cout<<"Node"<<id<<" createAdvertisment:";
	cout<<"inside create advertisment "<<segID<<endl;
	outMsgQueue.push_back(new advertisment(getSegment(segID),id,0,timeSlot));

}


void Node::addSegmentToDownloadList(segment *x){
	cout<<"Node"<<id<<" addSegmentToDownloadList:";
	cout<<"Node "<<id<<" add to toDownloadList segment with id "<<x->id<<endl;
	segToDownloadList[x]=false;
}

void Node::checkSegmentDownload(){

	map<segment*,bool>::iterator i;
	for(i=segToDownloadList.begin();i!=segToDownloadList.end();i++){

	}
}


Packet Node::createCodedPacket(const segment &s,int timeSlot) const{


	deque <unsigned char > r;
	r.push_back(randomNumber(1,256));//Avoid to have the first coefficient of coefficient vector equal to zero
	Packet Temp=r.back()*s.packets.front();
	for(unsigned int i=1;i<s.packets.size();i++){
	/*Pick up a random number*/
		r.push_back(randomNumber(0,256));
		Temp=Temp+r.back()*s.packets[i];
	}
	Temp.setCoefVector(r);
	Temp.id=Packet::counter++;
	Temp.setTimeCreated(timeSlot);
	/*Write Data on file for Tracing purposes*/

	fl<<"Coded Packet of segment"<<s.id<<" coef vector =[";
	for(unsigned int i=0;i<r.size();i++){
		fl<<(int)r[i]<<" ";
	}
	fl<<"]"<<endl;
	fl<<"------------   Packet id"<<Temp.id<<" --------"<<endl;
	fl<<"Header Info segID"<<Temp.segmentID;
	fl<<" indx="<<Temp.header[0].number;
	fl<<" size="<<Temp.payload.size();//header[1].number;
	fl<<" RTPid="<<Temp.header[2].number;
	fl<<" lid="<<Temp.header[3].number;
	fl<<" tid="<<Temp.header[4].number;
	fl<<" qid="<<Temp.header[5].number<<endl;
	fl<<" NALUid="<<Temp.header[6].number<<endl;
	fl<<" NALUidMult="<<Temp.header[7].number<<endl;
	for(unsigned int i=0;i<Temp.payload.size();i++){
		fl<<i+1<<")["<<(int)Temp.payload[i]<<"]"<<(bitset<8>)Temp.payload[i]<<" ";
	}
	fl<<endl;


	return Temp;

}


void Node::createPoolOfCodedPackets(segment *s,int num,int timeSlot) {/*creates a number of new coded packets based on segment packets are added directly on node's output queue using function createCodedPacket*/


	//Added code on 04-12-2015
	//sometimes node may allready have on its outqueue some coded packets of the corresponding segment s
	//in this case we have to check if the number of coded packets requested for this segment is bigger
	//than the coded packets for this segment that node already has in its outputqueue.
	//loop over the output queue
	int counter=0;//counter of coded packets that belong on segment s and are allready in the output queue.
	deque<Packet>::iterator it=outQueue.begin();
	deque<Packet>::iterator temp;
	cout<<"Node"<<id<<"Prior to created coded packets for segment"<<s->id<<" i will erase coded packets for the corresponding segment"<<endl;

	if(!outQueue.empty()){
		while(it!=outQueue.end()){



			if(it->segmentID==s->id){
				temp=it;
				if(outQueue.size()>1)
					it++;
				cout<<"erase "<<temp->id<<" ";
				outQueue.erase(temp);
			}
			else{
				it++;
			}
		}
	}
	cout<<endl;

	for(int i=0;i<num;i++){
		//outQueue.push_back(createCodedPacket(*s,timeSlot));
		//I will put the packets that are created as recovery packet of packet losses in front of outQueue
		outQueue.push_front(createCodedPacket(*s,timeSlot));
	}
}


void Node::checkIncomingCodedPackets(int timeSlot){
	/*this function is called whenever an advertisment is received*/
	cout<<"Node"<<id<<" checkIncomingCodedPackets:";
	if(segInProgress.empty()){
		cout<<"There is no segment in progress to download"<<endl;
		return;
	}
	char c;
	for(map<segment*,int>::iterator it=segInProgress.begin();it!=segInProgress.end();it++){
		/*If packets of segment downloaded equals to segment size and decoding of the segment never happened(false)*/
		/*i can start decoding*/

		deque<deque<Packet>::iterator> itTable;
		if(it->second==it->first->size && !decodedSegments[it->first]){
			cout<<"Node "<<id<<" i'm going to start decoding!!!!!!!!!!!!!!!!!"<<endl;

			myDecoder->setMatrix(coefTable[it->first]);
		/*Use Iterator case*/
			bool flag;
			cout<<"before start decoding size of inQueue "<<inQueue.size()<<endl;
			for(deque<Packet>::iterator z=inQueue.begin();z!=inQueue.end();z++){
				if(z->getSegmentID()==it->first->getID()){
					decodedMapping[it->first].push_back(*z);
					if(inQueue.size()==1 || z->id==inQueue.back().id){
						cout<<"before erase "<<z->id<<"inQueue.size()="<<inQueue.size()<<" z->id="<<z->id<<endl;
						cout<<"packet with id "<<decodedMapping[it->first].back().id<<" added corresponting to segment "<<decodedMapping[it->first].back().getSegmentID()<<endl;
						inQueue.pop_back();
						break;
					}
					else{
						cout<<"before erase "<<z->id<<endl;
						if(z->id!=inQueue.front().id)
							flag=true;
						inQueue.erase(z);

					}
					cout<<"packet with id "<<decodedMapping[it->first].back().id<<" added corresponting to segment "<<decodedMapping[it->first].back().getSegmentID()<<endl;
					if(flag){
						z--;
						flag=false;
					}
				}
			}
			/*After exiting for loop tempPacketList will contain only coded packets
			 * that belong to the segment that is going to be decoded*/
			stringstream ss;
			ss << id;
			string str = ss.str();
			string filename="Node"+str+"decSeg.txt";
			/*Write Data on file before decoding operation*/
			decodedMapping[it->first].back().showPayload(befDec);/*print payload of packet inside file*/
			/*################## Decode Packet ###########################*/
			myDecoder->decodePackets(decodedMapping[it->first]);/*Decoding Operation is going to take place*/
			/*################## End of Packet Decoding  #################*/
			/*Write Data on file after decoding operation*/
			decodedMapping[it->first].back().showPayload(afterDec);/*print payload of packet inside file*/
			/*Check the segment as decoded*/
			decodedSegments[it->first]=true;/*Check the segment as decoded*/
			ds.open(filename.c_str(),ios::out | ios::app);
			ds<<"segment"<<it->first->id<<"inside packet "<<decodedMapping[it->first].back().header[0].number<<" Nalunit"<<decodedMapping[it->first].back().header[6].number+255*decodedMapping[it->first].back().header[7].number<<" timeSlot:"<<timeSlot<<endl;
			/*Write in file data of segment that has been decoded*/
			ds.close();
			break;
		}
	}
}

segment* Node::addCoefVector(deque<unsigned char> &c,segment *segID){

	/*Create mapping with segmentID - 2 dimensional Array of coefficients*/
	for(map<segment*,deque<deque<unsigned char> > >::iterator it=coefTable.begin();it!=coefTable.end();it++){
		if(it->first==segID){
			it->second.push_back(c);
			if(segID->getSize()==(int)it->second.size())
				return segID;
			else
				return NULL;
		}

	}
	deque<deque <unsigned char> > tempDeque;
	tempDeque.push_back(c);
	coefTable[segID]=tempDeque;
	return NULL;
}


segment * Node::getSegment(int segID){
	for(unsigned int i=0;i<globalSegList.size();i++){
		if(globalSegList[i]->id==segID){
			return globalSegList[i];
		}
	}
	return NULL;
}


void Node::initiateDecoder(){

	myDecoder=new decoder(ff);
}

int Node::checkRequests(){
/*With this function node checks its requests inside inMsgQueue and
 * returns , determines the maximum dimension to send*/
	cout<<"Node"<<id<<" checkRequests:";
	if(inMsgQueue.empty()){
		cout<<"There is no incoming Message";
		return -1;
	}
	int maxDim=0;

	if(inMsgQueue.front()->isAdvertisment()){
		cout<<"I have to handle advertisment reception "<<endl;
		return -1;
	}
	segment* currSeg=NULL;
	for(deque<message *>::iterator it=inMsgQueue.begin();it!=inMsgQueue.end();it++){
			if((*it)->isRequest()){
				currSeg=(*it)->segmentPtr;
				break;
			}
	}

	for(deque<message *>::iterator it=inMsgQueue.begin();it!=inMsgQueue.end();it++){
		if((*it)->isAdvertisment() || currSeg==NULL)
			break;
		if(currSeg!=(*it)->segmentPtr)
			break;

		if(maxDim<(*it)->getDimensions())
			maxDim=(*it)->getDimensions();

		if(inMsgQueue.size()==1){
			inMsgQueue.clear();
			break;
		}
		else
			inMsgQueue.erase(it);

	}
	return maxDim;
}


void Node::composeVideoFile(){

	int segmentFrom3GId=0;
	stringstream ss;
	ss << id;
	string str = "videoNode"+ss.str()+".bin";
	string str2=pathToStore+"videoNodeNoZero"+ss.str()+".264";
	string str3= "videoNode3G"+ss.str()+".bin";
	string str4= "videoNodeDecSeg"+ss.str()+".bin"; //string for video file ignoring bytes from non decoded segments
	videoFile.open(str.c_str(),ios::out | ios::binary);
	videoFileNoZero.open(str2.c_str(),ios::out | ios::binary);
	videoFile3G.open(str3.c_str(),ios::out | ios::binary);
	videoFileDecSeg.open(str4.c_str(),ios::out | ios::binary);
	map<segment*,deque<Packet> > ::iterator it;
	map<segment*,int >:: iterator it2;

	cout<<" ===================       COMPOSE VIDEO FILE NODE "<<id<<" ====================="<<endl;
	cout<<"   segments used"<<endl;
	str="composeFileData"+ss.str()+".txt";
	ofstream composeFileData(str.c_str());
	str="videoFileDataTxt"+ss.str()+".txt";
	ofstream videoFileTxtData(str.c_str());

	composeFileData<<"segments downloaded from 3G: "<<segQueue.size()<<"globalSegList size= "<<Node::globalSegList.size()<<endl;
	videoFileTxtData<<"segments and data on packets of each segment"<<endl;
	for(int i=0;i<segQueue.size();i++){
		composeFileData<<"seg"<<segQueue[i].id;
	}
	composeFileData<<endl<<"========================"<<endl;
	videoFileTxtData<<endl<<"========================"<<endl;


	for(it=decodedMapping.begin();	it!=decodedMapping.end();it++){
			composeFileData<<"seg"<<it->first->id<<" ";
	}
	composeFileData<<endl;
	for(it=decodedMapping.begin();	it!=decodedMapping.end();it++){
			composeFileData<<"("<<it->first->getSize()<<") ";
	}
	composeFileData<<"segments in Progress list"<<endl;
	for(it2=segInProgress.begin();	it2!=segInProgress.end();it2++){
		composeFileData<<"seg"<<it2->first->id<<" ";
	}
	composeFileData<<endl;
	for(it2=segInProgress.begin();	it2!=segInProgress.end();it2++){
		composeFileData<<"("<<it2->second<<") ";
	}
	composeFileData<<endl<<"========================"<<endl;
	int dataSize=0;
	int i=0;
	for(it2=segInProgress.begin();it2!=segInProgress.end();){

		if(i<segQueue.size() && (it2->first->id)>segQueue[i].id){
			//Select packets from segments that were received from 3G
			composeFileData<<"3G downloaded segment "<<segQueue[i].id<<endl;
			videoFileTxtData<<"3G downloaded segment "<<segQueue[i].id<<endl;
			cout<<"#3Gseg"<<segQueue[i].id<<" #";
			for(unsigned int j=0;j<segQueue[i].packets.size();j++){
				videoFileTxtData<<"segment"<<segQueue[i].packets[j].segmentID<<"packet index inside segment"<<segQueue[i].packets[j].header[0].number+1<<" NAUID"<<segQueue[i].packets[j].header[6].number+255*segQueue[i].packets[j].header[7].number<<endl;
				dataSize+=segQueue[i].packets[j].payload.size();//header[1].number;
				for( int p=0;p<segQueue[i].packets[j].payload.size()/*header[1].number*/;p++){
					videoFile<<segQueue[i].packets[j].payload[p];
					videoFile3G<<segQueue[i].packets[j].payload[p];
					videoFileNoZero<<segQueue[i].packets[j].payload[p];
					videoFileTxtData<<"p"<<p<<")"<<bitset<8>(segQueue[i].packets[j].payload[p])<<" ";
				}
				videoFileTxtData<<endl<<"dataSize so far"<<dataSize<<endl;
			}
			i++;
		}
		else{

			videoFileTxtData<<"***********************************************"<<endl;
			videoFileTxtData<<" WLAN downloaded segment "<<(it2->first)->id<<it2->second<<")"<<endl;
			/*Data that has to do with actual payload are in decodedMapping map so we will search there*/
			//first of all get the apropriate iterator
			it=decodedMapping.find(it2->first);
			if(it!=decodedMapping.end()){//this means that the segment is found inside decoded segments list
				//Select packets from segments received with network coding through transmittions on WLAN
				composeFileData<<"WLAN downloaded segment "<<(it2->first)->id<<"("<<it2->second<<")"<<endl;
				cout<<"WLANseg"<<it->first->id<<" ";
				for(unsigned int k =0;k<(it->second).size();k++){
					videoFileTxtData<<endl<<"----------------------------"<<endl;
					videoFileTxtData<<"segment"<<(it->second)[k].segmentID<<"packet index inside segment"<<(it->second)[k].header[0].number<<" NAUID"<<(it->second)[k].header[6].number+255*(it->second)[k].header[7].number<<endl;
					videoFileTxtData<<endl<<"----------------------------"<<endl;
					dataSize+=(it->second)[k].payload.size();//header[1].number;
					for(int z=0;z<(it->second)[k].payload.size()/*header[1].number*/;z++){
						videoFile<<(it->second)[k].payload[z];
						videoFileNoZero<<(it->second)[k].payload[z];
						videoFileTxtData<<z<<")"<<(bitset<8>)((it->second)[k].payload[z])<<" ";
					}
				}
			}
			else{
				//Access globalsegment list to get information about the size of each packet that belongs to the segment that is not decoded
				for(int k=0;k<Node::globalSegList[it2->first->id-1]->packets.size();k++){
					videoFileTxtData<<"---------packet "<<k+1<<"-----------------"<<endl;
					for(unsigned int w=0;w<Node::globalSegList[it2->first->id-1]->packets[k].payload.size()/*header[1].number*/;w++){
						videoFile<<0x00;
						videoFileTxtData<<w<<")"<<(bitset<8>)0x00<<" ";
					}
				}
				videoFileTxtData<<" segment "<<it2->first->id<<" is not decoded"<<endl;
			}
			videoFileTxtData<<"dataSize:"<<dataSize<<"****************************************"<<endl;
			it2++;
		}
	}
	if(i<segQueue.size()){
		//Select packets from segments that were received from 3G probably remaining packets.
		composeFileData<<"3G downloaded segment "<<segQueue[i].id<<endl;
		videoFileTxtData<<"3G downloaded segment "<<segQueue[i].id<<endl;
		cout<<"!3GGseg"<<segQueue[i].id<<" !";
		for(unsigned int j=0;j<segQueue[i].packets.size();j++){
			videoFileTxtData<<endl<<"----------------------------"<<endl;
			videoFileTxtData<<"packet id inside segment"<<segQueue[i].packets[j].header[0].number<<" NAUID"<<segQueue[i].packets[j].header[6].number+255*segQueue[i].packets[j].header[7].number<<endl;
			videoFileTxtData<<endl<<"----------------------------"<<endl;
			for(int p=0;p<segQueue[i].packets[j].payload.size();p++){
				videoFile<<segQueue[i].packets[j].payload[p];
				videoFileNoZero<<segQueue[i].packets[j].payload[p];
				videoFile3G<<segQueue[i].packets[j].payload[p];
				videoFileTxtData<<"p"<<p<<")"<<bitset<8>(segQueue[i].packets[j].payload[p])<<" ";
			}
		}
		i++;
	}

	videoFile.close();
	videoFileNoZero.close();
	videoFile3G.close();
	composeFileData.close();
	cout<<endl<<"==================================================================="<<endl;
	cout.flush();

}



void Node::generateTraceFile(){

	stringstream ss;
	ss << id;
	string str = "Node"+ss.str()+"TraceFile.txt";
	traceFile.open(str.c_str());
	cout<<"Node"<<id<<" inside generate traceFile"<<endl;
	traceFile<<"Start-Pos.  Length  LId  TId  QId   Packet-Type  Discardable  Truncatable"<<endl;
	traceFile<<"==========  ======  ===  ===  ===  ============  ===========  ==========="<<endl;
	cout<<"Start-Pos.  Length  LId  TId  QId   Packet-Type  Discardable  Truncatable"<<endl;
	cout<<"==========  ======  ===  ===  ===  ============  ===========  ==========="<<endl;
	map<segment*,deque<Packet> > ::iterator it;
	it=decodedMapping.begin();
	int segmentFrom3GId=0,totalBytesCounted=0,previouTotalBytes=0,currentNALUnit=3;
	for(int i=0;i<Node::globalSegList.size();i++){
		if((i+1)%5==id && segmentFrom3GId<segQueue.size()-1)/*5 is the number of nodes*/
		{
			cout<<"========== segment "<<Node::globalSegList[i]->id<<"========="<<endl;
			/*This is the case where the segment is downloaded from 3G so packets
			 * * belong to different queue*/
			//
			//cout<<int2Hex(totalBytesCounted);
			for(unsigned int j=0;j<segQueue[segmentFrom3GId].packets.size();j++){
				if(currentNALUnit!=segQueue[segmentFrom3GId].packets[j].header[6].number+255*segQueue[segmentFrom3GId].packets[j].header[7].number){
					traceFile<<int2Hex(previouTotalBytes)<<" ";
					traceFile<<"NALUnit"<<segQueue[segmentFrom3GId].packets[j].header[6].number+255*segQueue[segmentFrom3GId].packets[j].header[7].number<<"      "<<totalBytesCounted<<"    ";
					traceFile<<segQueue[segmentFrom3GId].packets[j].header[3].number<<"    ";
					traceFile<<segQueue[segmentFrom3GId].packets[j].header[4].number<<"    ";
					traceFile<<segQueue[segmentFrom3GId].packets[j].header[5].number<<" ";
					if((segQueue[segmentFrom3GId].packets[j].header[6].number+(255*segQueue[segmentFrom3GId].packets[j].header[7].number))-1==3){
						traceFile<<"StreamHeader          No          No";
					}
					else if((segQueue[segmentFrom3GId].packets[j].header[6].number+255*segQueue[segmentFrom3GId].packets[j].header[7].number)-1<6 && (segQueue[segmentFrom3GId].packets[j].header[6].number*255+segQueue[segmentFrom3GId].packets[j].header[7].number)-1>3){
						traceFile<<"ParameterSet          No          No";
					}
					else{
						traceFile<<"SliceData                           ";
					}
					traceFile<<endl;

					previouTotalBytes=totalBytesCounted;
					cout<<" packetDI "<<segQueue[segmentFrom3GId].packets[j].id;
					cout<<" NALUnit "<<segQueue[segmentFrom3GId].packets[j].header[6].number+255*segQueue[segmentFrom3GId].packets[j].header[7].number;
									cout<<" LID "<<segQueue[segmentFrom3GId].packets[j].header[3].number;
									cout<<" TID "<<segQueue[segmentFrom3GId].packets[j].header[4].number;
									cout<<" QID "<<segQueue[segmentFrom3GId].packets[j].header[5].number;
									cout<<" totalBytes "<<previouTotalBytes;
									cout<<" segmentID "<<segQueue[segmentFrom3GId].packets[j].header[0];
									cout<<" sizeinBytes "<<segQueue[segmentFrom3GId].packets[j].payload.size();//.header[1];
									cout<<" RTP id "<<segQueue[segmentFrom3GId].packets[j].header[2]<<endl;
					totalBytesCounted=0;
					currentNALUnit=segQueue[segmentFrom3GId].packets[j].header[6].number+255*(segQueue[segmentFrom3GId].packets[j].header[7].number);
				}

				totalBytesCounted+=segQueue[segmentFrom3GId].packets[j].payload.size();//header[1].number;

			}
			segmentFrom3GId++;
		}
		else{
			cout<<"========== segment "<<it->first->id<<"========="<<endl;
			for(unsigned int k =0;k<(it->second).size()-1;k++){
				if(currentNALUnit!=(it->second)[k].header[6].number+255*((it->second)[k].header[7].number)){

					traceFile<<int2Hex(previouTotalBytes);
					traceFile<<"NALUnit"<<(it->second)[k].header[6].number<<"      "<<totalBytesCounted<<"    ";
					traceFile<<(it->second)[k].header[3].number<<"    ";
					traceFile<<(it->second)[k].header[4].number<<"    ";
					traceFile<<(it->second)[k].header[5].number<<" ";
					if(((it->second)[k].header[6].number+(255*(it->second)[k].header[7].number))-1==3){
						traceFile<<"StreamHeader          No          No";
					}
					else if(((it->second)[k].header[6].number+(255*(it->second)[k].header[7].number))-1<6 && ((it->second)[k].header[6].number+(255*(it->second)[k].header[7].number))-1>3){
						traceFile<<"ParameterSet          No          No";
					}
					else{
						traceFile<<"SliceData                           ";
					}
					traceFile<<endl;
					previouTotalBytes=totalBytesCounted;
					cout<<" packetDI "<<(it->second)[k].id;
					cout<<" NALUnit "<<(it->second)[k].header[6].number+(255*((it->second)[k].header[7].number));
									cout<<" LID "<<(it->second)[k].header[3].number;
									cout<<" TID "<<(it->second)[k].header[4].number;
									cout<<" QID "<<(it->second)[k].header[5].number;
									cout<<" totalBytes "<<previouTotalBytes;
									cout<<" segmentID "<<(it->second)[k].header[0];
									cout<<" size in bytes "<<(it->second)[k].payload.size();/*header[1];*/
									cout<<" RTP ID "<<(it->second)[k].header[2];
					totalBytesCounted=0;
					currentNALUnit=(it->second)[k].header[6].number+255*((it->second)[k].header[6].number);
				}
				totalBytesCounted+=(it->second)[k].payload.size();/*header[1].number;*/
			}
			it++;
			if(it==decodedMapping.end())
				break;
		}
	}
	traceFile.close();
}



/*Show functions */
void Node::showNeighborList(){
	cout<<"Node"<<id<<" showNeighborList:";
	cout<<"NeighborList of node"<<id<<":";
	if(neighborList.empty()){
		cout<<"EMPTY"<<endl;
		return;
	}

	for(unsigned int i=0;i<neighborList.size();i++){
		cout<<"n"<<neighborList[i]->id<<" ";
	}
	cout<<endl;
}

void Node::showPacketQueue(const deque<Packet> &queue,string type){
	cout<<"Node"<<id<<" showPacketQueue:";
	cout<<"node"<<id<<" packet "<<type<<" Queue list:";
	char c;
	if(queue.empty()){
		cout<<"EMPTY"<<endl;
		return;
	}
	cout<<endl;
	deque<int> temp;
	map<int,int> tempMap;/*this has segmentid , times of reference*/
	for(unsigned int i=0;i<queue.size();i++){
		if(i%15==0)
			cout<<endl;
		cout<<"p"<<queue[i].id<<" seg"<<queue[i].getSegmentID()<<",";
		tempMap[queue[i].getSegmentID()]++;
	}
	cout<<endl;

	for(unsigned int i=0;i<temp.size();i++){
		if(i==0)
			cout<<"segments Completed ";
		cout<<"s"<<temp[i]<<" ";
	}
	cout<<endl;
	cout<<"results :"<<endl;
	for(map<int,int>::iterator it=tempMap.begin();it!=tempMap.end();it++){
		cout<<"segmentid"<<it->first<<":"<<it->second<<endl;
		if(it->second>10){
			cout<<"too packets for segment to send"<<endl;
			cin>>c;
		}
	}
}

void Node::showMsgQueue(deque<message*> &queue,string type){
	cout<<"Node"<<id<<" showMsgQueue:";
	cout<<"node"<<id<<" "<<type<<" Queue Message list:";
	if(queue.empty()){
		cout<<"EMPTY"<<endl;
		return;
	}
	cout<<endl;
	for(unsigned int i=0;i<queue.size();i++){
		cout<<queue[i]->getMessageType()<<queue[i]->id<<" ";
	}
	cout<<endl;
	for(unsigned int i=0;i<queue.size();i++){
			cout<<"receiver "<<queue[i]->receiverID<<" ";
	}
	cout<<endl;
	for(unsigned int i=0;i<queue.size();i++){
		cout<<"dimRequested "<<queue[i]->getDimensions()<<" ";
	}
	cout<<endl;
}

void Node::showSegQueue(const deque<segment> &queue,string type){

	cout<<"node"<<id<<" "<<type<<" Queue list:";
	for(unsigned int i=0;i<queue.size();i++){
		cout<<"s"<<queue[i].id<<"(";
		for(unsigned int j=0;j<queue[i].packets.size();j++){
			cout<<"p"<<(queue[i].packets[j]).id<<" ";
		}
		cout<<") |";
	}
	cout<<endl;
}

void Node::showGlobalSegList(){
	cout<<"showGlobalSegList:";
	cout<<"size of global seglist "<<globalSegList.size()<<endl;
	for(unsigned int i=0;i<globalSegList.size();i++){
		cout<<"seg"<<globalSegList[i]->id<<"("<<globalSegList[i]->packets.size()<<") ";
	}
	cout<<endl;
}

void Node::showSegInProgressMapping(){

	cout<<"Node"<<id<<" showSegInProgressMapping:";
	if(segInProgress.empty())
		return;

	cout<<"Node "<<id<<" packets downdloaded until now"<<endl;

	for(map<segment*,int>::iterator it=segInProgress.begin();it!=segInProgress.end();it++){
		cout<<"seg"<< it->first->id <<" ("<<it->second<<") ";
		cout<<decodedSegments[it->first];
		cout<<endl;
	}
}


void Node::showSegToDownload(){
	map<segment*,bool>::iterator i;
	cout<<"Node "<<id<<":";
	for(i=segToDownloadList.begin();i!=segToDownloadList.end();i++){
		cout<<"s"<<i->first->getID()<<" ";
	}
	cout<<endl;
}

void Node::showcoefTables(){
	cout<<"Node"<<id<<" showcoefTables:";
	cout<<"Coefficient tables for node "<<id;
	if(coefTable.empty()){
		cout<<" is empty"<<endl;
		return;
	}
	cout<<endl;
	cout.flush();
	for(map<segment*,deque<deque<unsigned char> > > ::iterator it=coefTable.begin();it!=coefTable.end();it++){
		cout<<"------------------segment "<<it->first->getID()<<"-----------------------------"<<endl;
		for(unsigned int i=0;i<it->second.size();i++){
			cout<<i+1<<"):";
			for(unsigned int j=0;j<it->second[i].size();j++){
				cout<<(int)it->second[i][j]<<" ";
			}
			cout<<endl;
		}
		cout<<"------------------segment "<<it->first->getID()<<"-----------------------------"<<endl;
		cout.flush();
	}
}


/*Function to print segment data inside file it parses decodedSegment mapping*/
void Node::showSegDataInFiles(ofstream &f){
	f<<"Number of downloaded segments "<<decodedSegments.size()<<endl;
	map<segment*,bool>::iterator it;

	for(it=decodedSegments.begin();it!=decodedSegments.end();it++){
		if(it->second==true){
			f<<"Segment"<<(it->first)->getID()<<endl;
			it->first->showSegmentPackets(f);
		}
	}
}


void Node::showDecodedMapping(int timeSlot){
	if(decodedMapping.empty()){
		return;
	}
	stringstream ss;
	ss << id;
	string decodedDataFile;
	map<segment*,deque<Packet> > ::iterator it;
	decodedDataFile = "Node"+ss.str()+"DecodedData.txt";
	decodedData.open(decodedDataFile.c_str() ,ios::out | ios::app);

	cout<<"node"<<id<<" decodedMapping size "<<decodedMapping.size()<<endl;

	for(it=decodedMapping.begin();it!=decodedMapping.end();it++){
		cout<<"seg"<<(it->first)->id<<" ";
	}
	cout<<endl;

	decodedData<<"================================================= "<<endl;
	decodedData<<"node "<<id<<" timeslot "<<timeSlot<<" decodedData:"<<endl;
	decodedData<<"================================================= "<<endl;

	if(decodedMapping.size()==1)
		it=decodedMapping.begin();
	else{
		it=decodedMapping.end();
		it--;
	}
	for(unsigned int i=0;i<(it->first)->packets.size();i++){
		decodedData<<"segmentID"<<(it->first)->getID()<<" packetIDinsideSegment "<<(it->second)[i].header[0];
		decodedData<<" NALUnit "<<(it->second)[i].header[6].number+(255*((it->second)[i].header[7].number));
		decodedData<<" LID "<<(it->second)[i].header[3].number;
		decodedData<<" TID "<<(it->second)[i].header[4].number;
		decodedData<<" QID "<<(it->second)[i].header[5].number;
		decodedData<<" size in bytes "<<(it->second)[i].payload.size();/*header[1];*/
		decodedData<<" RTP ID "<<(it->second)[i].header[2]<<endl;
		for(unsigned int j=0;j<(it->second)[i].payload.size();j++){
			decodedData<<(int)(it->second)[i].payload[j]<<")"<<(bitset<8>)(it->second)[i].payload[j]<<" ";
			if(j%10==0)
				decodedData<<endl;
		}
		decodedData<<endl;
	}
	decodedData<<"--------  segmentID"<<(it->first)->getID()<<"-------"<<endl;
	decodedData.close();
}
