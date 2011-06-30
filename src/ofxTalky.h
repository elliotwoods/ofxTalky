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

class ofxTalky : public ofThread, public TalkyBase
{
public:
	~ofxTalky();
    
    //setup as client
	void setup(string remoteHost, int remotePort);
	
	//setup as server
	void setup(int localPort);
	
	bool	getIsServerBound();
	int		getNumClients();
    
    Poco::BasicEvent<int>   actionMsgRx;
	
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
    
	ofxTCPServer	*tcpServer;
	ofxTCPClient	*tcpClient;
};