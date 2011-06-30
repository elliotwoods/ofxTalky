//
//  TalkyBundleMessage.cpp
//  TalkyShapes
//
//  Created by Elliot Woods on 09/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TalkyBundleMessage.h"

TalkyMessage TalkyBundleMessage::pack()
{           
    
    ///////////////////////////////////
    // WRITE PAYLOAD
    ///////////////////////////////////
    //
    
    
    //setup message
    TalkyMessage bundleMsg;
    //
    
    //calculate payload size
    //
    //2 + sum(message + 2)
    //
    vector<TalkyMessage>::iterator it;
    unsigned short totalPayloadLength = 2;
    //
    for (it = bundle.begin(); it != bundle.end(); it++)
        totalPayloadLength += it->getPayloadLength() + 2;
    //
    
    
    //setup single master payload in message
    bundleMsg.initPayload(totalPayloadLength);
    char * tempPayload = bundleMsg.getPayload();
    //
    //nMessages
    * (unsigned short *)(tempPayload) = bundle.size();
    tempPayload += 2;
    //
    
    
    //add individual payloads to master payload
    unsigned short msgLength;
    //
    for (it = bundle.begin(); it != bundle.end(); it++)
    {
        msgLength = it->getPayloadLength();
        
        //payload length
        * (unsigned short *)(tempPayload) = msgLength;
        tempPayload += 2;
        
        //payload
        memcpy(tempPayload, it->getPayload(), msgLength);
        tempPayload += msgLength;
    }
    //
    ///////////////////////////////////
    
    return bundleMsg;
}

void TalkyBundleMessage::unpack(TalkyMessage &bundleMsg, unsigned short newType)
{   
    
    ///////////////////////////////////
    // OVERRIDE FOR SPECIFIC TYPE
    ///////////////////////////////////
    //
    if (newType == -1)
        newType = bundleMsg.ContentsType;
    //
    ///////////////////////////////////
    
    
    
    ///////////////////////////////////
    // READ PAYLOAD TO BUNDLE VECTOR
    ///////////////////////////////////
    //
    int remainingBytes;
    char * tempPayload = bundleMsg.getPayload(remainingBytes);

    
    //count messages
    unsigned short nMessages = * (unsigned short*) tempPayload;
    tempPayload+=2;
    remainingBytes -= 2;
    //
    bundle.resize(nMessages);
    //
    
    /////////////////////////
    //set individual messages
    unsigned short msgLength;
    for (unsigned short i=0; i<nMessages; i++)
    {
        msgLength = * (unsigned short*) tempPayload;
        tempPayload += 2;
        remainingBytes -= 2;
        
        if (remainingBytes < msgLength)
            throw("TalkyMessageBundle::deSerialse : Insufficient bytes in message");
        
        //set header
        TalkyMessage pushMsg;
        //
        pushMsg.Company[0]      = bundleMsg.Company[0];
        pushMsg.Company[1]      = bundleMsg.Company[1];
        pushMsg.Protocol[0]     = bundleMsg.Protocol[0];
        pushMsg.Protocol[1]     = bundleMsg.Protocol[1];
        pushMsg.ContentsType    = newType;
        pushMsg.Version         = bundleMsg.Version;
        pushMsg.Timestamp       = bundleMsg.Timestamp;
        //
        
        pushMsg.setPayload (tempPayload, msgLength);
        tempPayload += msgLength;
        remainingBytes -= msgLength;
        
        bundle[i] = pushMsg;
    }
    /////////////////////////
    
    //
    ///////////////////////////////////

}

void TalkyBundleMessage::push(TalkyMessage &msg)
{
    bundle.push_back(msg);
}