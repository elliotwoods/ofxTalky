#pragma once
//
//  TalkyBundleMessage.h
//  TalkyShapes
//
//  Created by Elliot Woods on 09/02/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "TalkyMessage.h"

#include <vector>
using namespace std;

class TalkyBundleMessage
{    
    public:
        TalkyMessage    pack();
        void    unpack(TalkyMessage &bundleMsg, unsigned short newType= -1);
        
        void    push(TalkyMessage &msg);
    
        vector<TalkyMessage>    bundle;
    
};