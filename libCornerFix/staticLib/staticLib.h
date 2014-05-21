/*
 *  staticLib.h
 *  staticLib
 *
 *  Created by Matt Bierner on 5/20/14.
 *  Copyright (c) 2014 Sandy McGuffog. All rights reserved.
 *
 */

#ifndef staticLib_
#define staticLib_

/* The classes below are exported */
#pragma GCC visibility push(default)

class staticLib
{
	public:
		void HelloWorld(const char *);
};

#pragma GCC visibility pop
#endif
