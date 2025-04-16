/*=========================================================================

Program: ALBA (Agile Library for Biomedical Applications)
Module: albaOpImporterScancoImageIO
Authors: Gianlugi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or


This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

This file is an adaption to ALBA/VTK of https://github.com/KitwareMedical/ITKIOScanco
=========================================================================*/


/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*=========================================================================

  Program: DICOM for VTK

  Copyright (c) 2015 David Gobbi
  All rights reserved.
  See Copyright.txt or http://dgobbi.github.io/bsd3.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * David's notes from vtkScancoCTReader
 * Read SCANCO ISQ and AIM medical image files
 *
 * This class reads ISQ and AIM files, which are used for high-resolution
 * computed tomography.  The information that it provides uses different
 * units as compared to the original files: all distances are given in
 * millimeters (instead of micrometers), times are given in milliseconds
 * (instead of microseconds), voltage and current given in kV and mA
 * (instead of volts and microamps).  If the scanner was calibrated, then
 * the data values can be converted to calibrated units.  To convert
 * to linear attenuation coefficients [cm^-1], simply divide the data
 * values by the MuScaling.  To convert to density values, multiply
 * the data values by the m_RescaleSlope and add the m_RescaleIntercept.
 * To convert to Hounsfield units, multiply by 1000/(MuScaling*m_MuWater)
 * and subtract 1000.
 *
 * Created at the Calgary Image Processing and Analysis Centre (CIPAC).
 */
#ifndef albaOpImporterScancoImageIO_H
#define albaOpImporterScancoImageIO_H


#include <fstream>
#include "albaOp.h"
class albaTagArray;

class albaOpImporterScancoImageIO : public albaOp
{
public:
	/** Run-time type information (and related methods). */
	albaTypeMacro(albaOpImporterScancoImageIO, albaOp);

	albaOpImporterScancoImageIO(const wxString &label= "Importer SCANCO");
	~albaOpImporterScancoImageIO();
	
	albaOp* Copy();

	/** Runs Operation. */
	void OpRun();

	
	/** The different types of ImageIO's can support data of varying
	 * dimensionality. For example, some file formats are strictly 2D
	 * while others can support 2D, 3D, or even n-D. This method returns
	 * true/false as to whether the ImageIO can support the dimension
	 * indicated. */
	bool SupportsDimension(unsigned long dimension) {
		return (dimension == 3);
	}
	/*-------- This part of the interfaces deals with reading data. ----- */

	/** Determine the file type. Returns true if this ImageIO can read the
	 * file specified. */
	bool CanReadFile(const char *);

	/** Set the spacing and dimension information for the set filename. */
	int ReadImageInformation();

	/** Reads the data from disk into the memory buffer provided. */
	void Read(void * buffer);

	/*-------- This part of the interfaces deals with writing data. ----- */

	/** Get a string that states the version of the file header.
	 * Max size: 16 characters. */
	const char *GetVersion() const
	{
		return this->m_Version;
	}
	void SetVersion(const char * version)
	{
		strncpy(this->m_Version, version, 18);
	}

	const char *GetCalibrationData() const
	{
		return this->m_CalibrationData;
	}
	void SetCalibrationData(const char * calibrationData)
	{
		strncpy(this->m_CalibrationData, calibrationData, 66);
	}

	const char *GetRescaleUnits() const
	{
		return this->m_RescaleUnits;
	}
	void SetRescaleUnits(const char * rescaleUnits)
	{
		strncpy(this->m_RescaleUnits, rescaleUnits, 18);
	}


	/** Set / Get the minimum and maximum values */
	const double *GetDataRange() const
	{
		return this->m_DataRange;
	}
	void SetDataRange(const double * dataRange)
	{
		this->m_DataRange[0] = dataRange[0];
		this->m_DataRange[1] = dataRange[1];
	}


	/** Get a string that states patient name.
	 * Max size: 40 characters. */
	const char *GetPatientName() const
	{
		return this->m_PatientName;
	}
	void SetPatientName(const char * patientName)
	{
		strncpy(this->m_PatientName, patientName, 42);
	}

	const char *GetCreationDate() const
	{
		return this->m_CreationDate;
	}
	void SetCreationDate(const char * creationDate)
	{
		strncpy(this->m_CreationDate, creationDate, 32);
	}

	const char *GetModificationDate() const
	{
		return this->m_ModificationDate;
	}
	void SetModificationDate(const char * modificationDate)
	{
		strncpy(this->m_ModificationDate, modificationDate, 32);
	}

	
	/** Returns FileName */
	albaString GetFileName() const { return m_FileName; }

	/** Sets FileName */
	void SetFileName(albaString fileName) { m_FileName = fileName; }

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node) { return true; };

protected:

	int ImportFile();

	/** Check the file header to see what type of file it is.
	 *
	 *  Return values are: 0 if unrecognized, 1 if ISQ/RAD,
	 *  2 if AIM 020, 3 if AIM 030.
	 */
	int	CheckVersion(const char header[16]);

	/** Convert char data to 32-bit int (little-endian). */
	static int DecodeInt(const void * data);
	/** Convert 32-bit int (little-endian) to char data. */
	static void	EncodeInt(int data, void * target);

	/** Convert char data to float (single precision). */
	static float DecodeFloat(const void * data);

	/** Convert char data to float (double precision). */
	static double DecodeDouble(const void * data);

	//! Convert a VMS timestamp to a calendar date.
	void DecodeDate(const void * data, int &year, int &month, int &day, int &hour, int &minute, int &second, int &millis);
	//! Convert the current calendar date to a VMS timestamp and store in target
	void EncodeDate(void * target);

	//! Strip a string by removing trailing whitespace.
	/*!
	 *  The dest must have a size of at least l+1.
	 */
	static void StripString(char * dest, const char * source, size_t length);
	static void PadString(char * dest, const char * source, size_t length);

	void InitializeHeader();

	int	ReadISQHeader(std::ifstream * file, unsigned long bytesRead);

	int	ReadAIMHeader(std::ifstream * file, unsigned long bytesRead);

	void PopulateMetaDataDictionary();

	size_t GetScalarSize();
	
	// Header information
	char   m_Version[18];
	char   m_PatientName[42];
	int    m_PatientIndex;
	int    m_ScannerID;
	char   m_CreationDate[32];
	char   m_ModificationDate[32];
	int    ScanDimensionsPixels[3];
	double ScanDimensionsPhysical[3];
	double m_SliceThickness;
	double m_SliceIncrement;
	double m_StartPosition;
	double m_EndPosition;
	double m_ZPosition;
	double m_DataRange[2];
	double m_MuScaling;
	int    m_NumberOfSamples;
	int    m_NumberOfProjections;
	double m_ScanDistance;
	double m_SampleTime;
	int    m_ScannerType;
	int    m_MeasurementIndex;
	int    m_Site;
	int    m_ReconstructionAlg;
	double m_ReferenceLine;
	double m_Energy;
	double m_Intensity;
	int    m_RescaleType;
	char   m_RescaleUnits[18];
	char   m_CalibrationData[66];
	double m_RescaleSlope;
	double m_RescaleIntercept;
	double m_MuWater;
	char * m_RawHeader;

	// The compression mode, if any.
	int m_Compression;

	bool m_HeaderInitialized;

	long  m_HeaderSize;

	int m_Dims[3];
	double m_Spacing[3];
	double m_Origin[3];
	int m_ScalarsType;
	albaTagArray *m_TagArray;


	albaString m_FileName;
};

#endif // albaOpImporterScancoImageIO
