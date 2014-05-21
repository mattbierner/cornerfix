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
 * dng_lmfit_functions.cpp
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

#include "dng_lmfit_functions.h"
#include "math.h"

dng_lmfit_functions::dng_lmfit_functions(void)
{
	equationType = 99;
	for (uint32 i = 0; i < kmaxEquations; i++) {
		for (uint32 j = 0; j < kmaxParamters; j++) {
			p[i][j] = 0.00001;
		}
	}
	luminanceTweak = 1.0;
	chromaTweak = 1.0;
	forceAsymetry = 1.0;
	tweakOn = false;
	centerX = 0;
	centerY = 0;
	correctionType = normalCorrection;

}

dng_lmfit_functions::~dng_lmfit_functions(void)
{
}

void dng_lmfit_functions::setEquationType(uint32 type) 
{

		equationType = type;

}

uint32 dng_lmfit_functions::getEquationType() 
{

		return equationType;

}

real64 *dng_lmfit_functions::getP()
{

	return p[equationType];

}

uint32 dng_lmfit_functions::getNumEquations()
{

	return kmaxEquations;

}

bool dng_lmfit_functions::isPolynomial() {

	if (equationType < 17) return true;
	else return false;
}


uint32 dng_lmfit_functions::getParNumber() 
{

	switch (equationType) {
		case 0: return 2;
			break;
		case 1: return 3;
			break;
		case 2: return 4;
			break;
		case 3: return 5;
			break;
		case 4: return 6;
			break;
		case 5: return 7;
			break;
		case 6: return 8;
			break;
		case 7: return 3;
			break;
		case 8: return 4;
			break;
		case 9: return 5;
			break;
		case 10: return 6;
			break;
		case 11: return 7;
			break;
		case 12: return 8;
			break;
		case 13: return 9;
			break;
		case 14: return 10;
			break;
		case 15: return 11;
			break;
		case 16: return 12;
			break;
		case 17: return 2;
			break;
		case 18: return 2;
			break;
		case 19: return 3;
			break;
		case 20: return 3;
			break;
		case 21: return 3;
			break;

		default: return 3;
			break;
	}

}


real64 dng_lmfit_functions::my_fit_function( real64 t, real64* p )
{
real64 retVal = 0;

	switch (equationType) {
		case 0:
			retVal = p[0] + p[1]*pow(t,2);
			break;
		case 1:
			retVal = p[0] + p[1]*pow(t,2) + p[2]*pow(t,4);
			break;
		case 2:
			retVal = p[0] + p[1]*pow(t,2) + p[2]*pow(t,4) + p[3]*pow(t,6);
			break;
		case 3:
			retVal = p[0] + p[1]*pow(t,2) + p[2]*pow(t,4) + p[3]*pow(t,6) + p[4]*pow(t,8);
			break;
		case 4:
			retVal = p[0] + p[1]*pow(t,2) + p[2]*pow(t,4) + p[3]*pow(t,6) + p[4]*pow(t,8) + p[5]*pow(t,10);
			break;
		case 5:
			retVal = p[0] + p[1]*pow(t,2) + p[2]*pow(t,4) + p[3]*pow(t,6) + p[4]*pow(t,8) + p[5]*pow(t,10) + p[6]*pow(t,12);
			break;
		case 6:
			retVal = p[0] + p[1]*pow(t,2) + p[2]*pow(t,4) + p[3]*pow(t,6) + p[4]*pow(t,8) + p[5]*pow(t,10) + p[6]*pow(t,12) + p[7]*pow(t,14);
			break;
		case 7:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2);
			break;
		case 8:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3);
			break;
		case 9:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3) + p[4]*pow(t,4);
			break;
		case 10:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3) + p[4]*pow(t,4) + p[5]*pow(t,5);
			break;
		case 11:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3) + p[4]*pow(t,4) + p[5]*pow(t,5) + p[6]*pow(t,6);
			break;
		case 12:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3) + p[4]*pow(t,4) + p[5]*pow(t,5) + p[6]*pow(t,6) + p[7]*pow(t,7);
			break;
		case 13:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3) + p[4]*pow(t,4) + p[5]*pow(t,5) + p[6]*pow(t,6) + p[7]*pow(t,7) + p[8]*pow(t,8);
			break;
		case 14:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3) + p[4]*pow(t,4) + p[5]*pow(t,5) + p[6]*pow(t,6) + p[7]*pow(t,7) + p[8]*pow(t,8) + p[9]*pow(t,9);
			break;
		case 15:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3) + p[4]*pow(t,4) + p[5]*pow(t,5) + p[6]*pow(t,6) + p[7]*pow(t,7) + p[8]*pow(t,8) + p[9]*pow(t,9) + p[10]*pow(t,10);
			break;
		case 16:
			retVal = p[0] + p[1]*t + p[2]*pow(t,2) + p[3]*pow(t,3) + p[4]*pow(t,4) + p[5]*pow(t,5) + p[6]*pow(t,6) + p[7]*pow(t,7) + p[8]*pow(t,8) + p[9]*pow(t,9) + p[10]*pow(t,10) + p[11]*pow(t,11);
			break;
		case 17:
			retVal = p[0] + pow(cos(p[1]*t), 4);
			break;
		case 18:
			retVal = p[0] + pow(cos(p[1]*t), 5);
			break;
		case 19:
			retVal = p[0] + pow(cos(p[1]*t), 3)*pow(cos(p[2]*t), 2);
			break;
		case 20:
			retVal = p[0] + pow(cos(p[1]*t), 4)*pow(cos(p[2]*t), 1);
			break;
		case 21:
			retVal = p[0] + pow(cos(p[1]*t), 4)*pow(cos(p[2]*t), 2);
			break;

		default:
			 retVal = ( p[0]*t + (1-p[0]+p[1]+p[2])*t*t ) / ( 1 + p[1]*t + p[2]*t*t );
			break;
	}

	return retVal;

}

real64 dng_lmfit_functions::evalSelected(real64 v)
{

	return my_fit_function( v, p[equationType] );

}

void dng_lmfit_functions::updateFactors()
{
	tweakK = 1 - chromaTweak - luminanceTweak + chromaTweak*luminanceTweak;
	tweakcDIVr = chromaTweak * (1.0 - luminanceTweak);
	tweakR = luminanceTweak * (1.0 - chromaTweak);
	tweakC = chromaTweak*luminanceTweak;
}

real64 dng_lmfit_functions::tweakFactors(real64 c, real64 r)
{
	real64 s = tweakK + tweakcDIVr*c/r + tweakR*r + tweakC*c;
	if (s < 0.00000001) {
		return 0.00000001;
	}
	else {
		return s;
	}

}


real64 dng_lmfit_functions::evalTweaked(real64 v) 
{
	switch (getCorrectionType()) {
		case normalCorrection:
			if (tweakOn)
			{
				return tweakFactors(my_fit_function( v, p[equationType] ), refFunction->evalSelected(v));
			}
			else 
			{
				return my_fit_function( v, p[equationType] );
			}
			break;
		case differentialCorrection:
			if (this == refFunction) {
				return my_fit_function( v, p[equationType] );
			}
			else {
				if (v < -2990.0) printf("\nevalTweaked: %f %f %f", v, refFunction->evalSelected(v), my_fit_function( v, p[equationType] ));
				return refFunction->evalSelected(v) / my_fit_function( v, p[equationType] );
			}
		default:
			return 0.0;
		}	
}



void dng_lmfit_functions::setLuminanceTweak(real64 tweak)
{
	luminanceTweak = tweak;
	updateFactors();
}

void dng_lmfit_functions::setChromaTweak(real64 tweak)
{
	chromaTweak = tweak;
	updateFactors();
}

real64 dng_lmfit_functions::getLuminanceTweak()
{
	return luminanceTweak;
}

real64 dng_lmfit_functions::getChromaTweak()
{
	return chromaTweak;
}

void dng_lmfit_functions::setAsymetry(real64 tweak)
{
	forceAsymetry = tweak;
	updateFactors();
}

real64 dng_lmfit_functions::getAsymetry()
{
	return forceAsymetry;
}

void dng_lmfit_functions::setTweakOn(bool val)
{
	tweakOn = val;
}

bool dng_lmfit_functions::getTweakOn()
{
	return tweakOn;
}

void dng_lmfit_functions::setRefFunction(dng_lmfit_functions *function)
{
#ifdef _DEBUG
	if (function == NULL) {
		printf("Refernce function is NULL");
	}
#endif
	refFunction = function;
}


void dng_lmfit_functions::setCenterX(int32 val)
{
	centerX = val;
}

void dng_lmfit_functions::setCenterY(int32 val)
{
	centerY = val;
}

int32 dng_lmfit_functions::getCenterX()
{
	return centerX;
}

int32 dng_lmfit_functions::getCenterY()
{
	return centerY;
}	

int32 dng_lmfit_functions::getCorrectionType() {
	return correctionType;
}

void dng_lmfit_functions::normalise()
{
	if (isPolynomial()) {
		for (int32 i = (int32) getNumEquations() - 1; i >= 0; i--) {
			p[getEquationType()][i] = p[getEquationType()][i]/p[getEquationType()][0];
		}
	}
}



