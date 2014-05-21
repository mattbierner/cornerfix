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
 * dng_lmfit.h
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Argonne National Laboratories;
 * Contributor(s):   s. garbow, kenneth e. hillstrom, jorge j. more;
 *                   Steve Moshier, Joachim Wuttke, Sandy McGuffog
 *
 * Changes
 * -------
 * 01 Jun 2007 : Original version for CornerFix;
 * 12 Sept 2009 : 1.2.0.0
 * 4 July 2011 : 1.4.2.0
 *
 */

/*
 * This is an implentation of lmfit
 * The original is from public domain Fortran version
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

#pragma once
#include "dng_lmfit_functions.h"

// parameters for calling the high-level interface lmfit
//   ( lmfit.c provides lm_initialize_control which sets default values ):
typedef struct {
        real64 ftol;       // relative error desired in the sum of squares.
        real64 xtol;       // relative error between last two approximations.
        real64 gtol;       // orthogonality desired between fvec and its derivs.
        real64 epsilon;    // step used to calculate the jacobian.
        real64 stepbound;  // initial bound to steps in the outer loop.
        real64 fnorm;      // norm of the residue vector fvec.
        int maxcall;       // maximum number of iterations.
        int nfev;          // actual number of iterations.
        int info;          // status of minimization.
} lm_control_type;

typedef struct {
        real64* user_t;
        real64* user_y;
        real64 (*user_func)( real64 user_t_point, real64* par );
} lm_data_type;

// the subroutine that calculates fvec:
typedef void (lm_evaluate_ftype) (
    real64* par, int m_dat, real64* fvec, void *data, int *info );

// the subroutine that informs about fit progress:
typedef void (lm_print_ftype) (
    int n_par, real64* par, int m_dat, real64* fvec, void *data,
    int iflag, int iter, int nfev );

class dng_lmfit
{
public:
	dng_lmfit(void);
	~dng_lmfit(void);

// compact high-level interface:
void lm_initialize_control( lm_control_type *control );
void lm_minimize ( int m_dat, int n_par, real64* par,
                   dng_lmfit_functions *data, lm_control_type *control );
real64 lm_enorm( int, real64* );

const char * lm_get_infmsg(int i);

const char * lm_get_shortmsg(int i);

private:


// default implementation therof, provided by lm_eval.c:
void lm_evaluate_default (
    real64* par, int m_dat, real64* fvec, dng_lmfit_functions *data, int *info );


// default implementation therof, provided by lm_eval.c:
void lm_print_default (
    int n_par, real64* par, int m_dat, real64* fvec, dng_lmfit_functions *data,
    int iflag, int iter, int nfev );



// low-level interface for full control:
void lm_lmdif( int m, int n, real64* x, real64* fvec, real64 ftol, real64 xtol,
               real64 gtol, int maxfev, real64 epsfcn, real64* diag, int mode,
               real64 factor, int *info, int *nfev, 
               real64* fjac, int* ipvt, real64* qtf,
               real64* wa1, real64* wa2, real64* wa3, real64* wa4,
               dng_lmfit_functions *data );


#ifndef _LMDIF
//extern char *lm_infmsg[];
//extern char *lm_shortmsg[];
#endif

//lm_eval.h



//void lm_evaluate_default( real64* par, int m_dat, real64* fvec, void *data, int *info );

//void lm_print_default( int n_par, real64* par, int m_dat, real64* fvec, void *data, int iflag, int iter, int nfev );

void lm_lmpar(int n, real64* r, int ldr, int* ipvt, real64* diag, real64* qtb,
              real64 delta, real64* par, real64* x, real64* sdiag,
              real64* wa1, real64* wa2);
			  
void lm_qrfac(int m, int n, real64* a, int pivot, int* ipvt,
           real64* rdiag, real64* acnorm, real64* wa);

void lm_qrsolv(int n, real64* r, int ldr, int* ipvt, real64* diag,
              real64* qtb, real64* x, real64* sdiag, real64* wa);
 

};
