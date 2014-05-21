/* ======================================
 * CornerFix - Fixing the M8 Cyan Corners
 * ======================================
 *
 * Project Info:  http://sourceforge.net/projects/cornerfix
 * Project Lead:  Sandy McGuffog (sandy.cornerfix@gmail.com);
 *
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * This program is free software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation;
 * either version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this
 * library; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * ---------------
 * dng_makernotes.cpp
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   The Leica MakerNote decoding information was provided by 
 *                   Carl Bretteville;
 *
 *
 * Changes
 * -------
 * 24 October 2007 : Original version;
 * 29 November 2007 : 0.9.1.7
 * 5 February 2007 : 0.9.2.0
 * 2 December 2008 : 1.0.0.0
 * 12 June 2009 : 1.1.0.0
 * 21 Nov 2009 : 1.3.0.4
 * 23 January 2011 : 1.4.1.0
 * 4 April 2011 : 1.4.1.1
 * 4 July 2011 : 1.4.2.0
 *
 */

/*
 * Decodes and stores maker specific data
 */

#include "dng_makernotes.h"

#include "dng_tag_codes.h"
#include "dng_tag_types.h"

static const char *LeicaLensNames[MAX_LEICA_FRAMELINES][MAX_LEICA_LENSES] = {
	{		// Framelines 0
		"Uncoded/Not detected",						// 0
		"Elmarit-M 21mm f/2.8",						// 1
		"Lens Code: 2",								// 2
		"Elmarit-M 28mm f/2.8 (III)",				// 3
		"Tele-Elmarit-M 90mm f/2.8 (II)",			// 4
		"Summilux-M 50mm f/1.4 (II)",				// 5
		"Summilux-M 35mm f/1.4",					// 6
		"Summicron-M 90mm f/2 (II)",				// 7
		"Lens Code: 8",								// 8
		"Apo-Telyt-M 135mm f/3.4",					// 9
		"Lens Code: 10",							// 10
		"Lens Code: 11",							// 11
		"Lens Code: 12",							// 12
		"Lens Code: 13",							// 13
		"Lens Code: 14",							// 14
		"Lens Code: 15",							// 15
		"Tri-Elmar-M 16-18-21mm f/4 ASPH.",			// 16
		"Lens Code: 17",							// 17
		"Lens Code: 18",							// 18
		"Lens Code: 19",							// 19
		"Lens Code: 20",							// 20
		"Lens Code: 21",							// 21
		"Lens Code: 22",							// 22
		"Summicron-M 50mm f/2 (III)",				// 23
		"Elmarit-M 21mm f/2.8 ASPH.",				// 24
		"Elmarit-M 24mm f/2.8 ASPH.",				// 25
		"Summicron-M 28mm f/2 ASPH.",				// 26
		"Elmarit-M 28mm f/2.8 (IV)",				// 27
		"Elmarit-M 28mm f/2.8 ASPH.",				// 28
		"Summilux-M 35mm f/1.4 ASPHERICAL",			// 29
		"Summicron-M 35mm f/2 ASPH.",				// 30
		"Noctilux-M 50mm f/1.2",					// 31
		"Summilux-M 50mm f/1.4 ASPH.",				// 32
		"Summicron-M 50mm f/2 (IV, V)",				// 33
		"Elmar-M 50mm f/2.8",						// 34
		"Summilux-M 75mm f/1.4",					// 35
		"Apo-Summicron-M 75mm f/2 ASPH.",			// 36
		"Apo-Summicron-M 90mm f/2 ASPH.",			// 37
		"Elmarit-M 90mm f/2.8",						// 38
		"Tele-Elmar-M 135mm f/4 (II)",				// 39
		"Macro-Adapter M",							// 40
		"Lens Code: 41",							// 41
		"Tri-Elmar-M 28-35-50mm f/4 ASPH.",			// 42
		"Summarit-M 35mm f/2.5",					// 43
		"Summarit-M 50mm f/2.5",					// 44
		"Summarit-M 75mm f/2.5",					// 45
		"Summarit-M 90mm f/2.5",					// 46
		"Summilux-M 21mm f/1.4",					// 47
		"Summilux-M 24mm f/1.4",					// 48
		"Noctilux 50mm f/0.95",						// 49
		"Elmar-M 24mm f/3.8",						// 50
		"Super-Elmar-M 21mm f/3.4 ASPH.",			// 51
		"Super-Elmar-M 18mm f/3.8 ASPH.",			// 52
		"Summicron-M 50mm f/2"						// 53
	},
	{			// Framelines 1
		"Uncoded/Not detected",						// 0
		"Elmarit-M 21mm f/2.8",						// 1
		"Lens Code: 2",								// 2
		"Elmarit-M 28mm f/2.8 (III)",				// 3
		"Tele-Elmarit-M 90mm f/2.8 (II)",			// 4
		"Summilux-M 50mm f/1.4 (II)",				// 5
		"Summicron-M 35mm f/2 (IV)",				// 6
		"Summicron-M 90mm f/2 (II)",				// 7
		"Lens Code: 8",								// 8
		"Elmarit-M 135mm f/2.8 (I/II)",				// 9
		"Lens Code: 10",							// 10
		"Lens Code: 11",							// 11
		"Lens Code: 12",							// 12
		"Lens Code: 13",							// 13
		"Lens Code: 14",							// 14
		"Lens Code: 15",							// 15
		"Tri-Elmar-M 16-18-21mm f/4 ASPH. 16mm",	// 16
		"Lens Code: 17",							// 17
		"Lens Code: 18",							// 18
		"Lens Code: 19",							// 19
		"Lens Code: 20",							// 20
		"Lens Code: 21",							// 21
		"Lens Code: 22",							// 22
		"Summicron-M 50mm f/2 (III)",				// 23
		"Elmarit-M 21mm f/2.8 ASPH.",				// 24
		"Elmarit-M 24mm f/2.8 ASPH.",				// 25
		"Summicron-M 28mm f/2 ASPH.",				// 26
		"Elmarit-M 28mm f/2.8 (IV)",				// 27
		"Elmarit-M 28mm f/2.8 ASPH.",				// 28
		"Summilux-M 35mm f/1.4 ASPH.",				// 29
		"Summicron-M 35mm f/2 ASPH.",				// 30
		"Noctilux-M 50mm f/1",						// 31
		"Summilux-M 50mm f/1.4 ASPH.",				// 32
		"Summicron-M 50mm f/2 (IV, V)",				// 33
		"Elmar-M 50mm f/2.8",						// 34
		"Summilux-M 75mm f/1.4",					// 35
		"Apo-Summicron-M 75mm f/2 ASPH.",			// 36
		"Apo-Summicron-M 90mm f/2 ASPH.",			// 37
		"Elmarit-M 90mm f/2.8",						// 38
		"Macro-Elmar-M 90mm f/4",					// 39
		"Macro-Adapter M",							// 40
		"Lens Code: 41",							// 41
		"Tri-Elmar-M 28-35-50mm f/4 ASPH.",			// 42
		"Summarit-M 35mm f/2.5",					// 43
		"Summarit-M 50mm f/2.5",					// 44
		"Summarit-M 75mm f/2.5",					// 45
		"Summarit-M 90mm f/2.5",					// 46
		"Summilux-M 21mm f/1.4",					// 47
		"Summilux-M 24mm f/1.4",					// 48
		"Noctilux 50mm f/0.95",						// 49
		"Elmar-M 24mm f/3.8",						// 50
		"Super-Elmar-M 21mm f/3.4 ASPH.",			// 51
		"Super-Elmar-M 18mm f/3.8 ASPH.",			// 52
		"Summicron-M 50mm f/2"						// 53
	},
	{			// Framelines 2
		"Uncoded/Not detected",						// 0
		"Elmarit-M 21mm f/2.8",						// 1
		"Lens Code: 2",								// 2
		"Elmarit-M 28mm f/2.8 (III)",				// 3
		"Tele-Elmarit-M 90mm f/2.8 (II)",			// 4
		"Summilux-M 50mm f/1.4 (II)",				// 5
		"Summicron-M 35mm f/2 (IV)",				// 6
		"Summicron-M 90mm f/2 (II)",				// 7
		"Lens Code: 8",								// 8
		"Elmarit-M 135mm f/2.8 (I/II)",				// 9
		"Lens Code: 10",							// 10
		"Lens Code: 11",							// 11
		"Lens Code: 12",							// 12
		"Lens Code: 13",							// 13
		"Lens Code: 14",							// 14
		"Lens Code: 15",							// 15
		"Tri-Elmar-M 16-18-21mm f/4 ASPH. 18mm",	// 16
		"Lens Code: 17",							// 17
		"Lens Code: 18",							// 18
		"Lens Code: 19",							// 19
		"Lens Code: 20",							// 20
		"Lens Code: 21",							// 21
		"Lens Code: 22",							// 22
		"Summicron-M 50mm f/2 (III)",				// 23
		"Elmarit-M 21mm f/2.8 ASPH.",				// 24
		"Elmarit-M 24mm f/2.8 ASPH.",				// 25
		"Summicron-M 28mm f/2 ASPH.",				// 26
		"Elmarit-M 28mm f/2.8 (IV)",				// 27
		"Elmarit-M 28mm f/2.8 ASPH.",				// 28
		"Summilux-M 35mm f/1.4 ASPH.",				// 29
		"Summicron-M 35mm f/2 ASPH.",				// 30
		"Noctilux-M 50mm f/1",						// 31
		"Summilux-M 50mm f/1.4 ASPH.",				// 32
		"Summicron-M 50mm f/2 (IV, V)",				// 33
		"Elmar-M 50mm f/2.8",						// 34
		"Summilux-M 75mm f/1.4",					// 35
		"Apo-Summicron-M 75mm f/2 ASPH.",			// 36
		"Apo-Summicron-M 90mm f/2 ASPH.",			// 37
		"Elmarit-M 90mm f/2.8",						// 38
		"Macro-Elmar-M 90mm f/4",					// 39
		"Macro-Adapter M",							// 40
		"Lens Code: 41",							// 41
		"Tri-Elmar-M 28-35-50mm f/4 ASPH.",			// 42
		"Summarit-M 35mm f/2.5",					// 43
		"Summarit-M 50mm f/2.5",					// 44
		"Summarit-M 75mm f/2.5",					// 45
		"Summarit-M 90mm f/2.5",					// 46
		"Summilux-M 21mm f/1.4",					// 47
		"Summilux-M 24mm f/1.4",					// 48
		"Noctilux 50mm f/0.95",						// 49
		"Elmar-M 24mm f/3.8",						// 50
		"Super-Elmar-M 21mm f/3.4 ASPH.",			// 51
		"Super-Elmar-M 18mm f/3.8 ASPH.",			// 52
		"Summicron-M 50mm f/2"						// 53
	},
	{			// Framelines 3
		"Uncoded/Not detected",						// 0
		"Elmarit-M 21mm f/2.8",						// 1
		"Lens Code: 2",								// 2
		"Elmarit-M 28mm f/2.8 (III)",				// 3
		"Tele-Elmarit-M 90mm f/2.8 (II)",			// 4
		"Summilux-M 50mm f/1.4 (II)",				// 5
		"Summicron-M 35mm f/2 (IV)",				// 6
		"Summicron-M 90mm f/2 (II)",				// 7
		"Lens Code: 8",								// 8
		"Elmarit-M 135mm f/2.8 (I/II)",				// 9
		"Lens Code: 10",							// 10
		"Lens Code: 11",							// 11
		"Lens Code: 12",							// 12
		"Lens Code: 13",							// 13
		"Lens Code: 14",							// 14
		"Lens Code: 15",							// 15
		"Tri-Elmar-M 16-18-21mm f/4 ASPH. 21 mm",	// 16
		"Lens Code: 17",							// 17
		"Lens Code: 18",							// 18
		"Lens Code: 19",							// 19
		"Lens Code: 20",							// 20
		"Lens Code: 21",							// 21
		"Lens Code: 22",							// 22
		"Summicron-M 50mm f/2 (III)",				// 23
		"Elmarit-M 21mm f/2.8 ASPH.",				// 24
		"Elmarit-M 24mm f/2.8 ASPH.",				// 25
		"Summicron-M 28mm f/2 ASPH.",				// 26
		"Elmarit-M 28mm f/2.8 (IV)",				// 27
		"Elmarit-M 28mm f/2.8 ASPH.",				// 28
		"Summilux-M 35mm f/1.4 ASPH.",				// 29
		"Summicron-M 35mm f/2 ASPH.",				// 30
		"Noctilux-M 50mm f/1",						// 31
		"Summilux-M 50mm f/1.4 ASPH.",				// 32
		"Summicron-M 50mm f/2 (IV, V)",				// 33
		"Elmar-M 50mm f/2.8",						// 34
		"Summilux-M 75mm f/1.4",					// 35
		"Apo-Summicron-M 75mm f/2 ASPH.",			// 36
		"Apo-Summicron-M 90mm f/2 ASPH.",			// 37
		"Elmarit-M 90mm f/2.8",						// 38
		"Macro-Elmar-M 90mm f/4",					// 39
		"Macro-Adapter M",							// 40
		"Lens Code: 41",							// 41
		"Tri-Elmar-M 28-35-50mm f/4 ASPH.",			// 42
		"Summarit-M 35mm f/2.5",					// 43
		"Summarit-M 50mm f/2.5",					// 44
		"Summarit-M 75mm f/2.5",					// 45
		"Summarit-M 90mm f/2.5",					// 46
		"Summilux-M 21mm f/1.4",					// 47
		"Summilux-M 24mm f/1.4",					// 48
		"Noctilux 50mm f/0.95",						// 49
		"Elmar-M 24mm f/3.8",						// 50
		"Super-Elmar-M 21mm f/3.4 ASPH.",			// 51
		"Super-Elmar-M 18mm f/3.8 ASPH.",			// 52
		"Summicron-M 50mm f/2"						// 53
	},
};

static const real64 LeicaMaxFStop[MAX_LEICA_LENSES] = {
	16.0, // Uncoded/Not detected			: 0
	16.0, // Elmarit-M 21mm f/2.8			: 1
	16.0, // Lens Code: 2					: 2
	16.0, // Elmarit-M 28mm f/2.8 (III)		: 3
	16.0, // Tele-Elmarit-M 90mm f/2.8 (II)	: 4
	16.0, // Summilux-M 50mm f/1.4 (II)		: 5
	16.0, // Summicron-M 35mm f/2 (IV)		: 6
	16.0, // Summicron-M 90mm f/2 (II)		: 7
	16.0, // Lens Code: 8					: 8
	16.0, // Elmarit-M 135mm f/2.8 (I/II)	: 9
	16.0, // Lens Code: 10					: 10
	16.0, // Lens Code: 11					: 11
	16.0, // Lens Code: 12					: 12
	16.0, // Lens Code: 13					: 13
	16.0, // Lens Code: 14					: 14
	16.0, // Lens Code: 15					: 15
	22.0, // Tri-Elmar-M 16-18-21mm f/4 ASPH.: 16
	16.0, // Lens Code: 17					: 17
	16.0, // Lens Code: 18					: 18
	16.0, // Lens Code: 19					: 19
	16.0, // Lens Code: 20					: 20
	16.0, // Lens Code: 21					: 21
	16.0, // Lens Code: 22					: 22
	16.0, // Summicron-M 50mm f/2 (III)		: 23
	16.0, // Elmarit-M 21mm f/2.8 ASPH.		: 24
	16.0, // Elmarit-M 24mm f/2.8 ASPH.		: 25
	16.0, // Summicron-M 28mm f/2 ASPH.		: 26
	22.0, // Elmarit-M 28mm f/2.8 (IV)		: 27
	22.0, // Elmarit-M 28mm f/2.8 ASPH.		: 28
	16.0, // Summilux-M 35mm f/1.4 ASPH.	: 29
	16.0, // Summicron-M 35mm f/2 ASPH.		: 30
	16.0, // Noctilux-M 50mm f/1			: 31
	16.0, // Summilux-M 50mm f/1.4 ASPH.	: 32
	16.0, // Summicron-M 50mm f/2 (IV, V)	: 33
	16.0, // Elmar-M 50mm f/2.8				: 34
	16.0, // Summilux-M 75mm f/1.4			: 35
	16.0, // Apo-Summicron-M 75mm f/2 ASPH.	: 36
	16.0, // Apo-Summicron-M 90mm f/2 ASPH.	: 37
	22.0, // Elmarit-M 90mm f/2.8			: 38
	32.0, // Macro-Elmar-M 90mm f/4			: 39
	22.0, // Macro-Adapter M				: 40
	16.0, // Lens Code: 41					: 41
	22.0, // Tri-Elmar-M 28-35-50mm f/4 ASPH.: 42
	16.0, // Summarit-M 35mm f/2.5			: 43
	16.0, // Summarit-M 50mm f/2.5			: 44
	16.0, // Summarit-M 75mm f/2.5			: 45
	16.0, // Summarit-M 90mm f/2.5			: 46
	16.0, // Summilux-M 21mm f/1.4			: 47
	16.0, // Summilux-M 24mm f/1.4			: 48
	16.0, // Noctilux 50mm f/0.95			: 49
	16.0, // Elmar-M 24mm f/3.8				: 50
	16.0, // Super-Elmar-M 21mm f/3.4 ASPH.	: 51
	16.0, // Elmar-M 18mm f/3.8 ASPH		: 52
	16.0, // Summicron-M 50mm f/2			: 53
};

static const real64 LeicaMinFStop[MAX_LEICA_LENSES] = {
	1.0, // Uncoded/Not detected			: 0
	2.8, // Elmarit-M 21mm f/2.8			: 1
	1.0, // Lens Code: 10					: 2
	2.8, // Elmarit-M 28mm f/2.8 (III)		: 3
	2.8, // Tele-Elmarit-M 90mm f/2.8 (II)	: 4
	1.4, // Summilux-M 50mm f/1.4 (II)		: 5
	2.0, // Summicron-M 35mm f/2 (IV)		: 6
	2.0, // Summicron-M 90mm f/2 (II)		: 7
	1.0, // Lens Code: 8					: 8
	2.8, // Elmarit-M 135mm f/2.8 (I/II)	: 9
	1.0, // Lens Code: 10					: 10
	1.0, // Lens Code: 11					: 11
	1.0, // Lens Code: 12					: 12
	1.0, // Lens Code: 13					: 13
	1.0, // Lens Code: 14					: 14
	1.0, // Lens Code: 15					: 15
	4.0, // Tri-Elmar-M 16-18-21mm f/4 ASPH.: 16
	1.0, // Lens Code: 17					: 17
	1.0, // Lens Code: 18					: 18
	1.0, // Lens Code: 19					: 19
	1.0, // Lens Code: 20					: 20
	1.0, // Lens Code: 21					: 21
	1.0, // Lens Code: 22					: 22
	2.0, // Summicron-M 50mm f/2 (III)		: 23
	2.8, // Elmarit-M 21mm f/2.8 ASPH.		: 24
	2.8, // Elmarit-M 24mm f/2.8 ASPH.		: 25
	2.0, // Summicron-M 28mm f/2 ASPH.		: 26
	2.8, // Elmarit-M 28mm f/2.8 (IV)		: 27
	2.8, // Elmarit-M 28mm f/2.8 ASPH.		: 28
	1.4, // Summilux-M 35mm f/1.4 ASPH.	: 29
	2.0, // Summicron-M 35mm f/2 ASPH.		: 30
	1.0, // Noctilux-M 50mm f/1			: 31
	1.4, // Summilux-M 50mm f/1.4 ASPH.	: 32
	2.0, // Summicron-M 50mm f/2 (IV, V)	: 33
	2.8, // Elmar-M 50mm f/2.8				: 34
	1.4, // Summilux-M 75mm f/1.4			: 35
	2.0, // Apo-Summicron-M 75mm f/2 ASPH.	: 36
	2.0, // Apo-Summicron-M 90mm f/2 ASPH.	: 37
	2.8, // Elmarit-M 90mm f/2.8			: 38
	4.0, // Macro-Elmar-M 90mm f/4			: 39
	1.0, // Macro-Adapter M				: 40
	1.0, // Lens Code: 41					: 41
	4.0, // Tri-Elmar-M 28-35-50mm f/4 ASPH.: 42
	2.5, // Summarit-M 35mm f/2.5			: 43
	2.5, // Summarit-M 50mm f/2.5			: 44
	2.5, // Summarit-M 75mm f/2.5			: 45
	2.5, // Summarit-M 90mm f/2.5			: 46
	1.4, // Summilux-M 21mm f/1.4			: 47
	1.4, // Summilux-M 24mm f/1.4			: 48
	0.95, // Noctilux 50mm f/0.95			: 49
	3.8, // Elmar-M 24mm f/3.8				: 50
	3.4, // Super-Elmar-M 21mm f/3.4 ASPH.	: 51
	3.8, // Elmar-M 18mm f/3.8 ASPH			: 52
	2.0, // Summicron-M 50mm f/2			: 53
};


dng_makernotes::dng_makernotes(void)
:
	fParent (0) ,	
	fJPEGFileQuality (0xffffffff),
	fSelectedUserProfile (0xffffffff),
	fMakerNoteCameraSerialNumber (0xffffffff),
	
	fWhiteBalancePreset (0xffffffff),
	
	fLensId (0xffffffff),
	
	fFrameLineSelector (0xffffffff),
	
	fBlueDotBrightness (0,0),
	fTTLBrightness (0,0),
	fM9BlueDotBrightness (0xffffffff),
	fM9TTLBrightness (0xffffffff),	
	fEstimatedAperture (0,0),

	fCalculatedAperture (0,0),
	
	fSystemTemperature (0xffffffff),
	fColorTemperature (0xffffffff),
	
	fUVIRCorrection (0xffffffff),
	fRedAsShotNeutral (0,0),
	fGreenAsShotNeutral (0,0),
	fBlueAsShotNeutral (0,0),
	fCCDID (0xffffffff),
	fCCDBoardID (0xffffffff),
	fControllerBoardID (0xffffffff),
	fM16CID (0xffffffff),
	fUniqueImageId (0xffffffff)
{
	clipApertures = false;
}

dng_makernotes::~dng_makernotes()
{
}


bool dng_makernotes::ParseTag (dng_stream &stream,
					   uint32 parentCode,
					   uint32 tagCode,
					   uint32 tagType,
					   uint32 tagCount,
					   uint64 tagOffset)
{
	if (parentCode == tcLeicaMakerNote) {
		switch (tagCode) {
				case tcJPEGFileQuality:
					if ((tagType == ttShort) && (tagCount == 1)) {
						fParent = parentCode;
						fJPEGFileQuality = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcSelectedUserProfile:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fSelectedUserProfile = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcMakerNoteCameraSerialNumber:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fMakerNoteCameraSerialNumber = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcWhiteBalancePreset:
					if ((tagType == ttShort) && (tagCount == 1)) {
						fParent = parentCode;
						fWhiteBalancePreset = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcLensId:
				case tcM9LensId:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fLensId = stream.TagValue_uint32 (tagType);
						fFrameLineSelector = fLensId & 0x3;
						fLensId = fLensId >>2;
					}
					break;
				case tcBlueDotBrightness:
					if ((tagType == ttSRational) && (tagCount == 1)) {
						fParent = parentCode;
						fBlueDotBrightness = stream.TagValue_srational (tagType);
					}
					break;
				case tcTTLBrightness:
					if ((tagType == ttSRational) && (tagCount == 1)) {
						fParent = parentCode;
						fTTLBrightness = stream.TagValue_srational (tagType);
					}
					break;
				case tcM9BlueDotBrightness:
					if ((tagType == ttSLong) && (tagCount == 1)) {
						fParent = parentCode;
						fM9BlueDotBrightness = stream.TagValue_int32 (tagType);
//						printf("fM9BlueDotBrightness %d\n", fM9BlueDotBrightness);
					}
					break;
				case tcM9TTLBrightness:
					if ((tagType == ttSLong) && (tagCount == 1)) {
						fParent = parentCode;
						fM9TTLBrightness = stream.TagValue_int32 (tagType);
//						printf("fM9TTLBrightness %d ", fM9TTLBrightness);
					}
					break;
				case tcEstimatedAperture:
				case tcM9EstimatedAperture:
					if (((tagType == ttSRational) || (tagType == ttRational)) && (tagCount == 1)) {
						fParent = parentCode;
						// TagValue_srational can read uRationals as well......
						fEstimatedAperture = stream.TagValue_srational (tagType);
						real64 tempFStop = pow (2.0, fEstimatedAperture.As_real64() * 0.5);
//						printf("fEstimatedAperture %f ", fEstimatedAperture.As_real64());
						if (tempFStop < 1.0) tempFStop = 1.0;
						if (tempFStop > 64.0) tempFStop = 64.0;
						fEstimatedAperture.Set_real64(2.0*log(tempFStop)/log(2.0), fEstimatedAperture.d);
					}
					break;				 
				case tcSystemTemperature:
					if ((tagType == ttSLong) && (tagCount == 1)) {
						fParent = parentCode;
						fSystemTemperature = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcColorTemperature:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fColorTemperature = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcUVIRCorrection:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fUVIRCorrection = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcRedAsShotNeutral:
					if ((tagType == ttSRational) && (tagCount == 1)) {
						fParent = parentCode;
						fRedAsShotNeutral = stream.TagValue_srational (tagType);
					}
					break;
				case tcGreenAsShotNeutral:
					if ((tagType == ttSRational) && (tagCount == 1)) {
						fParent = parentCode;
						fGreenAsShotNeutral = stream.TagValue_srational (tagType);
					}
					break;
				case tcBlueAsShotNeutral:
					if ((tagType == ttSRational) && (tagCount == 1)) {
						fParent = parentCode;
						fBlueAsShotNeutral = stream.TagValue_srational (tagType);
					}
					break;
				case tcCCDID:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fCCDID = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcCCDBoardID:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fCCDBoardID = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcControllerBoardID:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fControllerBoardID = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcM16CID:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fM16CID = stream.TagValue_uint32 (tagType);
					}
					break;
				case tcUniqueImageId:
					if ((tagType == ttLong) && (tagCount == 1)) {
						fParent = parentCode;
						fUniqueImageId = stream.TagValue_uint32 (tagType);
					}
					break;

				default:
					return false;
					break;
		}
		return true;
	}
	return false;
}
					   
void dng_makernotes::PostParse ()
{
	int32 d = 0x10000;
	if (fEstimatedAperture.IsValid()) {
		d = fEstimatedAperture.d;
	}

	// Now estimate a best fit aperture:
	if (fBlueDotBrightness.IsValid() && fTTLBrightness.IsValid()) {
		fCalculatedAperture.Set_real64(floor(((fBlueDotBrightness.As_real64() - fTTLBrightness.As_real64() + 4.35)*2.0) + 0.5)/2.0, d);
		// Ensure its slightly sane......
		real64 tempFStop = pow (2.0, fEstimatedAperture.As_real64() * 0.5);
		if (tempFStop < 1.0) fEstimatedAperture.Set_real64(2.0*log(1.0)/log(2.0), d);
		else if (tempFStop > 32.0) fEstimatedAperture.Set_real64(2.0*log(32.0)/log(2.0), d);
	}
	
	if ((fLensId != 0xffffffff) && (fLensId < MAX_LEICA_LENSES) && clipApertures) {

		if (fEstimatedAperture.IsValid()) {
			// First limit the leica estimated aperture.
			real64 tempFStop = pow (2.0, fEstimatedAperture.As_real64() * 0.5);
			if (tempFStop < LeicaMinFStop[fLensId]) fEstimatedAperture.Set_real64(2.0*log(LeicaMinFStop[fLensId])/log(2.0), d);
			else if (tempFStop > LeicaMaxFStop[fLensId]) fEstimatedAperture.Set_real64(2.0*log(LeicaMaxFStop[fLensId])/log(2.0), d);
		}
		if (fCalculatedAperture.IsValid()) {
			// Then the calculated aperture.
			real64 tempFStop = pow (2.0, fCalculatedAperture.As_real64() * 0.5);
			if (tempFStop < LeicaMinFStop[fLensId]) fCalculatedAperture.Set_real64(2.0*log(LeicaMinFStop[fLensId])/log(2.0), d);
			else if (tempFStop > LeicaMaxFStop[fLensId]) fCalculatedAperture.Set_real64(2.0*log(LeicaMaxFStop[fLensId])/log(2.0), d);
		}		
	}
}

bool dng_makernotes::IsValidDNG (dng_shared &shared,
						 uint32 parentCode)

{
	return true;
}

const char * dng_makernotes::GetLensName()
{

	if (fLensId != 0xffffffff) {	
		if (fLensId < MAX_LEICA_LENSES) {
			return LeicaLensNames[fFrameLineSelector][fLensId];
		}
		else {
			#if qWinOS
			sprintf_s(buf, BUF_LENGTH, "Code %u", fLensId);
			#else
			sprintf(buf, "Code %u", fLensId);
			#endif
			return buf;
		}
	}
	return "";
}

void dng_makernotes::setClipApertures(bool val)
{
	clipApertures = val;
}


void dng_makernotes::DumpMakerNotes()
{

	if (fParent == tcLeicaMakerNote) {
		dng_local_printf::printf ("\nLeica MakerNotes\n");
		if (fSelectedUserProfile != 0xffffffff) {
			dng_local_printf::printf ("  Selected user profile: %lu\n", fSelectedUserProfile);
		}
		if (fMakerNoteCameraSerialNumber != 0xffffffff) {
			dng_local_printf::printf ("  Camera Serial Number: %lu\n", fMakerNoteCameraSerialNumber);
		}
		if (fWhiteBalancePreset != 0xffffffff) {			
			dng_local_printf::printf ("  White Balance Preset: ");
			if (fWhiteBalancePreset == 0) dng_local_printf::printf ("Auto or manual\n");
			else if (fWhiteBalancePreset == 1) dng_local_printf::printf ("Daylight\n");
			else if (fWhiteBalancePreset == 2) dng_local_printf::printf ("Fluorescent\n");
			else if (fWhiteBalancePreset == 3) dng_local_printf::printf ("Tungsten\n");
			else if (fWhiteBalancePreset == 4) dng_local_printf::printf ("Flash\n");
			else if (fWhiteBalancePreset == 10) dng_local_printf::printf ("Cloudy\n");
			else if (fWhiteBalancePreset == 11) dng_local_printf::printf ("Shadow\n");
			else dng_local_printf::printf ("  %lu K\n", (fWhiteBalancePreset & 0x7fff));
		}
		if (fLensId != 0xffffffff) {			
			dng_local_printf::printf ("  Lens Id: %s\n", GetLensName());
		}
		if (fFrameLineSelector != 0xffffffff) {			
			dng_local_printf::printf ("  Frame Selector Position: ");
			if (fFrameLineSelector == 0) dng_local_printf::printf ("Uncoded/Not detected\n");
			else if (fFrameLineSelector == 1) dng_local_printf::printf ("28/90mm frame lines engaged\n");
			else if (fFrameLineSelector == 2) dng_local_printf::printf ("24/35mm frame lines engaged\n");
			else if (fFrameLineSelector == 3) dng_local_printf::printf ("50/75mm frame lines engaged\n");
		}
		if (fBlueDotBrightness.IsValid()) {
			dng_local_printf::printf ("  M8 Blue Dot Brightness: %lf\n", fBlueDotBrightness.As_real64());
		}
		if (fTTLBrightness.IsValid()) {
			dng_local_printf::printf ("  M8 TTL Brightness: %lf\n", fTTLBrightness.As_real64());
		}
		if (fM9BlueDotBrightness != 0xffffffff) {
			dng_local_printf::printf ("  M9 Blue Dot Brightness: %d\n", fM9BlueDotBrightness);
		}
		if (fM9TTLBrightness != 0xffffffff) {
			dng_local_printf::printf ("  M9 TTL Brightness: %d\n", fM9TTLBrightness);
		}
		if (fEstimatedAperture.IsValid()) {
			dng_local_printf::printf ("  Leica Estimated Aperture: f/%.1lf\n", pow (2.0, fEstimatedAperture.As_real64() * 0.5));
		}
		if (fCalculatedAperture.IsValid()) {
			dng_local_printf::printf ("  CornerFix Calculated Aperture: f/%.1lf\n", pow (2.0, fCalculatedAperture.As_real64() * 0.5));
		}
		if (fSystemTemperature != 0xffffffff) {
			dng_local_printf::printf ("  System temperature: %lu C\n", fSystemTemperature);
		}
		if (fColorTemperature != 0xffffffff) {
			if (fColorTemperature == 0) dng_local_printf::printf ("  Color temperature: Not Recorded\n");
			else dng_local_printf::printf ("  Color temperature: %lu K\n", fColorTemperature);
		}
		if (fUVIRCorrection != 0xffffffff) {			
			dng_local_printf::printf ("  UVIR Correction: ");
			if (fUVIRCorrection == 0) dng_local_printf::printf ("Not Active\n");
			else if (fUVIRCorrection == 1) dng_local_printf::printf ("Active\n");
		}
		if (fCCDID != 0xffffffff) {
			dng_local_printf::printf ("  CCD Sensor Id(?): %lu\n", fCCDID);
		}
		if (fCCDBoardID != 0xffffffff) {
			dng_local_printf::printf ("  CCD Board Id(?): %lu\n", fCCDBoardID);
		}
		if (fControllerBoardID != 0xffffffff) {
			dng_local_printf::printf ("  Controller Board Id(?): %lu\n", fControllerBoardID);
		}
		if (fM16CID != 0xffffffff) {
			dng_local_printf::printf ("  M16C micro-controller Id(?): %lu\n", fM16CID);
		}
		if (fUniqueImageId != 0xffffffff) {
			dng_local_printf::printf ("  Unique Image Id: %lu\n", fUniqueImageId);
		}
		dng_local_printf::printf ("\n");
	}
	else {
		dng_local_printf::printf ("No decodable MakerNotes found\n");
	}

}
