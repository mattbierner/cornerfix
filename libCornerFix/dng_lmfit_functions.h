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
 * dng_lmfit_functions.h
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   ;
 *
 * Changes
 * -------
 * 01 Jun 2007 : Original version for CornerFix;
 * 18 Jun 2007 : 0.9.0.5
 * 12 Sept 2009 : 1.2.0.0
 * 14 Oct 2009 : 1.3.0.0
 * 9 Nov 2009 : 1.3.0.3
 * 28 Nov 2009 : 1.3.0.4
 *
 */

/*
 * This is an implentation of functions for fitting with lmfit
 * The original lmfit is from public domain Fortran version
 * of Argonne National Laboratories MINPACK
 * argonne national laboratory. minpack project. march 1980.
 * burton s. garbow, kenneth e. hillstrom, jorge j. more
 * C translation by Steve Moshier
 * Joachim Wuttke converted the source into C++ compatible ANSI style
 * and provided a simplified interface
 * This version has been given a rudimentary class structure,
 * and some (void) * void typeparameters replaced with more type 
 * safe constructs
 */

/*
 * This file defines the equations that CornerFix uses to define vignetting....
 */

#pragma once
#include "dng_types.h"


enum localSegmentDefs {
	kLeftHorizonalSegment = 0,
	kRightHorizonalSegment,
	kTopVerticalSegment,
	kBottomVerticalSegment,
	kTopLeftDiagonalSegment,
	kBottomRightDiagonalSegment,
	kTopRightDiagonalSegment,
	kBottomLeftDiagonalSegment,
	kMaxSegments,
	kAllSegments
};

// Quadrants are used by the line processor
// Numbering is clockwise from 9 o'clock
enum localDiagonalOctants {
	kLeftHorizonalOctant = 0,
	kTopLeftDiagonalOctant,
	kTopVerticalOctant,
	kTopRightDiagonalOctant,
	kRightHorizonalOctant,
	kBottomRightDiagonalOctant,
	kBottomVerticalOctant,
	kBottomLeftDiagonalOctant,
	kMaxOctants
};

enum correctionData {
	referenceBayerColor = 3,
	normalCorrection = 0,
	differentialCorrection
};

class dng_lmfit_functions
{
public:
	dng_lmfit_functions(void);
	~dng_lmfit_functions(void);

	enum
		{
		
		// constants.
		
		kmaxEquations = 22,
		kmaxParamters = 12,
		
		};




	real64* user_t;
    real64* user_y;
	uint32 equationType;
	real64 p[kmaxEquations][kmaxParamters];
	real64 luminanceTweak;
	real64 chromaTweak;
	real64 tweakK;
	real64 tweakcDIVr;
	real64 tweakR;
	real64 tweakC;
	real64 forceAsymetry;
	bool tweakOn;
	dng_lmfit_functions *refFunction;
	int32 centerX;
	int32 centerY;
	int32 correctionType;

	virtual real64 my_fit_function( real64 t, real64* p );
	virtual void setEquationType(uint32 type);
	virtual uint32 getEquationType();
	bool isPolynomial();
	uint32 getParNumber();
	uint32 getNumEquations();
	real64 *getP();
	real64 evalSelected(real64 v);
	real64 evalTweaked(real64 v);
	void setLuminanceTweak(real64 tweak);
	void setChromaTweak(real64 tweak);
	real64 getLuminanceTweak();
	real64 getAsymetry();
	void setAsymetry(real64 tweak);
	real64 getChromaTweak();
	void setTweakOn(bool val);
	bool getTweakOn();
	real64 tweakFactors(real64 c, real64 r);
	void updateFactors();
	void setRefFunction(dng_lmfit_functions *function);
	void setCenterX(int32 val);
	void setCenterY(int32 val);
	int32 getCenterX();
	int32 getCenterY();	
	int32 getCorrectionType();
	void normalise();

};
