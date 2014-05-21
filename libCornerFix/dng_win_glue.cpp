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
 * dng_win_glue.cpp
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 01 Jun 2007 : Original version;
 * 18 Jun 2007 : 0.9.0.5
 * 22 Jun 2007 : 0.9.0.6
 * 1 Jul 2007 : 0.9.1.0
 * 1 Sept 2007 : 0.9.1.0 Mac version
 * 5 Sept 2007 : 0.9.1.2 - DMR Hack
 * 15 Sept 2007 : 0.9.1.3
 * 23 Oct 2007 : 0.9.1.5
 * 29 Nov 2007 : 0.9.1.7
 * 20 Dec 2007 : 0.9.1.8
 * 22 Dec 2007 : 0.9.1.9
 * 12 Jan 2008 : 0.9.2.0
 * 1 Sept 2009 : 1.1.1.0
 * 12 Sept 2009 : 1.2.0.0
 * 19 Sept 2009 : 1.3.0.0
 * 9 Nov 2009 : 1.3.0.3
 * 21 Nov 2009 : 1.3.0.4
 * 9 Mar 2010 : 1.3.0.6
 * 29 Mar 2010 : 1.3.0.7
 * 17 May 2010 : 1.3.0.8
 * 11 Sept 2010 : 1.3.0.9
 * 6 October 2010 : 1.4.0.0
 * 22 October 2010 : 1.4.0.2
 * 26 October 2010 : 1.4.0.3
 * 23 January 2011 : 1.4.1.0
 * 4 July 2011 : 1.4.2.0
 * 8 July 2012 : 1.4.2.1
 * 31 July 2012 : 1.4.2.2
 * 17 October 2013 : 1.4.2.5
 * 4 December 2013 : 1.4.2.6
 *
 */

/*
 * This is the primary point of conversion from Windows or OS X
 * land into DNG land, and manages a managed/unmanaged code
 * transition.......
 * This based on T. Knoll's original dng_validate code
 */

//#define DMR_HACK

#if qWinOS
#include "StdAfx.h"
#else
#endif
#include "dng_win_glue.h"
#include "CornerFixVersion.h"
#include "dng_devignette_negative.h"
#include "dng_camera_profile.h"
#include "dng_preview.h"



#if qWinOS
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
#endif

dng_win_glue::dng_win_glue(void)
{
	gMathDataType = ttShort;
	
	gFourColorBayer = false;
	
	gMosaicPlane = -1;
	
	gMinStage3Size = 0;
	
	gFinalPixelType = ttByte;
	doIlluminant = true;
	gFinalSpace = &dng_space_sRGB::Get ();
	deVignetteInfo = new M8_devignette ();
	cpfState = kcpfStateNone;
	buildCPF = false;
	multiple = false;
	buildHires = true;
	exifUpdate = false;
	clipApertures = false;
	calculateApertures = true;
	maximizeResolution = true;
    overwriteCameraName = false;
	compressTag = ccUncompressed;
}

void dng_win_glue::SetAntiAlias(int strength)
{
	deVignetteInfo->SetAntiAlias(strength);
}

void dng_win_glue::setAsymetry(real64 strength)
{
	deVignetteInfo->SetAsymetry(strength);
}

void dng_win_glue::SetChromaStrength(int strength)
{
	chromaStrength = strength;
}

void dng_win_glue::SetLumaStrength(int strength)
{
	lumaStrength = strength;
}

#if qWinOS
void dng_win_glue::setImage(String ^file)
#else
void dng_win_glue::setImage(NSString *file)
#endif
{
	gImageDNG.setFromString(file);
}

#if qWinOS
void dng_win_glue::setOutputFile(String ^file)
#else
void dng_win_glue::setOutputFile(NSString *file)
#endif
{
	gDumpDNG.setFromString(file);
}

#if qWinOS
void dng_win_glue::setCSVFile(String ^file)
#else
void dng_win_glue::setCSVFile(NSString *file)
#endif
{
	gDumpCSV.setFromString(file);
}

#if qWinOS
void dng_win_glue::setTIFFile(String ^file)
#else
void dng_win_glue::setTIFFile(NSString *file)
#endif
{
	gDumpTIF.setFromString(file);
}

void dng_win_glue::setBuildCPF()
{
	buildCPF = true;
}

void dng_win_glue::setVerbose(bool val)
{
	gVerbose = val;
}

void dng_win_glue::setCompress(bool val)
{
	buildHires = !val;
}

void dng_win_glue::setMultiple(bool val)
{
	multiple = val;
}

void dng_win_glue::setLossless(bool val)
{
	if (val)
	{
		compressTag = ccJPEG;
	}
	else
	{
		compressTag = ccUncompressed;
	}
}

void dng_win_glue::setDisableIlluminant(bool val)
{
	doIlluminant = !val;
}

void dng_win_glue::setExifUpdate(bool val)
{
	exifUpdate = val;
}

void dng_win_glue::setCalculateAperture(bool val)
{
	calculateApertures = val;
}

void dng_win_glue::setClipApertures(bool val)
{
	clipApertures = val;
}

void dng_win_glue::setMaximizeResolution(bool val)
{
	maximizeResolution = val;
}

void dng_win_glue::setOverwriteCameraName(bool val)
{
	overwriteCameraName = val;
}

int dng_win_glue::getCPFState()
{
	return cpfState;
}

void dng_win_glue::resetCPF()
{
	cpfState = kcpfStateNone;
}


#if qWinOS
bool dng_win_glue::setCPFFile(String ^file)
#else
bool dng_win_glue::setCPFFile(NSString *file)
#endif
{
	bool retVal(false);
	
	gDumpC.setFromString(file);
	
	if (gDumpC.NotEmpty ()) {
		FILE *stream; 
#if qWinOS
		fopen_s( &stream, gDumpC.Get (), "r" );
#else
		stream = fopen(gDumpC.Get (), "r" );
#endif
		retVal = deVignetteInfo->ReadProfile(stream);
		fclose( stream );
	}
	cpfState = kcpfStateSaved;
	gDumpC.Clear();
	
	return retVal;
}

#if qWinOS
bool dng_win_glue::saveCPFFile(String ^file)
#else
bool dng_win_glue::saveCPFFile(NSString *file)
#endif
{
	bool retVal = false;
	gDumpCPF.setFromString(file);
	if (gDumpCPF.NotEmpty () && (cpfState != kcpfStateNone)) {
		
		FILE *stream; 
		
#if qWinOS
		fopen_s( &stream, gDumpCPF.Get (), "w" );
#else
		stream = fopen(gDumpCPF.Get (), "w" );
#endif
		
		if (stream) {
			deVignetteInfo->OutputProfile(stream);
			cpfState = kcpfStateSaved;
			
			if (fclose( stream ) == 0) {
				retVal = true;
			}
		}
	}
	gDumpCPF.Clear();
	
	return retVal;
	
}

void dng_win_glue::zeroOutInfoBlacks (dng_linearization_info *info) {
	// Note we leave the number of rows and columns the same;
	// this is inefficient, but maximises compatibity
	for (uint32 n = 0; n < kMaxSamplesPerPixel; n++) {
		for (uint32  j = 0; j < info->fBlackLevelRepeatRows; j++) {
			for (uint32  k = 0; k < info->fBlackLevelRepeatCols; k++) {
				info->fBlackLevel[j][k][n] = 0;
			}
		}
	}
	// Now the BlackDeltas
	if (info->fBlackDeltaV.Get()) {
		for (uint32  j = 0; j < info->fActiveArea.H(); j++) {
			info->fBlackDeltaV->Buffer_real64 () [j] = 0.0;
		}
	}
	if (info->fBlackDeltaH.Get()) {
		for (uint32  k = 0; k < info->fActiveArea.W(); k++) {
			info->fBlackDeltaH->Buffer_real64 () [k] = 0.0;
		}
	}
}

dng_error_code dng_win_glue::dng_validate ()
{
	
	dng_error_code retVal = dng_error_none;
	// Sane starting point for the thumbnail....
	uint32 thumbMaxSize = 256;
	
	//	gVerbose = true;
	
	if (!gImageDNG.IsEmpty()) 
	{
		dng_local_printf::printf ("Processing \"%s\"...\n", gImageDNG.Get());
		
		try
		{
			
			dng_file_stream stream (gImageDNG.Get());
			
			dng_threaded_host host;
			
			bool alreadyProcessed = false;
			
			if (gMinStage3Size)
			{
				host.SetForPreview  (true);
				host.SetMinimumSize (gMinStage3Size);
			}
			
			if (gDumpDNG.NotEmpty ())
			{
				host.SetSaveDNGVersion (dngVersion_SaveDefault);
				host.SetSaveLinearDNG (false);
				host.SetKeepOriginalFile (true);
			}
			
			// Read into the negative.
			
			dng_local_printf::workerProgress(progressReading);
			
			dng_info info;
			
			AutoPtr<dng_negative> negative;
			
			{
				info.Parse (host, stream);

				info.fMakerNotes->setClipApertures(clipApertures);
				
				info.PostParse (host);
				
				info.fMakerNotes.Get()->DumpMakerNotes();
				
				if (!info.IsValidDNG ())
				{
					return dng_error_bad_format;
				}
				
				negative.Reset (host.Make_dng_negative ());
				
				negative->Parse (host, stream, info);
				
				negative->PostParse (host, stream, info);
				
				{
					dng_timer timer ("Raw image read time");
					negative->ReadStage1Image (host, stream, info);
				}
				
				negative->ValidateRawImageDigest (host);
				
				alreadyProcessed = negative->GetExif()->fSoftware.Contains("CornerFix");
				
				// Look for the thumbnail
				// We want a comparable thumbnail to the original.....
				uint32 smallestExisting = 9999;
				for (uint32 i = 0; i < info.fIFDCount; i++) 
				{
					// SubFileType 1 is a preview...... 
					if (info.fIFD[i]->fNewSubFileType == 1)
					{
						if (smallestExisting >= Max_uint32(info.fIFD[i]->fImageWidth, info.fIFD[i]->fImageLength))
						{
							smallestExisting = info.fIFD[i]->fImageWidth;
						}
					}
				}
				// Only if we haven't been here before
				if (!alreadyProcessed) {
					smallestExisting += smallestExisting>>2;
				}
				thumbMaxSize = Pin_uint32(128, smallestExisting, 768);
			}
			
			dng_string temp = negative->ModelName();
            
#ifdef _DEBUG
            dng_camera_profile profile = negative->ProfileByIndex (0);
            dng_matrix m1 = profile.ColorMatrix2();
            int be = negative->BaselineExposure() * 1000.0f;
            uint32 white = negative->WhiteLevel (0);
            uint32 black = ((dng_linearization_info *) negative->GetLinearizationInfo())->fBlackLevel[0][0][0];

            // we want:
            /* 
             { "Nikon D800E", 0, 0, 350,
			{ 7866,-2108,-555,-4869,12483,2681,-1176,2069,7501 } },
             */
            printf("{ \"%s\", %i, 0x%x, %i, \n { ", temp.Get(), black, white, be);
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 3; col++) {
                    int v = m1 [row] [col] *10000.0f;
                    if (row==0 && col==0) {
                        printf ("%i", v);
                    }
                    else {
                        printf (",%i", v);
                    }
                }
            }
            printf(" } },\n");
            
            printf ("DefaultCropOrigin: H = %0.2f V = %0.2f\n",
                                      negative->DefaultCropOriginH().As_real64 (),
                                      negative->DefaultCropOriginV().As_real64 ());
            printf ("DefaultCropSize: H = %0.2f V = %0.2f\n",
                                      negative->DefaultCropSizeH().As_real64 (),
                                      negative->DefaultCropSizeV().As_real64 ());
            
#endif
            
			
            bool isLinearRaw = !(negative->GetMosaicInfo () != NULL);
			
			if (isLinearRaw) {
				// Here we have an RGB "linear raw" image
				if (negative->Stage1Image() != NULL) {
					if (negative->Stage1Image()->Planes() != 3) {
						retVal =  CornerFix_bad_linear;
					}
				}
			}
			else {
				// Here we have a Bayer array - check it one we understand......
				if ((negative->GetMosaicInfo ()->fCFAPatternSize != dng_point (2, 2)) ||
                    (negative->GetMosaicInfo ()->fColorPlanes != 3)) {
					retVal = CornerFix_bad_Bayer;
				}
			}

			if (negative->Stage1Image() != NULL) {
				if ((negative->Stage1Image()->PixelType() != ttByte) && (negative->Stage1Image()->PixelType() != ttShort)) {
					retVal =  CornerFix_bad_pixel_format;
				}
			}			
			
#ifdef DMR_HACK
			if ((temp.EndsWith("DMR") || temp.StartsWith("M8")))
			{
				retVal = dng_error_none;
			}
#else
			if ((temp.StartsWith("M8") 
				  || temp.StartsWith("M9") 
				  || temp.StartsWith("S2") 
				  || temp.EndsWith("S2")
				  || temp.StartsWith("Sony NEX-")
                  || temp.StartsWith("M Monochrom")
				  ))
			{
				retVal = dng_error_none;
			}
#endif
			if (retVal != dng_error_none) {
				// If we completely can't process this file, then remove whatever temp file we have....
				remove(gDumpDNG.Get ());
				return retVal;
			}
			
			
			// If we want to take a look at noise data, uncomment this...
			// zeroOutInfoBlacks ((dng_linearization_info *) negative->GetLinearizationInfo());
			
			// Option to write stage 1 image.
			
			if (gDumpStage1.NotEmpty ())
			{
				dng_file_stream stream2 (gDumpStage1.Get (), true);
				const dng_image &stage1 = *negative->Stage1Image ();
				dng_image_writer writer;
				
				writer.WriteTIFF (host,
								  stream2,
								  stage1,
								  stage1.Planes () >= 3 ? piRGB 
								  : piBlackIsZero);
			}
			
			// If the DNG is level compressed and we asked for hires, then uncompress
			// and also remove any black levels we don't like.....
#ifdef DMR_HACK
			if (YES)
#else
			if (buildHires && negative->GetLinearizationInfo()->fLinearizationTable.Get()) 
#endif
			{
				dng_timer timer ("Linearize raw bayer data");
				// Here we build a hires version of the file
				
				// Finalize linearization info.
				
				{
					negative->SetActiveArea(negative->RawImage().Bounds ());
				}
				if (gFourColorBayer)
				{
					negative->SetFourColorBayer ();
				}
				
				newStage1Image.Reset (host.Make_dng_image (negative->RawImage().Bounds(),
													   negative->RawImage().Planes (),
													   gMathDataType));
				
#ifdef DMR_HACK
				for (int i = 0; i < kMaxSamplesPerPixel; i++) {
					((dng_linearization_info *) negative->GetLinearizationInfo())->fWhiteLevel[i] /= 4;
				}
#endif
				dng_highres_info hiresInfo((dng_linearization_info *) negative->GetLinearizationInfo(), host, maximizeResolution);
				hiresInfo.setAntiAlias(deVignetteInfo->GetAntiAlias());
				
				
				dng_image &newStage1 = *newStage1Image;
				
				const dng_image &stage1 = negative->RawImage();
				hiresInfo.doHires (host, stage1, newStage1);
				negative->SetStage1Image(newStage1Image);
				
				AutoPtr<dng_memory_block> newCurve; {
					negative->SetLinearization(newCurve);
				}
				zeroOutInfoBlacks ((dng_linearization_info *) negative->GetLinearizationInfo());
			}
			else {
				// Check whether we need to get rid of any black levels now;
				// If we're devignetting, it will be done then.
				if ((cpfState == kcpfStateNone) && !buildCPF) {
					{
						dng_timer timer ("Process Black Levels");
						newStage1Image.Reset (host.Make_dng_image (negative->RawImage().Bounds(),
																   negative->RawImage().Planes (),
																   negative->RawImage().PixelType()));

						dng_devignette_negative devignetter;
						
						dng_image &newStage1 = *newStage1Image;
						
						const dng_image &stage1 = negative->RawImage();
						const dng_linearization_info &linInfo = *negative->GetLinearizationInfo();
						dng_string temp = negative->ModelName();
						devignetter.doDeVignette (host, linInfo, stage1, newStage1, NULL, 
												  0, temp);
						negative->SetStage1Image(newStage1Image);
						zeroOutInfoBlacks ((dng_linearization_info *) negative->GetLinearizationInfo());
					}
				}
			}
			
			// This selects whether we generate a testchart, or process the actual image......
			if (false) {
				{
					negative->SetActiveArea(negative->RawImage().Bounds ());
				}
				if (gFourColorBayer)
				{
					negative->SetFourColorBayer ();
				}
				
				newStage1Image.Reset (host.Make_dng_image (negative->RawImage().Bounds(),
														   negative->RawImage().Planes (),
														   negative->RawImage().PixelType()));				
				
				GretagMacBeth patches(*negative);
				dng_macbeth_info macbethInfo;
				macbethInfo.initFromLinearizationInfo((dng_linearization_info *) negative->GetLinearizationInfo(), host, &patches);
				dng_image &newStage1 = *newStage1Image;
				
				const dng_image &stage1 = negative->RawImage();
				dng_string temp = negative->ModelName();
				macbethInfo.doMacbeth (host, stage1, newStage1, temp);
				negative->SetStage1Image(newStage1Image);
			}
			
			
			bool doneStage2 = false;
			
			// Now we actually devignette, if asked......
			// We iterate round this loop till we've built CPF files, devignetted, etc as required
			
			do {
				dng_local_printf::workerProgress(progressDevignette);
				if ((cpfState != kcpfStateNone) && !buildCPF) {
					{
						dng_timer timer ("Raw image devignette time");
						newStage1Image.Reset (host.Make_dng_image (negative->RawImage().Bounds(),
																   negative->RawImage().Planes (),
																   negative->RawImage().PixelType()));

						dng_devignette_negative devignetter;
						devignetter.SetChromaStrength(chromaStrength);
						devignetter.SetLumaStrength(lumaStrength);
						
						dng_image &newStage1 = *newStage1Image;
						
						const dng_image &stage1 = negative->RawImage();
						const dng_linearization_info &linInfo = *negative->GetLinearizationInfo();
						dng_string temp = negative->ModelName();
						devignetter.doDeVignette (host, linInfo, stage1, newStage1, deVignetteInfo, 
												  deVignetteInfo->GetAntiAlias(), temp);
						negative->SetStage1Image(newStage1Image);
						
						zeroOutInfoBlacks ((dng_linearization_info *) negative->GetLinearizationInfo());
					}
					
				}
				
				//Now we sort out all the data fields..........
				
				if (!negative->GetExif()->fSoftware.EndsWith(CornerFixVersion)) {
					
#ifdef DMR_HACK
					negative->GetExif()->fSoftware.Append("; post-processed by DMR2M8 ");
#else
					negative->GetExif()->fSoftware.Append("; post-processed by CornerFix ");
					negative->GetExif()->fSoftware.Append(CornerFixVersion);
#endif
					// Important to syncronize the XMP data with the EXIF data
					negative->GetXMP()->SetString (XMP_NS_XAP,
												   "CreatorTool", negative->GetExif()->fSoftware);
				}
				
				
				//Now we do the aperture and lens values
				
				if (exifUpdate) {
					if (calculateApertures && info.fMakerNotes->fCalculatedAperture.IsValid ()) {
						if (!negative->GetExif()->fApertureValue.IsValid () && info.fMakerNotes->fCalculatedAperture.IsValid ()) {
							negative->GetExif()->fApertureValue.n = info.fMakerNotes->fCalculatedAperture.n;
							negative->GetExif()->fApertureValue.d = info.fMakerNotes->fCalculatedAperture.d;
						}
					}
					else {
						if (!negative->GetExif()->fApertureValue.IsValid () && info.fMakerNotes->fEstimatedAperture.IsValid ()) {
							negative->GetExif()->fApertureValue.n = info.fMakerNotes->fEstimatedAperture.n;
							negative->GetExif()->fApertureValue.d = info.fMakerNotes->fEstimatedAperture.d;
						}
					}
					if (negative->GetExif()->fApertureValue.IsValid ()) {
						negative->GetXMP()->Set_urational (XMP_NS_EXIF,
														   "ApertureValue", negative->GetExif()->fApertureValue);
					}
					
					
					//We got to have fstop and aperture match.......
					if (!negative->GetExif()->fFNumber.IsValid () && negative->GetExif()->fApertureValue.IsValid ()) {
						negative->GetExif()->fFNumber.Set_real64(pow (2.0, negative->GetExif()->fApertureValue.As_real64() * 0.5));
						negative->GetXMP()->Set_urational (XMP_NS_EXIF,
														   "FNumber", negative->GetExif()->fFNumber);
					}
					if ((negative->GetExif()->fLensName.Length() == 0) && (info.fMakerNotes->fLensId != 0xffffffff)) {
						negative->GetExif()->fLensName.Append(info.fMakerNotes->GetLensName());
						negative->GetXMP()->SetString (XMP_NS_AUX, "Lens", negative->GetExif()->fLensName);
					}
				}
				
				// Do we want to change the BayerGreenSplit?
				if (deVignetteInfo->getBayerGreenSplit() < kMaxGreenSplit) {
					negative->SetGreenSplit(deVignetteInfo->getBayerGreenSplit());
                    overwriteCameraName = true;
                }
                if (overwriteCameraName) {
					if (!negative->GetExif()->fModel.Contains("/CornerFix")) {
						negative->GetExif()->fModel.Append("/CornerFix");
						negative->SetModelName(negative->GetExif()->fModel.Get());
					}
				}
				
				
#ifdef DMR_HACK
				if ([[NSUserDefaults standardUserDefaults] boolForKey:@"overwriteCameraName"]) 
				{
					negative->GetExif()->fModel.Set("M8 Digital Camera");
					negative->SetModelName("M8 Digital Camera");
				}
#endif
				
				// Metadata.
				
				negative->SynchronizeMetadata ();
				// Clearing the RawImageDigest ensures that if this file has previously been processed by
				// Lightroom, we don't pick up "Damaged File" errors the next time Lightroom loads it
				negative->ClearRawImageDigest ();
				
				// Test code to allow the color matrices to be overridden 
				{
					//					dng_camera_profile *profile = (dng_camera_profile *) negative->EmbeddedCameraProfile ();
					//					dng_matrix_3by3 m1 = dng_matrix_3by3(1,0,1);
					// Old cal values ColorMatrix1:
					//					dng_matrix_3by3 m1 = dng_matrix_3by3( 0.6863,  -0.1407,  -0.0775,
					//						-0.3086,   1.1390,   0.1921,
					//						-0.0971,   0.2791,   0.6609);
					
					//					profile->SetColorMatrix1(m1);
					//					profile->SetColorMatrix2(m1);
					//					m1[1][1] = 0.5;
					//					profile->SetCameraCalibration1(m1);
					//					profile->SetCameraCalibration2(m1);
					// ACR ignors the Colormatrix, but not the Calibration matrix
					// if it recognises the camera name
					//					negative->GetExif()->fModel.Set("Unknown Camera");
					//					negative->SetModelName("Unknown Camera");
				}
				
				
				// Four color Bayer option.
				
				dng_local_printf::workerProgress(progressLinearization);
				if (gFourColorBayer)
				{
					negative->SetFourColorBayer ();
				}
				
				// Build stage 2 image.
				
				{
					dng_timer timer ("Linearization time");
					negative->BuildStage2Image (host,
												gMathDataType);
					doneStage2 = true;
				}
				
				deVignetteInfo->SetStage2Buffer((dng_simple_image *) negative->Stage2Image());
                deVignetteInfo->setCropArea(negative->DefaultCropOriginH().As_real64(),
                                            negative->DefaultCropOriginV().As_real64(),
                                            negative->DefaultCropSizeH().As_real64(),
                                            negative->DefaultCropSizeV().As_real64());
				
				
				dng_local_printf::workerProgress(progressIlluminant);
				if (buildCPF && doIlluminant) {
					{
						dng_timer timer ("Illuminant compensation time");
						deVignetteInfo->correctIluminant();
					}
				}
				
				dng_local_printf::workerProgress(progressAnalysis);
				if (buildCPF) {
					{
						dng_timer timer ("Image analysis time");
						deVignetteInfo->setNumSegments(8);
						if (deVignetteInfo->AnalyseStage2(multiple)) {
							retVal = CornerFix_saturated;
						}
						if (gDumpCSV.NotEmpty ()) {
							FILE *stream; 
#if qWinOS
							fopen_s( &stream, gDumpCSV.Get (), "w" );
#else
							stream = fopen(gDumpCSV.Get (), "w" );
#endif
							deVignetteInfo->PrintAnalysisData(stream);
							
							fclose( stream );
							gDumpCSV.Clear();
						}
						cpfState = kcpfStateCreated;
						buildCPF = false;
						doneStage2 = !(gDumpCPF.NotEmpty () || gDumpStage2.NotEmpty () || gDumpStage3.NotEmpty () ||
									   gDumpTIF.NotEmpty () || gDumpDNG.NotEmpty ());
					}
				}
			}
			while (!doneStage2); // end do loop
			
			
			if (gDumpCPF.NotEmpty ()) {
				FILE *stream; 
#if qWinOS
				fopen_s( &stream, gDumpCPF.Get (), "w" );
#else
				stream = fopen(gDumpCPF.Get (), "w" );
#endif
				deVignetteInfo->OutputProfile(stream);
				fclose( stream );
			}
			
			if (gDumpStage2.NotEmpty ())
			{
				dng_file_stream stream2 (gDumpStage2.Get (), true);
				const dng_image &stage2 = *negative->Stage2Image ();
				dng_image_writer writer;
				
				writer.WriteTIFF (host,
								  stream2,
								  stage2,
								  stage2.Planes () >= 3 ? piRGB 
								  : piBlackIsZero);
			}
			
			// Build stage 3 image.
			dng_local_printf::workerProgress(progressInterpolate);
			
			{
				dng_timer timer ("Interpolate time");
				if (gDumpStage3.IsEmpty () && gDumpTIF.IsEmpty ()) {
					// We don't need a full size image
					host.SetForPreview  (true);
					host.SetPreferredSize (thumbMaxSize);
				}
				negative->BuildStage3Image (host,
											gMosaicPlane);
			}
			
			if (gDumpStage3.NotEmpty ())
			{
				dng_file_stream stream2 (gDumpStage3.Get (), true);
				const dng_image &stage3 = *negative->Stage3Image ();
				dng_image_writer writer;
				
				writer.WriteTIFF (host,
								  stream2,
								  stage3,
								  stage3.Planes () >= 3 ? piRGB 
								  : piBlackIsZero);
			}
			
			// Update metadata.
			
			negative->UpdateDateTimeToNow ();
			
			negative->RebuildIPTC (true, false);
			
			// Recomute the UniqueID - otherwise LR will use an old cached
			// thumbnail.
			negative->RecomputeRawDataUniqueID (host);
			
			// Output TIF file if requested.
			
			if (gDumpTIF.NotEmpty ())
			{
				// Render final image.
				dng_render render (host, *negative);
				
				render.SetFinalSpace     (*gFinalSpace   );
				render.SetFinalPixelType (gFinalPixelType);
				
				dng_local_printf::workerProgress(progressRender);
				if (gMinStage3Size)
				{
					dng_point stage3Size = negative->Stage3Image ()->Size ();
					render.SetMaximumSize (Max_uint32 (stage3Size.v,
													   stage3Size.h));
				}
				
				AutoPtr<dng_image> finalImage;
				
				{
					dng_timer timer ("Render time");
					finalImage.Reset (render.Render ());
				}
				
				finalImage->Rotate (negative->Orientation ());
				
				// Write TIF file.
				
				dng_file_stream stream2 (gDumpTIF.Get (), true);
				
				dng_local_printf::workerProgress(progressWriteTIFF);
				dng_image_writer writer;
				
				{
					dng_timer timer ("Write TIFF time");
					writer.WriteTIFF (host,
									  stream2,
									  *finalImage.Get (),
									  finalImage->Planes () >= 3 ? piRGB 
									  : piBlackIsZero,
									  ccUncompressed,
									  negative.Get (),
									  &render.FinalSpace ());
				}
			}
			
			// Output DNG file if requested.
			
			if (gDumpDNG.NotEmpty ())
			{
				// Build thumbnail image.
				dng_local_printf::workerProgress(progressThumbnail);
				dng_image_preview thumbnail;
				
				{
					dng_timer timer ("Build thumbnail time");
					dng_render render (host, *negative);
					
					render.SetFinalSpace (negative->IsMonochrome () ? dng_space_GrayGamma22::Get ()
										  : dng_space_sRGB       ::Get ());
					render.SetFinalPixelType (ttByte);
					render.SetMaximumSize (thumbMaxSize);
					thumbnail.fImage.Reset (render.Render ());
					thumbnail.fInfo.fApplicationName   .Set (CornerFixNameString);
					thumbnail.fInfo.fApplicationVersion.Set (CornerFixVersion);
					thumbnail.fInfo.fSettingsName.Set ("Default");
					thumbnail.fInfo.fColorSpace = thumbnail.fImage->Planes () == 1 ?
															previewColorSpace_GrayGamma22 :
															previewColorSpace_sRGB;
					
					dng_date_time_info dateTimeInfo;
					CurrentDateTimeAndZone (dateTimeInfo);
					
					thumbnail.fInfo.fDateTime = dateTimeInfo.Encode_ISO_8601 ();
				}
				
				// Write DNG file.
				dng_file_stream stream2 (gDumpDNG.Get (), true);
				
				dng_local_printf::workerProgress(progressWriteDNG);
				dng_image_writer writer;
				{
					dng_timer timer ("Write DNG time");
					if (maximizeResolution) stream2.SetLittleEndian(true);			
					
					writer.WriteDNG (host,
									 stream2,
									 *negative.Get (),
									 thumbnail,
									 compressTag);
				}
			}

		}
		catch (const dng_exception &except)
		{
			return except.ErrorCode ();
		}
		catch (...)
		{
			return dng_error_unknown;
		}
		dng_local_printf::printf ("Processing complete\n");
	}
	return retVal;
	
}



