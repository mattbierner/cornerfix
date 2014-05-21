/*
 *  staticLibPriv.h
 *  staticLib
 *
 *  Created by Matt Bierner on 5/20/14.
 *  Copyright (c) 2014 Sandy McGuffog. All rights reserved.
 *
 */

/* The classes below are not exported */
#pragma GCC visibility push(hidden)

class staticLibPriv
{
	public:
		void HelloWorldPriv(const char *);
};

#pragma GCC visibility pop
