/*
 *  ofxTalky.cpp
 *  network protocol test
 *
 *  Created by Elliot Woods on 01/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofxTalky.h"

///////////////////////////////////////////////////////////////
// PUBLIC
//

ofxTalky::~ofxTalky() {
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
    
    nodeType = 0;
    
    //hahahhahahahah
    delete this;
}

void ofxTalky::init(string remoteHost, int remotePort) {
    try {
        TalkyBase::setup(remoteHost, remotePort);
    } catch (string err) {
        ofLog(OF_LOG_ERROR, "ofxTalky" + err);
    }
}

void ofxTalky::init(int localPort) {
    try {
        TalkyBase::setup(localPort);
    } catch (string err) {
        ofLog(OF_LOG_ERROR, "ofxTalky" + err);
    }
}

bool ofxTalky::getIsServerBound() {
    try {
        return TalkyBase::getIsServerBound();
    } catch (string err) {
        ofLog(OF_LOG_ERROR, "ofxTalky" + err);
        return false;
    }
}

int ofxTalky::getNumClients() {
    try {
        return TalkyBase::getNumClients();
    } catch (string err) {
        ofLog(OF_LOG_ERROR, "ofxTalky" + err);
        return -1;
    }
}

///////////////////////////////////////////////////////////////
// PROTECTED
//

void ofxTalky::beginThread() {
    startThread(true, false);
}

void ofxTalky::threadedFunction() {
	//this is essentially our update loop
	while (isThreadRunning()) {
        
		ofSleepMillis(TALKY_SLEEP_TIME_MILLIS);
        
		try {
            update();
        }
        catch (string e)
        {
            cout << "ofxTalky: " << e << "\n";
        }
	}
	
	ofLog(OF_LOG_WARNING, "ofxTalky : thread closed");
}

bool ofxTalky::lockThread() {
    return lock();
}

void ofxTalky::unlockThread() {
    unlock();
}

bool ofxTalky::lockServer() {
    return tcpServer->lock();
}

void ofxTalky::unlockServer() {
    tcpServer->unlock();
}

void ofxTalky::initClient() {
    tcpClient = new ofxTCPClient();
	tcpClient->setVerbose(true);
}

void ofxTalky::initServer() {
    tcpServer = new ofxTCPServer();
	tcpServer->setVerbose(true);
}

void ofxTalky::startClient() {
	isConnected = tcpClient->setup(_remoteHost, _remotePort, false);
}

void ofxTalky::startServer() {
    tcpServer = new ofxTCPServer();
	tcpServer->setVerbose(true);
	isServerBound = tcpServer->setup(_localPort, false);
}

bool ofxTalky::isClientConnected() {
    return tcpClient->isConnected();
}

bool ofxTalky::isServerConnected() {
    return tcpServer->isConnected();
}

bool ofxTalky::isServersClientConnected(int iClient) {
    return tcpServer->isClientConnected(iClient);
}

int ofxTalky::getNumServerClients() {
    return tcpServer->getNumClients();
}

int ofxTalky::rxServer(int iClient, char *buffer, int bufferSize) {
	int nBytesReceived = tcpServer->receiveRawBytes(iClient, buffer, bufferSize);
	
	processRxErrors(nBytesReceived);
	return nBytesReceived;
}

int ofxTalky::rxClient(char *buffer, int bufferSize) {
	int nBytesReceived = tcpClient->receiveRawBytes(buffer, bufferSize);
	
	processRxErrors(nBytesReceived);
    return nBytesReceived;
}

void ofxTalky::processRxErrors(int& n) {
	//negative values in ofxNetwork signify error codes
	if (n < 0)
	{
		/*
		switch (n)
		{
			case SOCKET_TIMEOUT:
				ofLogWarning() << "Socket timeout";
				break;
			case SOCKET_ERROR:
				ofLogWarning() << "Socket error";
		}
		 */
		n = 0;
	}
}

void ofxTalky::txServer(int iClient, char *buffer, int messageSize) {
    tcpServer->sendRawBytes(iClient, buffer, messageSize);
}

void ofxTalky::txClient(char *buffer, int messageSize) {
    tcpClient->sendRawBytes(buffer, messageSize);
}

void ofxTalky::notifyReceiveEvent(int msgCount) {
    actionMsgRx.notify(this, msgCount);
}

void ofxTalky::notifyClientIsNowConnected() {
	int msg = 0;
	actionClientIsNowConnected.notify(this, msg);
}

void ofxTalky::throwWarning(string s) {
	ofLogWarning() << s;
}

void ofxTalky::throwError(string s) {
	ofLogError() << s;
}