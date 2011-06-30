#pragma once
//
//  TalkyBase.h
//  TalkyShapes
//
//  Created by Elliot Woods on 05/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//


#define TALKY_SLEEP_TIME 10
#define TALKY_RECONNECT_TIME 200

#define TALKY_BUFFER_IN_SIZE 100000
#define TALKY_BUFFER_OUT_SIZE 100000

#include "TalkyMessage.h"
#include "TalkyBundleMessage.h"

#include <vector>
#include <ctime>
#include <iostream>

using namespace std;

class TalkyBase
{
public:
	TalkyBase();
    virtual ~TalkyBase();
	
    ////////////////////////////////
    // MAY NEED SOME OVERRIDING
    // FOR ERROR HANDLING
    ////////////////////////////////
    //
	//setup as client
	virtual void    setup(string remoteHost, int remotePort);
	
	//setup as server
	virtual void    setup(int localPort);

    virtual bool	getIsServerBound();
    virtual int		getNumClients();
    //
    ////////////////////////////////
    
	bool	getIsConnected() { return isConnected; }
	float	getTimeUntilNextConnectNorm();
    
    string  getRemoteHost();
    
    void    send(TalkyMessage &msg);
	
	vector<TalkyMessage>		receiveQueue;
	
	bool	popMessage(unsigned int i, TalkyMessage &msg);
	void	clearMessages();
	
protected:
	void update();
    
    
    ////////////////////////////////
    // INTERFACE
    ////////////////////////////////
    //
    virtual void beginThread() = 0;
    
    virtual bool lockThread() = 0;
    virtual void unlockThread() = 0;
    virtual bool lockServer() { return true; };
    virtual void unlockServer() { };
    
    virtual void initClient() = 0;
    virtual void initServer() = 0;
    
    virtual void startClient() = 0;
    virtual void startServer() = 0;
    
    virtual bool isClientConnected() = 0;
    virtual bool isServerConnected() = 0;
    virtual bool isServersClientConnected(int iClient) = 0;
    
    virtual int getNumServerClients() = 0;
    
    virtual int rxServer(int iClient, char *buffer, int bufferSize) = 0;
    virtual int rxClient(char *buffer, int bufferSize) = 0;
    
    virtual void txServer(int iClient, char *buffer, int messageSize) = 0;
    virtual void txClient(char *buffer, int messageSize) = 0;
    
    virtual void notifyReceiveEvent(int msgCount) = 0;
	//
    ////////////////////////////////
    
    vector<TalkyMessage>		sendQueue;
	
	void    processIncomingBytes(int nBytes);
	
	char	bufferIn[TALKY_BUFFER_IN_SIZE];
	char	bufferOut[TALKY_BUFFER_OUT_SIZE];
	
	int				_localPort;
	
	string			_remoteHost;
	int				_remotePort;
	
	int				nodeType; //0=none, 1=client, 2=server
	
	bool			isServerBound; // have we got the port?
	bool			isConnected;
	long			lastConnectAttempt;
};