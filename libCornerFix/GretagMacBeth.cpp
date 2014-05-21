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
 * GretagMacBeth.cpp
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 12 Jan 2008 : 0.9.2.0
 * 17 Sept 2009 : 1.3.0.0
 * 22 Oct 2009 : 1.3.0.2
 *
 */

#include "GretagMacBeth.h"
#include "dng_matrix.h"
#include "dng_camera_profile.h"



static const double gretagLABD50[NUMGretagPatches+1][3] = {
	{  37.986, 13.555, 14.059 },	//  CC1 D50
	{  65.711, 18.13, 17.81 },		//  CC2 D50
	{  49.927, -4.88, -21.925 },	//  CC3 D50
	{  43.139, -13.095, 21.905 },	//  CC4 D50
	{  55.112, 8.844, -25.399 },	//  CC5 D50
	{  70.719, -33.397, -0.199 },	//  CC6 D50
	{  62.661, 36.067, 57.096 },	//  CC7 D50
	{  40.02, 10.41, -45.964 },		//  CC8 D50
	{  51.124, 48.239, 16.248 },	//  CC9 D50
	{  30.325, 22.976, -21.587 },	//  CC10 D50
	{  72.532, -23.709, 57.255 },	//  CC11 D50
	{  71.941, 19.363, 67.857 },	//  CC12 D50
	{  28.778, 14.179, -50.297 },	//  CC13 D50
	{  55.261, -38.342, 31.37 },	//  CC14 D50
	{  42.101, 53.378, 28.19 },		//  CC15 D50
	{  81.733, 4.039, 79.819 },		//  CC16 D50
	{  51.935, 49.986, -14.574 },	//  CC17 D50
	{  51.038, -28.631, -28.638 },	//  CC18 D50
	{  96.539, -0.425, 1.186 },		//  CC19 D50
	{  81.257, -0.638, -0.335 },	//  CC20 D50
	{  66.766, -0.734, -0.504 },	//  CC21 D50
	{  50.867, -0.153, -0.27 },		//  CC22 D50
	{  35.656, -0.421, -1.231 },	//  CC23 D50
//	{  20.461, -0.079, -0.973 },	//  CC24 D50
	{  50.461, -0.079, -0.973 },	//  CC24 D50
	{  23.000, -0.079, -0.973 }		//  Black surround D50
};

static const double stepLABD50[NUMGretagPatches+1][3] = {
{ 96.539, 0.0, 0.0 },			//  CC1 D50,
{ 93.23126087, 0.0, 0.0 },		//  CC2 D50
{ 89.92352174, 0.0, 0.0 },		//  CC3 D50,
{ 86.61578261, 0.0, 0.0 },		//  CC4 D50,
{ 83.30804348, 0.0, 0.0 },		//  CC5 D50,
{ 80.00030435, 0.0, 0.0 },		//  CC6 D50,
{ 76.69256522, 0.0, 0.0 },		//  CC7 D50,
{ 73.38482609, 0.0, 0.0 },		//  CC8 D50
{ 70.07708696, 0.0, 0.0 },		//  CC9 D50,
{ 66.76934783, 0.0, 0.0 },		//  CC10 D50,
{ 63.4616087, 0.0, 0.0 },		//  CC11 D50,
{ 60.15386957, 0.0, 0.0 },		//  CC12 D50,
{ 56.84613043, 0.0, 0.0 },		//  CC13 D50,
{ 53.5383913, 0.0, 0.0 },		//  CC14 D50,
{ 50.23065217, 0.0, 0.0 },		//  CC15 D50
{ 46.92291304, 0.0, 0.0 },		//  CC16 D50
{ 43.61517391, 0.0, 0.0 },		//  CC17 D50,
{ 40.30743478, 0.0, 0.0 },		//  CC18 D50,
{ 36.99969565, 0.0, 0.0 },		//  CC19 D50
{ 33.69195652, 0.0, 0.0 },		//  CC20 D50,
{ 30.38421739, 0.0, 0.0 },		//  CC21 D50,
{ 27.07647826, 0.0, 0.0 },		//  CC22 D50
{ 23.76873913, 0.0, 0.0 },		//  CC23 D50,
{ 20.461, 0.0, 0.0 },			//  CC24 D50,
{ 23.000, 0.0, 0.0 }			//  Black surround D50
};


GretagMacBeth::GretagMacBeth( const dng_negative &negative)
:	fNegative (negative)
{
	// Compute camera space to linear ProPhoto RGB parameters.
	dng_matrix fCameraToRGB;	
	{
		dng_camera_profile_id profileID;	// Default profile ID.
		AutoPtr<dng_color_spec> spec (fNegative.MakeColorSpec (profileID));
		if (fNegative.HasCameraNeutral ()) {
			spec->SetWhiteXY (spec->NeutralToXY (fNegative.CameraNeutral ()));
		}
		else if (fNegative.HasCameraWhiteXY ()) {
			spec->SetWhiteXY (fNegative.CameraWhiteXY ());
		}
		else {
			spec->SetWhiteXY (D55_xy_coord ());
		}

		// This where things get interesting; the DNG docs, on the face of it, say that the XYZ to camera
		// matrix is G.C.M where G is the AutoBalance, C the camera calibration matrix, and M the ColorMatrix
		// So something like the following should work:

		//	dng_matrix xyzToCamera;
		//	dng_matrix reduction;		
		//	spec->FindXYZtoCamera (spec->WhiteXY (), xyzToCamera, reduction);

		// Of course, it doesn't - the reason is that whitebalance hasn't been taken into account
		// You have to adjust the matrix to take the camera's AsShotNeutral into account, together with the
		// reference illuminant.
		// Rather than go throught that whole process, the easiest thing to do is just to invert 
		// the CameraToPCS matrix......

		dng_matrix xyzToCamera = Invert(spec->CameraToPCS());
		
		for (int i = 0; i < NUMGretagPatches+1; i++) {
			dng_vector_3 lab;
			// This selects whether we have a Gretag chart or a grey step level chart
			if (true) {
				lab = dng_vector_3(gretagLABD50[i][0], gretagLABD50[i][1], gretagLABD50[i][2]);
			}
			else {
				lab = dng_vector_3(stepLABD50[i][0], stepLABD50[i][1], stepLABD50[i][2]);
			}
			dng_vector_3 xyzVal;
			labToXYZ(&lab, &xyzVal);
			patchRGBs[i] = xyzToCamera*xyzVal;
//			dng_vector test;
//			test = (dng_space_ProPhoto::Get ().MatrixFromPCS () * spec->CameraToPCS())*patchRGBs[i];
		}		
	}
}


void GretagMacBeth::labToXYZ(dng_vector *lab, dng_vector *XYZ)
{
  double e = 216.0/24389.0;             // 0.0088565
  double k = 24389.0/27.0;              // 903.30
  double l = (*lab)[0];
  double a = (*lab)[1];
  double b = (*lab)[2];

  double Yr;
  if (l > k*e) {
	  Yr = pow((l+16.0)/116.0,3);
  }
  else {
	  Yr = l/k;
  }
  double fy;
  if (Yr > e) {
	  fy = (l+16.0)/116.0;
  }
  else {
	  fy = (k*Yr +16.0)/116.0;
  }
  double fx = a/500.0 + fy;
  double fz = fy - b/200.0;
  double fx3 = pow(fx, 3);
  double fz3 = pow(fz, 3);
  double Xr;
  if (fx3 > e) {
	  Xr = fx3;
  }
  else {
	  Xr = (116.0*fx - 16.0)/k;
  }
  double Zr;
  if (fz3 > e) {
	  Zr = fz3;
  }
  else {
	  Zr = (116.0*fz - 16.0)/k;
  }
  dng_vector D50white = XYtoXYZ (D50_xy_coord());
  (*XYZ)[0] = Xr*D50white[0];
  (*XYZ)[1] = Yr*D50white[1];
  (*XYZ)[2] = Zr*D50white[2];
}

dng_vector_3 GretagMacBeth::getpatchRGB(int index)
{
	return patchRGBs[index];
}


GretagMacBeth::~GretagMacBeth(void)
{
}
