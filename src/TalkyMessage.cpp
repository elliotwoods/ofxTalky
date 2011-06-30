/*
 *  TalkyMessage.cpp
 *  network protocol test
 *
 *  Created by Elliot Woods on 01/02/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "TalkyMessage.h"

TalkyMessage::TalkyMessage() :
hasPayload(false),
PayloadLength(0),
Version(0),
Timestamp(clock() / CLOCKS_PER_MILLISEC),
ContentsType(0)
{
}

bool TalkyMessage::serialise(char* &message, int &remainingAvailableBytes)
{
	
	int len = getTotalLength();
	if (len > remainingAvailableBytes)
		return false;
	
    * (unsigned char *) message = 0;
    
	memcpy(message+1, Company, 2);
	memcpy(message+3, Protocol, 2);
    
    * (unsigned short *) (message + 5)  = Version;
    * (unsigned long *)  (message + 7)  = Timestamp;
    * (unsigned short *) (message + 11) = ContentsType;
    * (unsigned short *) (message + 13) = PayloadLength;
    
	memcpy(message+15, Payload, PayloadLength);
	
	message[len-1] = TALKY_ENDCHAR;
	
	message += len;
	remainingAvailableBytes -= len;
	
	return true;	
}

bool TalkyMessage::deSerialise(char* &message, int &remainingBytesReceived)
{
    if (* (unsigned char *) message != 0)
        throw("TalkyMessage::deSerialise : We're trying to deserialise a non-simple message to a simple talkymessage");

    memcpy(Company, message+1, 2);
    memcpy(Protocol, message+3, 2);
    
    unsigned short tempLength;
    Version         = * (unsigned short *) (message + 5);
    Timestamp       = * (unsigned long *)  (message + 7);
    ContentsType    = * (unsigned short *) (message + 11);
    tempLength      = * (unsigned short *) (message + 13);
    
    initPayload(tempLength);
    
    if (getTotalLength() > remainingBytesReceived)
        throw("TalkyMessage::deSerialise : We've run out of message to deserialise");
    
    memcpy(Payload, message+15, PayloadLength);
    
    if (message[getTotalLength()-1] != TALKY_ENDCHAR)
        throw("TalkyMessage::deSerialise : End charachter of message is wrong when deserialising");
	
	message += getTotalLength();
	remainingBytesReceived -= getTotalLength();
	
	return true;
}

char *TalkyMessage::getPayload(int &length) const
{
	length = PayloadLength;
	return getPayload();
}

char *TalkyMessage::getPayload() const
{
	return Payload;
}

void TalkyMessage::setPayload(char* const message, unsigned short length)
{
	initPayload(length);
	memcpy(Payload, message, length);
}

int TalkyMessage::getTotalLength()
{
	return      1 // message type
            +   2 // company
            +   2 // protocol
            +   2 // version
            +   4 // timestamp
            +   2 // contents type
            +   2 // Payload length
            +   PayloadLength //Payload
            +   1; //endchar
}

unsigned short TalkyMessage::getPayloadLength()
{
    return PayloadLength;
}

string TalkyMessage::toString()
{
	
	stringstream out;

	out << "Company:\t" << string(Company, 2) << "\n";
	out << "Protocol:\t" << string(Protocol, 2) << "\n";
	out << "Version:\t" << Version << "\n";
	out << "Timestamp:\t" << Timestamp << "\n";
	out << "ContentsType:\t" << ContentsType << "\n";
	out << "PayloadLength:\t" << PayloadLength << "\n";
	
	out << "Payload:\n";
	
	for (int i=0; i<PayloadLength; i++)
	{
		if (Payload[i] > 32)
			out << Payload[i];
		else
			out << ".";
	}
	
	out << "\n\n";
	
	return out.str();
}

void TalkyMessage::setCompany(const char * s)
{
	memcpy(Company, s, 2);
}

void TalkyMessage::setProtocol(const char * s)
{
	memcpy(Protocol, s, 2);
}

void TalkyMessage::setTimestamp()
{
	Timestamp = clock() / CLOCKS_PER_MILLISEC;
}

bool TalkyMessage::operator<(const TalkyMessage& other) const
{
	return Timestamp < other.Timestamp;
}

void TalkyMessage::initPayload(unsigned short length)
{
	if (hasPayload && PayloadLength != length)
	{
		delete[] Payload;
		hasPayload = false;
	}
	
	if (!hasPayload)
	{
		Payload = new char[length];
		PayloadLength = length;
	}
}