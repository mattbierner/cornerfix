/*
 *  staticLib.cp
 *  staticLib
 *
 *  Created by Matt Bierner on 5/20/14.
 *  Copyright (c) 2014 Sandy McGuffog. All rights reserved.
 *
 */

#include <iostream>
#include "staticLib.h"
#include "staticLibPriv.h"

void staticLib::HelloWorld(const char * s)
{
	 staticLibPriv *theObj = new staticLibPriv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void staticLibPriv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};

