/*
 * Node.h
 *
 *  Created on: Feb 17, 2014
 *      Author: vapashos
 */

#ifndef NODE_H_
#define NODE_H_
#include "channelWlan.h"
#include "request.h"
#include "advertisment.h"
#include "segment.h"
#include "decoder.h"
#include <set>

class Node {
public:

	/*Static variables*/
	static int counter;
	static channelWlan wChannel;
	static deque<segment*> globalSegList;
	static deque<message> messageList;
	static bool useWlan;
	static ofstream fl;

	ofstream ds;/*ds stream to write decoded segments*/
	ofstream befDec;/*stream to write packet payload before decoding*/
	ofstream afterDec;/*stream to write packet payload after decoding*/
	ofstream videoFile;/*this is the file of the video after transmittions*/
	ofstream videoFileNoZero;/*this is the file of the video after transmittions with no zero bytes whenever a data are not decoded*/
	ofstream videoFile3G;/*this is the file of the video after transmittions with no zero bytes whenever a data are not decoded*/
	ofstream videoFileDecSeg;/*output stream with file with data of segments that were only decoded*/
	ofstream traceFile; /*stream for trace File for the receiver*/
	ofstream decodedData;

	int id,index;	/*This is an index of the first packet of a segment inside queue3G
	 	 	 	 	 * it is updated any time user finishes downloading a segment inside recv3GPacket*/
					/*Packet Queues*/
	deque<Packet> queue3G;
	deque<Packet> inQueue;
	deque<Packet> outQueue;
	deque <Packet> tempQueue;
	/*Message Queues*/
	deque<message*> inMsgQueue;
	deque<message*> outMsgQueue;
	/*Segment Queue*/
	deque<segment> segQueue;
	map<segment*,int> segInProgress;
	map<segment*,bool> decodedSegments;
	/*Coefficient matrix this is used as input to the decoder to decode t
	 the coded packets when the appropriate number is selected
	 this is the first approach cause using this we do not check if
	 a new coded packet received is an innovative packet ,which means
	 that the coefficients vector is a linead independent vector comparing
	 with the others that have been already received*/
	map<segment*,deque<deque<unsigned char> > > coefTable;/*2dimensional Table for each segment*/

	/*Packets Successfully Decoded By Node*/
	map<segment*,deque<Packet> > decodedMapping;
	/*This is a mapping for set of coded packets that belong to a segment
	 * the coefficient matrix used for decoding and especially to determine
	 * each time if a packet received contains coeffients vector that is
	 * linearly independent comparing to these that have been already received
	 * This matrix coefficient Matrix is Used to determine the linear dependability
	 * of coefficients after this the coefficientMatrixNoGauss will be used for
	 * decoding */
	map<segment*,deque<deque  <ffNumber> > > coefficientMatrix;
	map<segment*,deque<deque  <ffNumber> > > coefficientMatrixNoGauss;
	/*Neighbor list*/
	deque<Node*> neighborList;
	/*Segments to Download List*/
	map<segment*,bool> segToDownloadList;/*if boolean value is true segment has been downloaded from 3G network*/
	/*Pointer on a decoder*/
	decoder* myDecoder;
	segment* bdSegment;		/* broadcasting segment */
	deque<segment *> advertisedSegments;//list of advertised segments
	/*Constructors Destructors*/
	Node();
	Node(const Node &n);
	virtual ~Node();

	/*functions */
	void addNeighbor(Node *x);
	void initChannelWLAN();
	void sendPacket(int timeSlot,int receiverID);
	void recvPacket(int timeSlot);
	void recv3GPacket(Packet &p,int timeSlot);
	bool broadcastPacket(int timeSlot,Node *brNode);
	Packet* findLatestPacket();
	Packet createCodedPacket(const segment &s,int timeSlot) const;/*create a new coded packet based on segment*/
	void createPoolOfCodedPackets(segment *s,int num,int timeSlot) ;/*creates a number of new coded packets based on segment packets are added directly on node's output queue using function createCodedPacket*/
	void checkIncomingCodedPackets(int timeSlot);
	segment* addCoefVector(deque<unsigned char> &c,segment *segID);/*Add vector on coefTable returns pointer on segment that can be decoded else NULL*/
	bool decodeSegment();
	/*Message functions*/
	void sendMessage(int timeSlot);
	void recvMessage(int timeSlot);
	bool isEmpty(const deque<Packet> &queue);

	void createRequest(int receiverID,int segID,int dimensions,int timeSlot);
	void createAdvertisment(int segID,int timeSlot);
	void addSegmentToDownloadList(segment *x);
	void checkSegmentDownload();/*At each time slot the node checks if he successfully downloaded a segment*/
	void initiateDecoder();
	int checkRequests();/*With this function node checks its requests inside inMsgQueue and determines the maximum dimension to send*/

	void composeVideoFile();
	/*Generate traceFile for the receiver*/
	void generateTraceFile();


	/*Get functions*/
	static segment * getSegment(int segID);

	/*Show functions*/
	void showNeighborList();
	void showNodeChannels();
	void showPacketQueue(const deque<Packet> &queue,string type);
	void showMsgQueue(deque<message*> &queue,string type);
	void showSegQueue(const deque<segment> &queue,string type);
	void showSegInProgressMapping();
	void showSegToDownload();
	void showcoefTables();
	static void showGlobalSegList();
	void showSegDataInFiles(ofstream &f);
	void showDecodedMapping(int timeSlot);
};

#endif /* NODE_H_ */
