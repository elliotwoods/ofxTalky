#pragma once
/*
 *  ofxTalky.h
 *
 *  Created by Elliot Woods on 01/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TalkyBase.h"


#include "ofxNetwork.h"
#include "ofMain.h"

//events
#include "Poco/BasicEvent.h"
using Poco::BasicEvent;

using namespace Talky;

class ofxTalky : public ofThread, public TalkyBase
{
public:
	~ofxTalky();
    
    ///setup as client
	void init(string remoteHost, int remotePort=TALKY_DEFAULT_PORT);
	
	///setup as server
	void init(int localPort=TALKY_DEFAULT_PORT);
	
	///We keep the old syntax for backwards compatability
	void setup(int localPort=TALKY_DEFAULT_PORT) { init(localPort); };
	void setup(string remoteHost, int remotePort=TALKY_DEFAULT_PORT) {init(remoteHost, remotePort); };
	
	bool	getIsServerBound();
	int		getNumClients();
    
    Poco::BasicEvent<int>		actionMsgRx;
	Poco::BasicEvent<int>		actionClientIsNowConnected;
	
protected:
    void threadedFunction();
    
    void beginThread(); 
    
    bool lockThread();
    void unlockThread();
    bool lockServer();
    void unlockServer();
    
    void initClient();
    void initServer();
    
    void startClient();
    void startServer();
    
    bool isClientConnected();
    bool isServerConnected();
    bool isServersClientConnected(int iClient);
	
    int getNumServerClients();
	
    int rxServer(int iClient, char *buffer, int bufferSize);
    int rxClient(char *buffer, int bufferSize);
	
    void txServer(int iClient, char *buffer, int messageSize);
    void txClient(char *buffer, int messageSize);

    void notifyReceiveEvent(int msgCount);
	void notifyClientIsNowConnected();
	
	void throwWarning(string s);
	void throwError(string s);
	
	/** In openFrameworks' ofxNetwork,
	 network errors arrive as negative
	 byte counts. We process them here.
	 */
	void processRxErrors(int& n);
    
	ofxTCPServer	*tcpServer;
	ofxTCPClient	*tcpClient;
};