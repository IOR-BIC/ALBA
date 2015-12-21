/*=========================================================================

 Program: MAF2
 Module: vtkDicomUnPackerTest
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "vtkDicomUnPacker.h"
#include "vtkDicomUnPackerTest.h"

#include "mafString.h"
#include "vtkMAFSmartPointer.h"

#include "vtkDirectory.h"
#include "vtkImageData.h"
#include "vtkPlaneSource.h"

#define DELTA 0.01

//-------------------------------------------------------------------------
void vtkDicomUnPackerTest::ReadTest()
//-------------------------------------------------------------------------
{
	//Inizialize DICOM DIRECTORY
	vtkMAFSmartPointer<vtkDirectory> DicomDirectory;
	mafString DicomPath=MAF_DATA_ROOT;
	DicomPath<<"/DicomUnpacker/TestDicomUnpacker";
	DicomDirectory->Open(DicomPath);
  mafString DictonaryFilename=MAF_DATA_ROOT;  
	DictonaryFilename<<"/Dictionaries/dicom3.dic";
	int NumberOfImage = 0;
	double MinScalar = 9999.0;
	double MaxScalar = -9999.0;
	double MinZ = 9999.0;
	double MaxZ = -9999.0;
	for (int i=0; i < DicomDirectory->GetNumberOfFiles(); i++)
	{
		if ((strcmp(DicomDirectory->GetFile(i),".") == 0) || (strcmp(DicomDirectory->GetFile(i),"..") == 0) || (strcmp(DicomDirectory->GetFile(i),"CVS") == 0)) 
		{
			continue;
		}
		else
		{
			//Count the images
			NumberOfImage++;
			// Append of the path at the dicom file
			mafString CurrentFile = DicomDirectory->GetFile(i);
			mafString FilePath=DicomPath;
			FilePath<<"/";
			FilePath<<CurrentFile;
			
			//Inizialize DICOM READER
			vtkMAFSmartPointer<vtkDicomUnPacker> DicomReader;
			DicomReader->SetFileName(FilePath);
			DicomReader->UseDefaultDictionaryOff();
			DicomReader->SetDictionaryFileName(DictonaryFilename);
			DicomReader->UpdateInformation();
			DicomReader->Update();
			double DicomRange[2];
			DicomReader->GetOutput()->GetScalarRange(DicomRange);
			MinScalar = min(MinScalar,DicomRange[0]);
			MaxScalar = max(MaxScalar,DicomRange[1]);
			double Bounds[6];
			DicomReader->GetOutput()->GetBounds(Bounds);
			//Check dimensions XY
			CPPUNIT_ASSERT(Bounds[0]+DELTA>0 && Bounds[0]-DELTA<0);
			CPPUNIT_ASSERT(Bounds[1]+DELTA>379.26 && Bounds[1]-DELTA<379.26);
			CPPUNIT_ASSERT(Bounds[2]+DELTA>0 && Bounds[2]-DELTA<0);
			CPPUNIT_ASSERT(Bounds[3]+DELTA>379.26 && Bounds[3]-DELTA<379.26);
			MaxZ=max(MaxZ,Bounds[5]);
			MinZ=min(MinZ,Bounds[4]);

			//Check modality
			CPPUNIT_ASSERT(strcmp( DicomReader->GetModality(), "CT" ) == 0);
		}
	}
	//Check dimension Z
	CPPUNIT_ASSERT(MinZ+DELTA>-40.5 && MinZ-DELTA<-40.5);
	CPPUNIT_ASSERT(MaxZ+DELTA>-36.5 && MaxZ-DELTA<-36.5);
	//Chcek scalar range
	CPPUNIT_ASSERT(MinScalar==-3024 && MaxScalar==1699);
	//Check the reader has read all images
	CPPUNIT_ASSERT(NumberOfImage==3);
}