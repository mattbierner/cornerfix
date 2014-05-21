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
 * tiffThumb.cpp
 * ---------------
 * (C) Copyright 2007-2011, by Sandy McGuffog and Contributors.
 *
 * Original Author:  Sandy McGuffog;
 * Contributor(s):   -;
 *
 *
 * Changes
 * -------
 * 15 June 2009 : Initial version for CornerFix 1.1.0.0
 * 14 July 2008 : CornerFix 1.1.0.1; fix bug that could could cause a "bad parameter"
 *                or "security" exception.
 * 28 Nov 2009  : CornerFix 1.3.0.4; fix to avoid (harmless) compiler warning.
 * 7 Mar 2010   : CornerFix 1.3.0.6; fix to allow JPEG format thumbnails
 * 4 July 2011 : CornerFix 1.4.2.0; fix for compile warnings under XCode 4.1
 *
 */

/*
* This code extracts the thumbnail from a DNG file and turns it into a standalone 
* TIFF or JPEG format file.
* This was prompted by Windows 7, which (sigh) is no longer capable of displaying the 
* thumbnail of a DNG file in a .Net PictureBox control, which all previous version of 
* Windows did. This in turn broke the previous version of CornerFix
*
* Although written for CornerFix, tiffThumb is completely standalone, as it contains all
* the code in it to decode enough of a DNG file to find the parts it needs. AKA, 
* tiffThumb does not use or depend on the DNG SDK. This was done because (a) the DNG
* SDK doesn't actually extract thumbnails, and to make it do so is quite difficult and 
* (b) this code is useful standalone.
* Note that while that means tiffThumb understands subsets of DNG and tiff formats
* (actually the same thing), this code does not actually undestand the image data - 
* it just copies image data verbatim.
* If the thumbnail is in JPEG format, then then tiffThumb actually produces a JPEG file.
* In that case, in order to get image orientation right, it will also insert an APP1 
* EXIF section into the file, as a pure JFIF JPEG doesn't have any way to specify
* orientation. 
* Documentation for the API is in tiffThumb.h
*/

#include "tiffThumb.h"


// Main File header
struct TIFFFile {
	uint8_t	byteOrder[2];							// Big or little endian
	uint8_t	magicNumber[2];							// Type signature; from here on in
													// everything depends on the endian tag
	uint8_t	ifdOffset[4];							// 0th IFD
};



// IFD Header
struct TIFFIFDHeader {
	uint8_t	numEntries[2];							// How many entries in the IFD?
};

// IFD footer
struct TIFFIFDFooter {
	uint8_t	ifdOffset[4];							// Offset of the next IFD
};

// IFD Structure
struct TIFFIFD {
	struct TIFFIFDHeader header;
	struct TIFFDirectory dirZero;					// The first directory entry....
													// .... numEntries of directories....
	struct TIFFIFDFooter footer;
};


/*****************************************************************************/

// TIFF-style byte order markers.

enum
{	
	byteOrderII					= 0x4949,		// 'II'
	byteOrderMM					= 0x4D4D		// 'MM'
};

/*****************************************************************************/


enum TIFFTagTypeEnum
{
	TIFFTypeByte = 1,
	TIFFTypeAscii,
	TIFFTypeShort,
	TIFFTypeLong,
	TIFFTypeRational,
	TIFFTypeSByte,
	TIFFTypeUndefined,
	TIFFTypeSShort,
	TIFFTypeSLong,
	TIFFTypeSRational,
	TIFFTypeFloat,
	TIFFTypeDouble,
	TIFFTypeIFD,
	TIFFTypeUnicode,
	TIFFTypeComplex
};

static const size_t TIFFTaglengths[TIFFTypeComplex+1] = {
	1,   // Dummy data
	1,   // TIFFTypeByte,
	1,   // TIFFTypeAscii,
	2,   // TIFFTypeShort,
	4,   // TIFFTypeLong,
	8,   // TIFFTypeRational,
	1,   // TIFFTypeSByte,
	1,   // TIFFTypeUndefined,
	2,   // TIFFTypeSShort,
	4,   // TIFFTypeSLong,
	8,   // TIFFTypeSRational,
	4,   // TIFFTypeFloat,
	8,   // TIFFTypeDouble,
	4,   // TIFFTypeIFD,
	2,   // TIFFTypeUnicode,
	8    // TIFFTypeComplex
};

// Tags
enum TIFFTagCodeEnum
{
	kcNewSubFileType				= 254,
	kcSubFileType					= 255,
	kcImageWidth					= 256,
	kcImageLength					= 257,
	kcBitsPerSample					= 258,
	kcCompression					= 259,
	kcPhotometricInterpretation		= 262,
	kcThresholding					= 263,
	kcCellWidth						= 264,
	kcCellLength					= 265,
	kcFillOrder						= 266,
	kcImageDescription				= 270,
	kcMake							= 271,
	kcModel							= 272,
	kcStripOffsets					= 273,
	kcOrientation					= 274,
	kcSamplesPerPixel				= 277,
	kcRowsPerStrip					= 278,
	kcStripByteCounts				= 279,
	kcMinSampleValue				= 280,
	kcMaxSampleValue				= 281,
	kcXResolution					= 282,
	kcYResolution					= 283,
	kcPlanarConfiguration			= 284,
	kcFreeOffsets					= 285,
	kcFreeByteCounts				= 286,
	kcGrayResponseUnit				= 290,
	kcGrayResponseCurve				= 291,
	kcResolutionUnit				= 296,
	kcTransferFunction				= 301,
	kcSoftware						= 305,
	kcDateTime						= 306,
	kcArtist						= 315,
	kcHostComputer					= 316,
	kcPredictor						= 317,
	kcWhitePoint					= 318,
	kcPrimaryChromaticities			= 319,
	kcColorMap						= 320,
	kcTileWidth						= 322,
	kcTileLength					= 323,
	kcTileOffsets					= 324,
	kcTileByteCounts				= 325,
	kcSubIFDs						= 330,
	kcExtraSamples					= 338,
	kcSampleFormat					= 339,
	kcJPEGTables					= 347,
	kcJPEGProc						= 512,
	kcJPEGInterchangeFormat			= 513,
	kcJPEGInterchangeFormatLength	= 514,
	kcYCbCrCoefficients				= 529,
	kcYCbCrSubSampling				= 530,
	kcYCbCrPositioning				= 531,
	kcReferenceBlackWhite			= 532,
	kcXMP							= 700,
	kcKodakCameraSerialNumber		= 33405,
	kcCFARepeatPatternDim			= 33421,
	kcCFAPattern					= 33422,
	kcBatteryLevel					= 33423,
	kcKodakDCRPrivateIFD			= 33424,
	kcCopyright						= 33432,
	kcExposureTime					= 33434,
	kcFNumber						= 33437,
	kcIPTC_NAA						= 33723,
	kcLeafPKTS						= 34310,
	kcAdobeData						= 34377,
	kcExifIFD						= 34665,
	kcICCProfile					= 34675,
	kcExposureProgram				= 34850,
	kcSpectralSensitivity			= 34852,
	kcGPSInfo						= 34853,
	kcISOSpeedRatings				= 34855,
	kcOECF							= 34856,
	kcInterlace						= 34857,
	kcTimeZoneOffset				= 34858,
	kcSelfTimerMode					= 34859,
	kcExifVersion					= 36864,
	kcDateTimeOriginal				= 36867,
	kcDateTimeDigitized				= 36868,
	kcComponentsConfiguration		= 37121,
	kcCompressedBitsPerPixel		= 37122,
	kcShutterSpeedValue				= 37377,
	kcApertureValue					= 37378,
	kcBrightnessValue				= 37379,
	kcExposureBiasValue				= 37380,
	kcMaxApertureValue				= 37381,
	kcSubjectDistance				= 37382,
	kcMeteringMode					= 37383,
	kcLightSource					= 37384,
	kcFlash							= 37385,
	kcFocalLength					= 37386,
	kcFlashEnergy					= 37387,
	kcSpatialFrequencyResponse		= 37388,
	kcNoise							= 37389,
	kcFocalPlaneXResolution 		= 37390,
	kcFocalPlaneYResolution			= 37391,
	kcFocalPlaneResolutionUnit		= 37392,
	kcImageNumber					= 37393,
	kcSecurityClassification		= 37394,
	kcImageHistory					= 37395,
	kcSubjectArea					= 37396,
	kcExposureIndex					= 37397,
	kcTIFF_EP_StandardID			= 37398,
	kcSensingMethod					= 37399,
	kcMakerNote						= 37500,
	kcUserComment					= 37510,
	kcSubsecTime					= 37520,
	kcSubsecTimeOriginal			= 37521,
	kcSubsecTimeDigitized			= 37522,
	kcAdobeLayerData				= 37724,
	kcFlashPixVersion				= 40960,
	kcColorSpace					= 40961,
	kcPixelXDimension				= 40962,
	kcPixelYDimension				= 40963,
	kcRelatedSoundFile				= 40964,
	kcInteroperabilityIFD			= 40965,
	kcFlashEnergyExif				= 41483,
	kcSpatialFrequencyResponseExif 	= 41484,
	kcFocalPlaneXResolutionExif		= 41486,
	kcFocalPlaneYResolutionExif		= 41487,
	kcFocalPlaneResolutionUnitExif	= 41488,
	kcSubjectLocation				= 41492,
	kcExposureIndexExif			    = 41493,
	kcSensingMethodExif				= 41495,
	kcFileSource					= 41728,
	kcSceneType						= 41729,
	kcCFAPatternExif				= 41730,
	kcCustomRendered				= 41985,
	kcExposureMode					= 41986,
	kcWhiteBalance					= 41987,
	kcDigitalZoomRatio				= 41988,
	kcFocalLengthIn35mmFilm			= 41989,
	kcSceneCaptureType				= 41990,
	kcGainControl					= 41991,
	kcContrast						= 41992,
	kcSaturation					= 41993,
	kcSharpness						= 41994,
	kcDeviceSettingDescription		= 41995,
	kcSubjectDistanceRange			= 41996,
	kcImageUniqueID					= 42016,
	kcGamma							= 42240,
	kcPrintImageMakchingInfo		= 50341,
	kcDNGVersion					= 50706,
	kcDNGBackwardVersion			= 50707,
	kcUniqueCameraModel				= 50708,
	kcLocalizedCameraModel			= 50709,
	kcCFAPlaneColor					= 50710,
	kcCFALayout						= 50711,
	kcLinearizationTable			= 50712,
	kcBlackLevelRepeatDim			= 50713,
	kcBlackLevel					= 50714,
	kcBlackLevelDeltaH				= 50715,
	kcBlackLevelDeltaV				= 50716,
	kcWhiteLevel					= 50717,
	kcDefaultScale					= 50718,
	kcDefaultCropOrigin				= 50719,
	kcDefaultCropSize				= 50720,
	kcColorMatrix1					= 50721,
	kcColorMatrix2					= 50722,
	kcCameraCalibration1			= 50723,
	kcCameraCalibration2			= 50724,
	kcReductionMatrix1				= 50725,
	kcReductionMatrix2				= 50726,
	kcAnalogBalance					= 50727,
	kcAsShotNeutral					= 50728,
	kcAsShotWhiteXY					= 50729,
	kcBaselineExposure				= 50730,
	kcBaselineNoise					= 50731,
	kcBaselineSharpness				= 50732,
	kcBayerGreenSplit				= 50733,
	kcLinearResponseLimit			= 50734,
	kcCameraSerialNumber			= 50735,
	kcLensInfo						= 50736,
	kcChromaBlurRadius				= 50737,
	kcAntiAliasStrength				= 50738,
	kcShadowScale					= 50739,
	kcDNGPrivateData				= 50740,
	kcMakerNoteSafety				= 50741,
	kcCalibrationIlluminant1		= 50778,
	kcCalibrationIlluminant2		= 50779,
	kcBestQualityScale				= 50780,
	kcRawDataUniqueID				= 50781,
	kcOriginalRawFileName			= 50827,
	kcOriginalRawFileData			= 50828,
	kcActiveArea					= 50829,
	kcMaskedAreas					= 50830,
	kcAsShotICCProfile				= 50831,
	kcAsShotPreProfileMatrix		= 50832,
	kcCurrentICCProfile				= 50833,
	kcCurrentPreProfileMatrix		= 50834,
	kcColorimetricReference			= 50879,
	kcCameraCalibrationSignature	= 50931,
	kcProfileCalibrationSignature,
	kcExtraCameraProfiles,
	kcAsShotProfileName,
	kcNoiseReductionApplied,
	kcProfileName,
	kcProfileHueSatMapDims,
	kcProfileHueSatMapData1,
	kcProfileHueSatMapData2,
	kcProfileToneCurve,
	kcProfileEmbedPolicy,
	kcProfileCopyright,
	kcForwardMatrix1				= 50964,
	kcForwardMatrix2,
	kcPreviewApplicationName,
	kcPreviewApplicationVersion,
	kcPreviewSettingsName,
	kcPreviewSettingsDigest,
	kcPreviewColorSpace,
	kcPreviewDateTime,
	kcRawImageDigest,
	kcOriginalRawFileDigest,
	kcSubTileBlockSize,
	kcRowInterleaveFactor,
	kcProfileLookTableDims			= 50981,
	kcProfileLookTableData			= 50982,
	kcKodakKDCPrivateIFD			= 65024,
	kcFirstSubIFD					= 0x10000,
	kcLastSubIFD					= 0x1FFFF,
	kcFirstChainedIFD				= 0x20000,
	kcLastChainedIFD				= 0x2FFFF,
	kcFirstMakerNoteIFD				= 0x30000,
	kcLastMakerNoteIFD				= 0x3FFFF,
	kcCanonMakerNote				= kcFirstMakerNoteIFD,
	kcEpsonMakerNote,
	kcFujiMakerNote,
	kcKodakMakerNote,
	kcKodakMakerNote65280,
	kcLeicaMakerNote,
	kcMamiyaMakerNote,
	kcMinoltaMakerNote,
	kcNikonMakerNote,
	kcOlympusMakerNote,
	kcOlympusMakerNote8208,
	kcOlympusMakerNote8224,
	kcOlympusMakerNote8240,
	kcOlympusMakerNote8256,
	kcOlympusMakerNote8272,
	kcOlympusMakerNote12288,
	kcPanasonicMakerNote,
	kcPentaxMakerNote,
	kcPhaseOneMakerNote,
	kcRicohMakerNote,
	kcRicohMakerNoteCameraInfo,
	kcSonyMakerNote,
	kcSonyMakerNoteSubInfo,
	kcSonyPrivateIFD1,
	kcSonyPrivateIFD2,
	kcSonyPrivateIFD3A,
	kcSonyPrivateIFD3B,
	kcSonyPrivateIFD3C,
	kcCanonCRW						= 0x40000,
	kcContaxRAW,
	kcContaxHeader,
	kcFujiRAF,
	kcFujiHeader,
	kcFujiRawInfo1,
	kcFujiRawInfo2,
	kcLeafMOS,
	kcMinoltaMRW,
	kcPanasonicRAW,
	kcFoveonX3F,
	kcJPEG,
	kcAdobePSD	
};

enum MetaLookUpLengths {
	kMaxTIFFTagCodes		= 264,
};

static const uint32_t TIFFTagCodes[kMaxTIFFTagCodes] = {
	kcNewSubFileType,
	kcSubFileType,
	kcImageWidth,
	kcImageLength,
	kcBitsPerSample,
	kcCompression,
	kcPhotometricInterpretation,
	kcThresholding,
	kcCellWidth,
	kcCellLength,
	kcFillOrder,
	kcImageDescription,
	kcMake,
	kcModel,
	kcStripOffsets,
	kcOrientation,
	kcSamplesPerPixel,
	kcRowsPerStrip,
	kcStripByteCounts,
	kcMinSampleValue,
	kcMaxSampleValue,
	kcXResolution,
	kcYResolution,
	kcPlanarConfiguration,
	kcFreeOffsets,
	kcFreeByteCounts,
	kcGrayResponseUnit,
	kcGrayResponseCurve,
	kcResolutionUnit,
	kcTransferFunction,
	kcSoftware,
	kcDateTime,
	kcArtist,
	kcHostComputer,
	kcPredictor,
	kcWhitePoint,
	kcPrimaryChromaticities,
	kcColorMap,
	kcTileWidth,
	kcTileLength,
	kcTileOffsets,
	kcTileByteCounts,
	kcSubIFDs,
	kcExtraSamples,
	kcSampleFormat,
	kcJPEGTables,
	kcJPEGProc,
	kcJPEGInterchangeFormat,
	kcJPEGInterchangeFormatLength,
	kcYCbCrCoefficients,
	kcYCbCrSubSampling,
	kcYCbCrPositioning,
	kcReferenceBlackWhite,
	kcXMP,
	kcKodakCameraSerialNumber,
	kcCFARepeatPatternDim,
	kcCFAPattern,
	kcBatteryLevel,
	kcKodakDCRPrivateIFD,
	kcCopyright,
	kcExposureTime,
	kcFNumber,
	kcIPTC_NAA,
	kcLeafPKTS,
	kcAdobeData,
	kcExifIFD,
	kcICCProfile,
	kcExposureProgram,
	kcSpectralSensitivity,
	kcGPSInfo,
	kcISOSpeedRatings,
	kcOECF,
	kcInterlace,
	kcTimeZoneOffset,
	kcSelfTimerMode,
	kcExifVersion,
	kcDateTimeOriginal,
	kcDateTimeDigitized,
	kcComponentsConfiguration,
	kcCompressedBitsPerPixel,
	kcShutterSpeedValue,
	kcApertureValue,
	kcBrightnessValue,
	kcExposureBiasValue,
	kcMaxApertureValue,
	kcSubjectDistance,
	kcMeteringMode,
	kcLightSource,
	kcFlash,
	kcFocalLength,
	kcFlashEnergy,
	kcSpatialFrequencyResponse,
	kcNoise,
	kcFocalPlaneXResolution,
	kcFocalPlaneYResolution,
	kcFocalPlaneResolutionUnit,
	kcImageNumber,
	kcSecurityClassification,
	kcImageHistory,
	kcSubjectArea,
	kcExposureIndex,
	kcTIFF_EP_StandardID,
	kcSensingMethod,
	kcMakerNote,
	kcUserComment,
	kcSubsecTime,
	kcSubsecTimeOriginal,
	kcSubsecTimeDigitized,
	kcAdobeLayerData,
	kcFlashPixVersion,
	kcColorSpace,
	kcPixelXDimension,
	kcPixelYDimension,
	kcRelatedSoundFile,
	kcInteroperabilityIFD,
	kcFlashEnergyExif,
	kcSpatialFrequencyResponseExif,
	kcFocalPlaneXResolutionExif,
	kcFocalPlaneYResolutionExif,
	kcFocalPlaneResolutionUnitExif,
	kcSubjectLocation,
	kcExposureIndexExif,
	kcSensingMethodExif,
	kcFileSource,
	kcSceneType,
	kcCFAPatternExif,
	kcCustomRendered,
	kcExposureMode,
	kcWhiteBalance,
	kcDigitalZoomRatio,
	kcFocalLengthIn35mmFilm,
	kcSceneCaptureType,
	kcGainControl,
	kcContrast,
	kcSaturation,
	kcSharpness,
	kcDeviceSettingDescription,
	kcSubjectDistanceRange,
	kcImageUniqueID,
	kcGamma,
	kcPrintImageMakchingInfo,
	kcDNGVersion,
	kcDNGBackwardVersion,
	kcUniqueCameraModel,
	kcLocalizedCameraModel,
	kcCFAPlaneColor,
	kcCFALayout,
	kcLinearizationTable,
	kcBlackLevelRepeatDim,
	kcBlackLevel,
	kcBlackLevelDeltaH,
	kcBlackLevelDeltaV,
	kcWhiteLevel,
	kcDefaultScale,
	kcDefaultCropOrigin,
	kcDefaultCropSize,
	kcColorMatrix1,
	kcColorMatrix2,
	kcCameraCalibration1,
	kcCameraCalibration2,
	kcReductionMatrix1,
	kcReductionMatrix2,
	kcAnalogBalance,
	kcAsShotNeutral,
	kcAsShotWhiteXY,
	kcBaselineExposure,
	kcBaselineNoise,
	kcBaselineSharpness,
	kcBayerGreenSplit,
	kcLinearResponseLimit,
	kcCameraSerialNumber,
	kcLensInfo,
	kcChromaBlurRadius,
	kcAntiAliasStrength,
	kcShadowScale,
	kcDNGPrivateData,
	kcMakerNoteSafety,
	kcCalibrationIlluminant1,
	kcCalibrationIlluminant2,
	kcBestQualityScale,
	kcRawDataUniqueID,
	kcOriginalRawFileName,
	kcOriginalRawFileData,
	kcActiveArea,
	kcMaskedAreas,
	kcAsShotICCProfile,
	kcAsShotPreProfileMatrix,
	kcCurrentICCProfile,
	kcCurrentPreProfileMatrix,
	kcColorimetricReference,
	kcCameraCalibrationSignature,
	kcProfileCalibrationSignature,
	kcExtraCameraProfiles,
	kcAsShotProfileName,
	kcNoiseReductionApplied,
	kcProfileName,
	kcProfileHueSatMapDims,
	kcProfileHueSatMapData1,
	kcProfileHueSatMapData2,
	kcProfileToneCurve,
	kcProfileEmbedPolicy,
	kcProfileCopyright,
	kcForwardMatrix1,
	kcForwardMatrix2,
	kcPreviewApplicationName,
	kcPreviewApplicationVersion,
	kcPreviewSettingsName,
	kcPreviewSettingsDigest,
	kcPreviewColorSpace,
	kcPreviewDateTime,
	kcRawImageDigest,
	kcOriginalRawFileDigest,
	kcSubTileBlockSize,
	kcRowInterleaveFactor,
	kcProfileLookTableDims,
	kcProfileLookTableData,
	kcKodakKDCPrivateIFD,
	kcFirstSubIFD,
	kcLastSubIFD,
	kcFirstChainedIFD,
	kcLastChainedIFD,
	kcFirstMakerNoteIFD,
	kcLastMakerNoteIFD,
	kcCanonMakerNote,
	kcEpsonMakerNote,
	kcFujiMakerNote,
	kcKodakMakerNote,
	kcKodakMakerNote65280,
	kcLeicaMakerNote,
	kcMamiyaMakerNote,
	kcMinoltaMakerNote,
	kcNikonMakerNote,
	kcOlympusMakerNote,
	kcOlympusMakerNote8208,
	kcOlympusMakerNote8224,
	kcOlympusMakerNote8240,
	kcOlympusMakerNote8256,
	kcOlympusMakerNote8272,
	kcOlympusMakerNote12288,
	kcPanasonicMakerNote,
	kcPentaxMakerNote,
	kcPhaseOneMakerNote,
	kcRicohMakerNote,
	kcRicohMakerNoteCameraInfo,
	kcSonyMakerNote,
	kcSonyMakerNoteSubInfo,
	kcSonyPrivateIFD1,
	kcSonyPrivateIFD2,
	kcSonyPrivateIFD3A,
	kcSonyPrivateIFD3B,
	kcSonyPrivateIFD3C,
	kcCanonCRW,
	kcContaxRAW,
	kcContaxHeader,
	kcFujiRAF,
	kcFujiHeader,
	kcFujiRawInfo1,
	kcFujiRawInfo2,
	kcLeafMOS,
	kcMinoltaMRW,
	kcPanasonicRAW,
	kcFoveonX3F,
	kcJPEG,
	kcAdobePSD
};

static const char *TIFFTagStrings[kMaxTIFFTagCodes] = {
	"NewSubFileType",
	"SubFileType",
	"ImageWidth",
	"ImageLength",
	"BitsPerSample",
	"Compression",
	"PhotometricInterpretation",
	"Thresholding",
	"CellWidth",
	"CellLength",
	"FillOrder",
	"ImageDescription",
	"Make",
	"Model",
	"StripOffsets",
	"Orientation",
	"SamplesPerPixel",
	"RowsPerStrip",
	"StripByteCounts",
	"MinSampleValue",
	"MaxSampleValue",
	"XResolution",
	"YResolution",
	"PlanarConfiguration",
	"FreeOffsets",
	"FreeByteCounts",
	"GrayResponseUnit",
	"GrayResponseCurve",
	"ResolutionUnit",
	"TransferFunction",
	"Software",
	"DateTime",
	"Artist",
	"HostComputer",
	"Predictor",
	"WhitePoint",
	"PrimaryChromaticities",
	"ColorMap",
	"TileWidth",
	"TileLength",
	"TileOffsets",
	"TileByteCounts",
	"SubIFDs",
	"ExtraSamples",
	"SampleFormat",
	"JPEGTables",
	"JPEGProc",
	"JPEGInterchangeFormat",
	"JPEGInterchangeFormatLength",
	"YCbCrCoefficients",
	"YCbCrSubSampling",
	"YCbCrPositioning",
	"ReferenceBlackWhite",
	"XMP",
	"KodakCameraSerialNumber",
	"CFARepeatPatternDim",
	"CFAPattern",
	"BatteryLevel",
	"KodakDCRPrivateIFD",
	"Copyright",
	"ExposureTime",
	"FNumber",
	"IPTC_NAA",
	"LeafPKTS",
	"AdobeData",
	"ExifIFD",
	"ICCProfile",
	"ExposureProgram",
	"SpectralSensitivity",
	"GPSInfo",
	"ISOSpeedRatings",
	"OECF",
	"Interlace",
	"TimeZoneOffset",
	"SelfTimerMode",
	"ExifVersion",
	"DateTimeOriginal",
	"DateTimeDigitized",
	"ComponentsConfiguration",
	"CompressedBitsPerPixel",
	"ShutterSpeedValue",
	"ApertureValue",
	"BrightnessValue",
	"ExposureBiasValue",
	"MaxApertureValue",
	"SubjectDistance",
	"MeteringMode",
	"LightSource",
	"Flash",
	"FocalLength",
	"FlashEnergy",
	"SpatialFrequencyResponse",
	"Noise",
	"FocalPlaneXResolution",
	"FocalPlaneYResolution",
	"FocalPlaneResolutionUnit",
	"ImageNumber",
	"SecurityClassification",
	"ImageHistory",
	"SubjectArea",
	"ExposureIndex",
	"TIFF_EP_StandardID",
	"SensingMethod",
	"MakerNote",
	"UserComment",
	"SubsecTime",
	"SubsecTimeOriginal",
	"SubsecTimeDigitized",
	"AdobeLayerData",
	"FlashPixVersion",
	"ColorSpace",
	"PixelXDimension",
	"PixelYDimension",
	"RelatedSoundFile",
	"InteroperabilityIFD",
	"FlashEnergyExif",
	"SpatialFrequencyResponseExif",
	"FocalPlaneXResolutionExif",
	"FocalPlaneYResolutionExif",
	"FocalPlaneResolutionUnitExif",
	"SubjectLocation",
	"ExposureIndexExif",
	"SensingMethodExif",
	"FileSource",
	"SceneType",
	"CFAPatternExif",
	"CustomRendered",
	"ExposureMode",
	"WhiteBalance",
	"DigitalZoomRatio",
	"FocalLengthIn35mmFilm",
	"SceneCaptureType",
	"GainControl",
	"Contrast",
	"Saturation",
	"Sharpness",
	"DeviceSettingDescription",
	"SubjectDistanceRange",
	"ImageUniqueID",
	"Gamma",
	"PrintImageMatchingInfo",
	"DNGVersion",
	"DNGBackwardVersion",
	"UniqueCameraModel",
	"LocalizedCameraModel",
	"CFAPlaneColor",
	"CFALayout",
	"LinearizationTable",
	"BlackLevelRepeatDim",
	"BlackLevel",
	"BlackLevelDeltaH",
	"BlackLevelDeltaV",
	"WhiteLevel",
	"DefaultScale",
	"DefaultCropOrigin",
	"DefaultCropSize",
	"ColorMatrix1",
	"ColorMatrix2",
	"CameraCalibration1",
	"CameraCalibration2",
	"ReductionMatrix1",
	"ReductionMatrix2",
	"AnalogBalance",
	"AsShotNeutral",
	"AsShotWhiteXY",
	"BaselineExposure",
	"BaselineNoise",
	"BaselineSharpness",
	"BayerGreenSplit",
	"LinearResponseLimit",
	"CameraSerialNumber",
	"LensInfo",
	"ChromaBlurRadius",
	"AntiAliasStrength",
	"ShadowScale",
	"DNGPrivateData",
	"MakerNoteSafety",
	"CalibrationIlluminant1",
	"CalibrationIlluminant2",
	"BestQualityScale",
	"RawDataUniqueID",
	"OriginalRawFileName",
	"OriginalRawFileData",
	"ActiveArea",
	"MaskedAreas",
	"AsShotICCProfile",
	"AsShotPreProfileMatrix",
	"CurrentICCProfile",
	"CurrentPreProfileMatrix",
	"ColorimetricReference",
	"CameraCalibrationSignature",
	"ProfileCalibrationSignature",
	"ExtraCameraProfiles",
	"AsShotProfileName",
	"NoiseReductionApplied",
	"ProfileName",
	"ProfileHueSatMapDims",
	"ProfileHueSatMapData1",
	"ProfileHueSatMapData2",
	"ProfileToneCurve",
	"ProfileEmbedPolicy",
	"ProfileCopyright",
	"ForwardMatrix1",
	"ForwardMatrix2",
	"PreviewApplicationName",
	"PreviewApplicationVersion",
	"PreviewSettingsName",
	"PreviewSettingsDigest",
	"PreviewColorSpace",
	"PreviewDateTime",
	"RawImageDigest",
	"OriginalRawFileDigest",
	"SubTileBlockSize",
	"RowInterleaveFactor",
	"ProfileLookTableDims",
	"ProfileLookTableData",
	"KodakKDCPrivateIFD",
	"FirstSubIFD",
	"LastSubIFD",
	"FirstChainedIFD",
	"LastChainedIFD",
	"FirstMakerNoteIFD",
	"LastMakerNoteIFD",
	"CanonMakerNote",
	"EpsonMakerNote",
	"FujiMakerNote",
	"KodakMakerNote",
	"KodakMakerNote65280",
	"LeicaMakerNote",
	"MamiyaMakerNote",
	"MinoltaMakerNote",
	"NikonMakerNote",
	"OlympusMakerNote",
	"OlympusMakerNote8208",
	"OlympusMakerNote8224",
	"OlympusMakerNote8240",
	"OlympusMakerNote8256",
	"OlympusMakerNote8272",
	"OlympusMakerNote12288",
	"PanasonicMakerNote",
	"PentaxMakerNote",
	"PhaseOneMakerNote",
	"RicohMakerNote",
	"RicohMakerNoteCameraInfo",
	"SonyMakerNote",
	"SonyMakerNoteSubInfo",
	"SonyPrivateIFD1",
	"SonyPrivateIFD2",
	"SonyPrivateIFD3A",
	"SonyPrivateIFD3B",
	"SonyPrivateIFD3C",
	"CanonCRW",
	"ContaxRAW",
	"ContaxHeader",
	"FujiRAF",
	"FujiHeader",
	"FujiRawInfo1",
	"FujiRawInfo2",
	"LeafMOS",
	"MinoltaMRW",
	"PanasonicRAW",
	"FoveonX3F",
	"JPEG",
	"AdobePSD"
};


//////////////////////////////////////////////////////////////
//
// JFIF file structures 
//
//////////////////////////////////////////////////////////////
//
static const uint8_t kJFIFHeader [] =
{
	0xff, 0xd8,						// SOI
};

static const uint8_t kAPP0Header [] =
{
	0xff, 0xe0,						// APP0
	0x00, 0x10,						// length of 16 excluding APP0 marker
	0x4a, 0x46, 0x49, 0x46, 0x00,	// "JFIF"
	0x01, 0x01,						// Version 1.01
	0x00,							// No units, X and Y specify the pixel aspect ratio
	0x00, 0x01,						// Horizontal pixel density
	0x00, 0x01,						// Vertical pixel density
	0x00,							// Thumbnail horizontal pixel count (no thumbnail)
	0x00							// Thumbnail vertical pixel count (no thumbnail)
									// JPEG data stream follows
};

static const uint8_t kAPP1Header [] =
{
	0xff, 0xe1,							// APP1
	0x00, 0xf0,							// length of EXIF data area including size (must be overwritten)
	0x45, 0x78, 0x69, 0x66, 0x00, 0x00	// EXIF marker
										// TIFF header 0x49, 0x49 follows
};

static const uint8_t kJFIFFooter [] =
{
	0xff, 0xd9						// EOI
};


/*
void dumpSum(uint8_t *data, size_t len) {
	size_t i;
	int chkSum = 0;
	for (i = 0; i < len; i++) {
		chkSum+= *data;
		data++;
	}
	dng_local_printf::printf("Checksum: %i\n", chkSum);
}
*/

//////////////////////////////////////////////////////////////
//
// Utility File functions 
//
//////////////////////////////////////////////////////////////
// 
bool readBytes(FILE *fp, const size_t length, uint8_t *data)
{
	int c;
	size_t count;
	if (length == 0) return(0);
	count=0;
	
	switch (length)
	{
		case 0:
			break;
		case 1:
		{
			c=getc(fp);
			if (c == EOF)
				break;
			*data++=(uint8_t) c;
			count++;
		}
		case 2:
		{
			c=getc(fp);
			if (c == EOF)
				break;
			*data++=(uint8_t) c;
			count++;
		}
		default:
		{
			count=(size_t) fread(data,1,length,fp);
			break;
		}
	}
	return(count == length);
}


//////////////////////////////////////////////////////////////
//
// Utility String functions 
//
//////////////////////////////////////////////////////////////
// 

int compareBytes(const char *buffer, const char *string)
{
	size_t length = strlen(string);
	return strncmp(buffer, string, length);
}


//////////////////////////////////////////////////////////////
//
// Class initialiser and destructors 
//
//////////////////////////////////////////////////////////////
tiffThumb::tiffThumb(void)
{
	outputFileBuffer = NULL;
	magicCheck = 0;
	dumpTags = false;
}

tiffThumb::~tiffThumb()
{
	if (outputFileBuffer != NULL) {
		free(outputFileBuffer);
		outputFileBuffer = NULL;
	}
}



//////////////////////////////////////////////////////////////
//
// Class accessors 
//
//////////////////////////////////////////////////////////////

size_t tiffThumb::getWidth()
{
	return width;
}

size_t tiffThumb::getHeight()
{
	return length;
}

char *tiffThumb::getErrorString()
{
	return errorString;
}

void tiffThumb::setMagicNumberCheck(uint32_t magicNumber) {
	magicCheck = magicNumber;
}

void tiffThumb::setVerbose(bool val) {
	dumpTags = val;
}



//////////////////////////////////////////////////////////////
//
// Endian neutral reads 
//
//////////////////////////////////////////////////////////////
// 
uint16_t tiffThumb::getTIFF16(uint8_t buffer[]) {
	if (isLittleEndian) {
		return ((uint16_t) buffer[1])<<8 | (uint16_t) buffer[0];
	}
	else {
		return ((uint16_t) buffer[0])<<8 | (uint16_t) buffer[1];
	}
}

uint32_t tiffThumb::getTIFF32(uint8_t buffer[]) {
	if (isLittleEndian) {
		return ((uint32_t) buffer[3])<<24 | ((uint32_t) buffer[2])<<16 | ((uint32_t) buffer[1])<<8 | (uint32_t) buffer[0];	
	}
	else {
		return ((uint32_t) buffer[0])<<24 | ((uint32_t) buffer[1])<<16 | ((uint32_t) buffer[2])<<8 | (uint32_t) buffer[3];	
	}
}

void tiffThumb::writeTIFF32(uint8_t buffer[], uint32_t val) {
	if (isLittleEndian) {
		buffer[0] = (uint8_t) (val & 0xff);
		buffer[1] = (uint8_t) ((val>>8) & 0xff);
		buffer[2] = (uint8_t) ((val>>16) & 0xff);
		buffer[3] = (uint8_t) ((val>>24) & 0xff);
		}
	else {
		buffer[3] = (uint8_t) (val & 0xff);
		buffer[2] = (uint8_t) ((val>>8) & 0xff);
		buffer[1] = (uint8_t) ((val>>16) & 0xff);
		buffer[0] = (uint8_t) ((val>>24) & 0xff);		
	}	
}


void tiffThumb::writeTIFF16(uint8_t buffer[], uint32_t val) {
	if (isLittleEndian) {
		buffer[0] = (uint8_t) (val & 0xff);
		buffer[1] = (uint8_t) ((val>>8) & 0xff);
	}
	else {
		buffer[1] = (uint8_t) (val & 0xff);
		buffer[0] = (uint8_t) ((val>>8) & 0xff);		
	}	
}

void tiffThumb::writeJPEG16(uint8_t buffer[], uint32_t val) {
	buffer[1] = (uint8_t) (val & 0xff);
	buffer[0] = (uint8_t) ((val>>8) & 0xff);		
}

int tiffThumb::tagPass(TIFFDirectory *tag) {
	int retVal = 0;
	switch (getTIFF16(tag->tag)) {	
		case kcNewSubFileType:
		case kcSubFileType:
		case kcImageWidth:
		case kcImageLength:
		case kcBitsPerSample:
		case kcCompression:
		case kcPhotometricInterpretation:
		case kcMake:
		case kcModel:
			retVal = 1;
			break;
		case kcStripOffsets:
			retVal = 2;
			break;
		case kcOrientation:
		case kcSamplesPerPixel:
		case kcRowsPerStrip:
		case kcStripByteCounts:
		case kcPlanarConfiguration:
		case kcSoftware:
		case kcDateTime:
			retVal = 1;
			break;
		default:
			retVal = 0;
			break;			
	}
	return retVal;
}


uint8_t *tiffThumb::getData(uint8_t *mem, uint32_t dataOffset, size_t size) {
	if ((fp != NULL) && (mem != NULL)) {
		fseek(fp, dataOffset, SEEK_SET);
		if (!readBytes(fp, size, mem)) {
			strncpy(errorString, "Data Error reading file", kTIFFMaxStringLength*3-1);
			free(mem);
			return NULL;
		}
	}
	else {
		if (mem != NULL) free(mem);
		return NULL;
	}
	return mem;
}

uint8_t *tiffThumb::getTagData(TIFFDirectory *tag) {
	uint16_t tagType = getTIFF16(tag->tagType);
	if ((tagType < TIFFTypeByte) || (tagType > TIFFTypeComplex)) {
		// unknown
		return (uint8_t *) NULL;
	}
	size_t size = TIFFTaglengths[getTIFF16(tag->tagType)]*getTIFF32(tag->count);
	size = size < 4 ? 4 : size;
	uint8_t *mem = (uint8_t *) malloc(size);
	memset(mem, 0x0, size);
	if (mem == NULL) {
		return NULL;
	}
	if (size > 4) {
		uint32_t dataOffset = getTIFF32(tag->valueOffset);
		mem = getData(mem, dataOffset, size);
	}
	else {
		memcpy(mem, tag->valueOffset, size);
	}
	return mem;
}

void tiffThumb::writeTagData(uint8_t *theData, size_t size, uint8_t *dest, int *tagNum, uint32_t *metaOffset) {
	TIFFDirectory *directoryArray = (TIFFDirectory *) (dest+ sizeof(TIFFFile) + sizeof(TIFFIFDHeader));
	if (size > 4) {
		// The hard way....The data gets written to the metadata section
		// Write the offset to the tag itself
		writeTIFF32(directoryArray[*tagNum].valueOffset, *metaOffset);
		// Now write the data itself
		memcpy(dest + *metaOffset, theData, size);		
		// Update the offset for the next piece of data
		// Must be on a word boundry
		*metaOffset += ((size + 1) & 0xfffffffe);
	}
	else {
		// The easy way; just write to the value/offset of the tag itself
		memcpy(&(directoryArray[*tagNum].valueOffset), theData, size);
	}
}

void tiffThumb::copyTag(TIFFDirectory *tag, uint8_t *dest, int *tagNum, uint32_t *metaOffset, uint32_t *imageOffset, int skipBytes) {
	// Note that is metaOffset is null, then we just want the data copied....
	// In that case, we also strip the first two bytes, the SOI marker, off....
	uint16_t tagType = getTIFF16(tag->tagType);
	if ((tagType < TIFFTypeByte) || (tagType > TIFFTypeComplex) || (tagPass(tag) == 0)) {
		// unknown or not copied
		return;
	}
	
	// First copy the basic tag.. we'll override offsets later
	if (metaOffset != NULL) memcpy(dest+(*tagNum)*sizeof(TIFFDirectory) + sizeof(TIFFFile) + sizeof(TIFFIFDHeader), tag, sizeof(TIFFDirectory));
	
	// Secondly, read the metadata so we can modify offsets
	uint8_t *tagData = getTagData(tag);
	if (tagData == NULL) {
		throw "Could not read tag data";
	}
	size_t size = TIFFTaglengths[getTIFF16(tag->tagType)]*getTIFF32(tag->count);	
	
	uint8_t *mem = NULL;
	uint8_t *byteCountData = NULL;
	
	// Thirdly check if we need to copy image data
	try {
		if (getTIFF16(tag->tag) == kcStripOffsets) {
			unsigned int index = 0;
			byteCountData = getTagData(stripByteCounts);
			if (byteCountData != NULL) {
				for (index = 0; index < getTIFF32(stripByteCounts->count); index++) {
					uint32_t thisOffset = getTagUIntValue(tagData, tagType, index);
					if (metaOffset != NULL) {
						// Write the offset to the tag itself
						writeTIFF32((uint8_t *) &((uint32_t *) tagData)[index], *imageOffset);
					}
					// Now write the data itself
					uint32_t imageSize = getTagUIntValue(byteCountData, getTIFF16(stripByteCounts->tagType), index);
					mem = (uint8_t *) malloc(imageSize);
					if (mem == NULL) {
						throw "Could not assign memory to read image data";
					}
					mem = getData(mem, thisOffset, imageSize);
					if (mem != NULL) {

						// If this is JPEG, we don't want the SOI, so use skipBytes
						// But note we keep the EOI
						imageSize -= skipBytes;
						memcpy(dest + *imageOffset, mem + skipBytes, imageSize);							

						*imageOffset += imageSize;
						free(mem);
						mem = NULL;
					}
					else {
						throw "Tag get data failed";
					}				
				}
				free(byteCountData);
				byteCountData = NULL;
			}
		}
		else if (getTIFF16(tag->tag) == kcNewSubFileType) {
			// This is now the main image
			// Note that this is critically important - if we don't ID it as the main image, 
			// windows will throw up......
			writeTIFF32(tagData, 0);
		}
		
		// Finally copy metadata.....
		if (metaOffset != NULL) writeTagData(tagData, size, dest, tagNum, metaOffset);
		
		if (tagData != NULL) {
			free(tagData);
			tagData = NULL;
		}
	}
	catch (...) {
		// Clean up
		if (mem != NULL) {
			free(mem);
			mem = NULL;
		}
		if (byteCountData != NULL) {
			free(byteCountData);
			byteCountData = NULL;
		}
		if (tagData != NULL) {
			free(tagData);
			tagData = NULL;
		}
		// Tell the caller this went bad
		throw "Error trying to copy tag";
	}
	(*tagNum)++;
	return;
}



uint8_t tiffThumb::getTagByte(void *buffer, uint32_t index) {
	return ((uint8_t *) buffer)[index];
}

char tiffThumb::getTagASCII(void *buffer, uint32_t index) {
	return ((char *) buffer)[index];
}

uint16_t tiffThumb::getTagShort(void *buffer, uint32_t index) {
//	return ((uint16_t *) buffer)[index];
	return getTIFF16(((uint8_t *) buffer)+index*sizeof(uint16_t));
}

uint32_t tiffThumb::getTagLong(void *buffer, uint32_t index) {
//	return ((uint32_t *) buffer)[index];
	return getTIFF32(((uint8_t *) buffer)+index*sizeof(uint32_t));
}


uint32_t tiffThumb::getTagUIntValue(void *buffer, uint16_t tagType, uint32_t index) {
	switch (tagType) {
		case TIFFTypeByte:
		case TIFFTypeAscii:
		case TIFFTypeSByte:		
			return getTagByte(buffer, index);
			break;
		case TIFFTypeShort:
		case TIFFTypeSShort:		
			return getTagShort(buffer, index);
			break;
		case TIFFTypeLong:
		case TIFFTypeSLong:		
		case TIFFTypeIFD:		
			return getTagLong(buffer, index);
			break;
		case TIFFTypeRational:
		case TIFFTypeUndefined:		
		case TIFFTypeSRational:		
		case TIFFTypeFloat:		
		case TIFFTypeDouble:		
		case TIFFTypeUnicode:			
		case TIFFTypeComplex:			
		default:
			break;
	}
	return 0;
}

size_t tiffThumb::parseTag(TIFFDirectory *tag) {
	uint16_t tagType = getTIFF16(tag->tagType);
	uint8_t *tagData = NULL;
	
	// First get whatever data we need to get from the various tags
	switch (getTIFF16(tag->tag)) {	
		case kcNewSubFileType:
		case kcSubFileType:
			break;
		case kcImageWidth:
			tagData = getTagData(tag);
			if (tagData != NULL) {
				width = getTagUIntValue(tagData, tagType, 0);
				free(tagData);
			}
			break;
		case kcImageLength:
			tagData = getTagData(tag);
			if (tagData != NULL) {
				length = getTagUIntValue(tagData, tagType, 0);
				free(tagData);
			}
			break;
		case kcBitsPerSample:
			break;
		case kcCompression:
			tagData = getTagData(tag);
			if (tagData != NULL) {
				isJPEG = (getTagUIntValue(tagData, tagType, 0) == 7);
				free(tagData);
			}
			break;
		case kcPhotometricInterpretation:
		case kcMake:
		case kcModel:
		case kcStripOffsets:
		case kcOrientation:
		case kcSamplesPerPixel:
		case kcRowsPerStrip:
			break;
		case kcStripByteCounts:
			// Hold onto this for future use.....
			stripByteCounts = tag;
			break;
		case kcPlanarConfiguration:
		case kcSoftware:
		case kcDateTime:
		default:
			break;			
	}
	
	// Now work out how much memory this tag needs
	if ((tagType < TIFFTypeByte) || (tagType > TIFFTypeComplex) || (tagPass(tag) == 0)) {
		// Not included in our file
		return 0;
	}
	else {
		// Either just the directory, or directory plus additional
		size_t size = (TIFFTaglengths[tagType]*getTIFF32(tag->count) + 1) & 0xfffffffe;
		return size > 4 ? sizeof(TIFFDirectory) + size : sizeof(TIFFDirectory);
	}
}


void tiffThumb::dumpTagDescription(TIFFDirectory *tag) {
	
	unsigned int index = 0;
	uint16_t tagId = getTIFF16(tag->tag);
	while ((index < kMaxTIFFTagCodes) && (tagId != TIFFTagCodes[index])) {
		index++;
	}
	if (index >=  kMaxTIFFTagCodes) {
		printf("Unknown Tag: %i", tagId);
	}
	else {
		printf("Tag: %s", TIFFTagStrings[index]);
	}
	
	uint8_t *buf = getTagData(tag);
	
	if (buf != NULL) {
		
		switch (getTIFF16(tag->tagType)) {
				
			case TIFFTypeByte:
				printf(" Byte data");
				for (index = 0; index < getTIFF32(tag->count); index++) {
					printf(" %0x", getTagByte(buf, index));
				}
				break;
			case TIFFTypeAscii:
				printf(" ASCII data ");			
				for (index = 0; index < getTIFF32(tag->count); index++) {
					printf("%c", getTagASCII(buf, index));
				}		
				break;
			case TIFFTypeShort:
				printf(" Short data");			
				for (index = 0; index < getTIFF32(tag->count); index++) {
					printf(" %i", getTagShort(buf, index));
				}
				break;
			case TIFFTypeLong:
				printf(" Long data");			
				for (index = 0; index < getTIFF32(tag->count); index++) {
					printf(" %i", getTagLong(buf, index));
				}
				break;
			case TIFFTypeRational:
				printf(" Rational data");			
				break;
			case TIFFTypeSByte:
				printf(" SByte data");			
				break;
			case TIFFTypeUndefined:
				printf(" Undefined data");			
				break;
			case TIFFTypeSShort:
				printf(" SShort data");			
				break;
			case TIFFTypeSLong:
				printf(" SLong data");			
				break;
			case TIFFTypeSRational:
				printf(" SRational data");			
				break;
			case TIFFTypeFloat:
				printf(" Float data");			
				break;
			case TIFFTypeDouble:
				printf(" Double data");			
				break;
			case TIFFTypeIFD:
				printf(" IFD data");			
				break;
			case TIFFTypeUnicode:
				printf(" Unicode data");			
				break;
			case TIFFTypeComplex:
				printf(" Complex data");			
				break;
			default:
				break;
		}
		printf("\n");
		free(buf);
	}
}

void tiffThumb::getBuffer(size_t *size, uint8_t **buffer) {
	*buffer = outputFileBuffer;
	if (outputFileBuffer != NULL) {
		*size = totalMemory;
	}
	else {
		*size = 0;
	}
}

bool tiffThumb::parseFile (const char *in_file) {
	return parseFile(in_file, 0);
}

bool tiffThumb::getIsJPEG() {
	return isJPEG;
}

bool tiffThumb::parseFile (const char *in_file, int imageNumber)
{
	fp = NULL;
	bool status = true;
	TIFFFile *tiffFileHeader;
	if (outputFileBuffer != NULL) {
		free(outputFileBuffer);
		outputFileBuffer = NULL;
	}
	
	TIFFIFD *ifdHeader = NULL;
	TIFFIFD *ifdFullHeader = NULL;
	
	errorString[0] = 0x0;
	isLittleEndian = true;
	isJPEG = false;
	
	stripByteCounts = NULL;
	width = 0;
	length = 0;
	
	
	try {
		fp = fopen (in_file, "rb");
		if (fp == NULL) 
		{
			strncpy(errorString, "Could not open TIFF file", kTIFFMaxStringLength*3-1);
			return false;
		}
		
		// Check that this is a TIFF file.
		tiffFileHeader = (TIFFFile *) malloc(sizeof(TIFFFile));
		
		if (tiffFileHeader == NULL) {
			status = false;
		}
		
		if (status) {
			if (!readBytes(fp, sizeof(TIFFFile), (uint8_t *) tiffFileHeader)) {
				tiffFileHeader = NULL;
				strncpy(errorString, "TIFF file is too small to be valid", kTIFFMaxStringLength*3-1);
				status = false;
			}
		}
		
		if (getTIFF16(tiffFileHeader->byteOrder) == byteOrderII) {
			isLittleEndian = true;
		}
		else if (getTIFF16(tiffFileHeader->byteOrder) == byteOrderMM) {
			isLittleEndian = false;
		}
		else {
			status = false;
		}
		
		if (magicCheck != 0) {
			uint16_t magic = getTIFF16(tiffFileHeader->magicNumber);
			if (magic != magicCheck) {
				strncpy(errorString, "File magic number did not match", kTIFFMaxStringLength*3-1);
				status = false;
			}
		}
		uint32_t ifdOffset = getTIFF32(tiffFileHeader->ifdOffset);
		ifdHeader = (TIFFIFD *) malloc(sizeof(TIFFIFD));
		memset(ifdHeader, 0x0, sizeof(TIFFIFD));
		if (ifdHeader == NULL) {
			strncpy(errorString, "Could not allocate memory for header", kTIFFMaxStringLength*3-1);
			status = false;
		}
		
		while (status && (ifdOffset != 0x0)) {
			fseek(fp, ifdOffset, SEEK_SET);
			if (!readBytes(fp, sizeof(TIFFIFD), (uint8_t *) ifdHeader)) {
				strncpy(errorString, "IFD could not be read", kTIFFMaxStringLength*3-1);
				status = false;
			}
			else {
				size_t ifdSize = sizeof(TIFFIFDHeader) + sizeof(TIFFDirectory)*getTIFF16(ifdHeader->header.numEntries) + sizeof(TIFFIFDFooter);
				ifdFullHeader = (TIFFIFD *) malloc(ifdSize);
				memset(ifdFullHeader, 0x0, ifdSize);
				if (ifdFullHeader == NULL) {
					strncpy(errorString, "Could not allocate memory for full header", kTIFFMaxStringLength*3-1);
					status = false;
				}
				else {
					fseek(fp, ifdOffset, SEEK_SET);
					if (!readBytes(fp, ifdSize, (uint8_t *) ifdFullHeader)) {
						strncpy(errorString, "IFD Offset could not be read", kTIFFMaxStringLength*3-1);
						status = false;
						ifdOffset = 0x0;
					}
					else {
						// Here we have a full IFD
						uint16_t numDirectories = getTIFF16(ifdFullHeader->header.numEntries);
						size_t tiffTagMemory = 0;
						size_t stripTagMemory = 0;
						size_t jpegMemory = 0;
						int numTags = 0;
						int numStripTags = 0;
						// Pass One gets us memory size, and basic image data
						uint16_t dirCount = 0;
						struct TIFFDirectory *tag = (TIFFDirectory *) (&ifdFullHeader->dirZero);
						for (dirCount = 0; dirCount < numDirectories; dirCount++) {
							if (dumpTags) {
								dumpTagDescription(tag);
							}
							size_t mSize = parseTag(tag);
							if (mSize > 0) {
								numTags++;
								tiffTagMemory += mSize;
								if (getTIFF16(tag->tag) == kcStripOffsets) {
									stripTagMemory += mSize;
									numStripTags++;
								}
							}
							tag++;
						}
						// Do we have a file with a strip-based thumbnail?
						if (stripByteCounts == NULL) {
							// whoops.....not TIFF/EP compliant; let's not go here
							strncpy(errorString, "No strip based preview found", kTIFFMaxStringLength*3-1);
							status = false;
							ifdOffset = 0x0;
						}
						else {
							// Calculate the full memory requirement
							tiffTagMemory += sizeof(TIFFFile) + sizeof(TIFFIFDHeader) + sizeof(TIFFIFDFooter);
							if (isJPEG) {
								// Note that the JPEG data stream already has a EOI
								// Also, in the JPEG file, the JPEG data stream no longer requires
								// the TIFF style tags
								jpegMemory += sizeof(kJFIFHeader) + sizeof(kAPP0Header) + sizeof(kAPP1Header);
								tiffTagMemory -= stripTagMemory;
								numTags -= numStripTags;
							}

							totalMemory = tiffTagMemory + jpegMemory;
							
							uint8_t *buf = getTagData(stripByteCounts);
							if (buf != NULL) {
								unsigned int index = 0;
								for (index = 0; index < getTIFF32(stripByteCounts->count); index++) {
									totalMemory += (getTagUIntValue(buf, getTIFF16(stripByteCounts->tagType), index) + 1) & 0xfffffffe;
								}
								if (isJPEG) {
									// We're striping off the SOI in the JPEG data stream, so this is actually one less...
									totalMemory -= sizeof(kJFIFHeader);
								}
								free(buf);
								buf = NULL;
							}
							else {
								throw "Could not get tag data";
							}
							// Now we have total memory.......
							if (imageNumber == 0) {
								// Get us a buffer, and see that's ok.....
								outputFileBuffer = (uint8_t *) malloc(totalMemory);
								if (outputFileBuffer == NULL) {
									strncpy(errorString, "Could not allocate memory for tiff file buffer", kTIFFMaxStringLength*3-1);
									status = false;
									ifdOffset = 0x0;

								}
								else {
									// Here we have a buffer, and the source file has been parsed
									// Now write the file......
									// Note this section writes either a TIFF, or a JEPG file
									// In the JPEG case, esentially all of the TIFF/EXIF fields are embedded
									// into the JPEG file anyway, so in either case we write the same stuff, 
									// just in a different order
									memset(outputFileBuffer, 0, totalMemory);
									size_t jpegOffset = 0;
									if (isJPEG) {
										// If this is a JPEG file, then we need to write the headers...
										// JFIF header
										memcpy(outputFileBuffer + jpegOffset, kJFIFHeader, sizeof(kJFIFHeader));
										jpegOffset += sizeof(kJFIFHeader);
										// Then the APP0 header.....
										memcpy(outputFileBuffer + jpegOffset, kAPP0Header, sizeof(kAPP0Header));									
										jpegOffset += sizeof(kAPP0Header);
										// Then the APP1 header with the EXIF data.....
										memcpy(outputFileBuffer + jpegOffset, kAPP1Header, sizeof(kAPP1Header));
										// Write the size of the APP1 and the EXIF data
										writeJPEG16(outputFileBuffer + jpegOffset + sizeof(kJFIFHeader), tiffTagMemory + sizeof(kAPP1Header) - sizeof(kJFIFHeader));
										jpegOffset += sizeof(kAPP1Header);
									}
									
									// Now write the TIFF file structure
									// If the JPEG case, this is embedded inside an APP1/EXIF header
									// First the header....
									
									writeTIFF32(tiffFileHeader->ifdOffset, sizeof(TIFFFile));
									memcpy(outputFileBuffer + jpegOffset, tiffFileHeader, sizeof(TIFFFile));
									
									// Then the IFD
									struct TIFFIFD *newIFD = (struct TIFFIFD *) (outputFileBuffer + jpegOffset + sizeof(TIFFFile));
									writeTIFF16(newIFD->header.numEntries, numTags);
									
									
									// Then the actual directory entries
									// For a TIFF file, this goes in the main portion of the file. For JPEG
									// file, it goes in the EXIF/APP1 section
									dirCount = 0;
									tag = (TIFFDirectory *) (&ifdFullHeader->dirZero);
									int tagNum = 0;
									uint32_t metaOffset = sizeof(TIFFFile) + sizeof(TIFFIFDHeader) + numTags*sizeof(TIFFDirectory) + sizeof(TIFFIFDFooter);
									uint32_t imageOffset = tiffTagMemory;
									for (dirCount = 0; dirCount < numDirectories; dirCount++) {
										if (!isJPEG || (getTIFF16(tag->tag) != kcStripOffsets)) {
											copyTag(tag, outputFileBuffer + jpegOffset, &tagNum, &metaOffset, &imageOffset, 0);
										}
										tag++;
									}					
									
									// Write IFD footer
									writeTIFF32((uint8_t *) tag, 0x0);
									
									ifdOffset = getTIFF32(((TIFFIFDFooter *) tag)->ifdOffset);
									
									if (isJPEG) {
										// Then write the actual image data extracted from StripOffsets
										dirCount = 0;
										tag = (TIFFDirectory *) (&ifdFullHeader->dirZero);
										int tagNum = 0;
										// Adjust to the location of the JPEG data
										imageOffset += jpegOffset;
										int skipBytes = sizeof(kJFIFHeader);
										for (dirCount = 0; dirCount < numDirectories; dirCount++) {
											if (getTIFF16(tag->tag) == kcStripOffsets) {
												copyTag(tag, outputFileBuffer, &tagNum, NULL, &imageOffset, skipBytes);
												// Only skip the first time
												skipBytes = 0;
											}
											tag++;
										}									
									}
									// Next image in source file
									imageNumber--;
								}
							}
							else {
								imageNumber--;
							}	
						}
					}
				}
			}			  
		}
	}
	catch (char *err) {
		if (errorString == NULL) {
			strncpy(errorString, err, kTIFFMaxStringLength*3-1);
			strcat(errorString, " while processing image");
		}
		status = false;
	}
	catch (...) {
		if (errorString == NULL) strncpy(errorString, "I/O error while processing image", kTIFFMaxStringLength*3-1);
		status = false;
	}
	
	if (ifdFullHeader != NULL) {
		free(ifdFullHeader);
		ifdFullHeader = NULL;
	}	
	
	if (ifdHeader != NULL) {
		free(ifdHeader);
		ifdHeader = NULL;
	}
	
	if (tiffFileHeader != NULL) {
		free(tiffFileHeader);
		tiffFileHeader = NULL;
	}
	fclose(fp);
	fp = NULL;
	return status;
}
