//
//  TalkyBase.cpp
//  TalkyShapes
//
//  Created by Elliot Woods on 05/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TalkyBase.h"


///////////////////////////////////////////////////////////////
// PUBLIC
//

TalkyBase::TalkyBase() : 
nodeType(0),
lastConnectAttempt(0),
isConnected(false),
isServerBound(false)
{
    
}

TalkyBase::~TalkyBase()
{
    if (nodeType != 0)
        throw("You need to implement a destructor in your Talky class. Check  TalkyBase::~TalkyBase() for notes");
  /*
   //implement the following in destructor of TalkyBase implementing class. Example code from ofxTalky:
   
   if (nodeType==0)
       return;
   
   stopThread(true);
   
   if (nodeType == 1)
   {
       tcpClient->close();
       delete tcpClient;
   }
   
   if (nodeType == 2)
   {
       tcpServer->close();
       delete tcpServer;
   }
   
   */
}


//CLIENT SETUP
void TalkyBase::setup(string remoteHost, int remotePort)
{
	if (nodeType != 0)
	{
		throw(string("Already initialised as node type ") + (nodeType==1 ? string("server") : string("client")));
		return;
	}
	
    
	_remoteHost = remoteHost;
	_remotePort = remotePort;
	
    initClient();
	beginThread();
	
	nodeType = 1;
	
}

//SERVER SETUP
void TalkyBase::setup(int localPort)
{
	if (nodeType != 0)
	{
		throw("Already initialised as node type " + (nodeType==1 ? string("server") : string("client")));
		return;
	}
	
	_localPort = localPort;
	
    initServer();
	beginThread();
	
	nodeType = 2;
	
}

bool TalkyBase::getIsServerBound()
{
	if (nodeType==2)
		return isServerBound;
	else
		throw("I'm not a server node, so can't possibly be bound! so why are you asking?");
	return false;
}

int TalkyBase::getNumClients()
{
	if (nodeType==2)
        return getNumServerClients();
	else
        throw ("I'm not a server node, so can't possibly have clients! so why are you asking?");
	return -1;
}

float TalkyBase::getTimeUntilNextConnectNorm()
{
	float frac =  float(clock() / CLOCKS_PER_MILLISEC - lastConnectAttempt) / float(TALKY_RECONNECT_TIME);
    
    return (frac < 1.0f ? frac : 1.0f);
}

string TalkyBase::getRemoteHost()
{
    if (nodeType == 1)
        return _remoteHost;
    else
        return "";
}

void TalkyBase::send(TalkyMessage &msg)
{    
    sendQueue.push_back(msg);
}

bool TalkyBase::popMessage(unsigned int i, TalkyMessage &msg)
{
	if (!lockThread())
		return false;
	
	if (receiveQueue.size() > 0)
	{
		msg = receiveQueue.front();
		receiveQueue.erase(receiveQueue.begin());
		
		unlockThread();
		return true;
	} else	{
		unlockThread();
		return false;
	}
	
	
}

void TalkyBase::clearMessages()
{
	while (!lockThread())
		sleep(10);
	
	receiveQueue.clear();
	
	unlockThread();
}


///////////////////////////////////////////////////////////////
// PROTECTED
//

void TalkyBase::update()
{
	if (nodeType == 0)
		return;
	
	/////////////////////////////////////
	// CHECK RECONNECTS
	/////////////////////////////////////
	//
	int currentTime = clock() / CLOCKS_PER_MILLISEC;
	if (currentTime - lastConnectAttempt > TALKY_RECONNECT_TIME)
	{
		
		if (nodeType == 1)
			//client
			isConnected = isClientConnected();
		else
			//server
			isConnected = isServerConnected();
        
		
		if (!isConnected && nodeType==1)
		{
			//client
            startClient();
		}
		
		
		if (!isServerBound && nodeType==2)
		{
			//server
            startServer();
		}
		
		lastConnectAttempt = currentTime;
	}
	//
	/////////////////////////////////////
	
	
	
	/////////////////////////////////////
	// RECEIVE DATA
	/////////////////////////////////////
	//
	int availableBytes;
	
	if (nodeType == 1)
	{	
		if (isClientConnected())
		{			
			//client
			availableBytes = rxClient(bufferIn, TALKY_BUFFER_IN_SIZE);
            
			if (availableBytes > 0)
				processIncomingBytes(availableBytes);
		}
		
	} else {
		
		//server
		if (getNumServerClients() > 0) {
			
			if (lockServer())
			{
				for (int iRemote=0; iRemote<getNumServerClients() ; iRemote++)
				{
					if (!isServersClientConnected(iRemote))
						continue; 
					
					availableBytes = rxServer(iRemote, bufferIn, TALKY_BUFFER_IN_SIZE);					
					
					if (availableBytes > 0)
						processIncomingBytes(availableBytes);
					
				}
				unlockServer();
			}
            
		}
	}
        
	//
	/////////////////////////////////////
	
	
	
	/////////////////////////////////////
	// SEND DATA
	/////////////////////////////////////
	//
	if (isConnected)
	{
		int availableBytes = TALKY_BUFFER_OUT_SIZE;
		char * bufferPointer = bufferOut;
		bool hasMessage = false;
        
        vector<TalkyMessage>::iterator it;
        
		while (sendQueue.size() > 0)
		{
            //point to the one we want to deal with
            it = sendQueue.begin();
            
            //let's try to serialise
            if (!it->serialise(bufferPointer, availableBytes))
                break;
            
			//if we were able to put this message into the buffer
			//i.e. there was sufficient space in the buffer, no exceptions
			//then remove it from send message queue
			sendQueue.erase(it);
			
			hasMessage = true;
		}
		
		if (hasMessage)
			if (nodeType == 1)
				
				//client
                txClient(bufferOut, TALKY_BUFFER_OUT_SIZE - availableBytes);
		
			else
				
				//server
				for (int iClient=0; iClient < getNumServerClients(); iClient++)
                    txServer(iClient, bufferOut, TALKY_BUFFER_OUT_SIZE - availableBytes);
		
	}
	//
	/////////////////////////////////////
	
	
}

void TalkyBase::processIncomingBytes(int nBytes)
{
	lockThread();
	
	char * messagePointer = bufferIn;
	
	//perhaps recode this so we dont copy
	TalkyMessage currentMessage;
	
	while (nBytes > 0 && currentMessage.deSerialise(messagePointer, nBytes))
		receiveQueue.push_back(currentMessage);
	
	sort(receiveQueue.begin(), receiveQueue.end());
	
    //trigger message available event
    //processing will be performed in this
    //thread
    int msgCount = receiveQueue.size();
    notifyReceiveEvent(msgCount);
    
	unlockThread();
}
