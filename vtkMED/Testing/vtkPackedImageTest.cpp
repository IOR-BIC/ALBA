/*=========================================================================

 Program: MAF2Medical
 Module: vtkPackedImageTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------
#include "vtkPackedImage.h"
#include "vtkPackedImageTest.h"


#include <cppunit/config/SourcePrefix.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"

#include "vtkBMPReader.h"
#include "vtkTexture.h"
#include "vtkPlaneSource.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkWindowToImageFilter.h"
#include "vtkImageMathematics.h"
#include "vtkImageData.h"
#include "vtkJPEGWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPointData.h"

#include "mafString.h"

#ifndef VTK_IMG_PACK_DICOM
#define VTK_IMG_PACK_DICOM 2
#endif

void vtkPackedImageTest::setUp()
{
}

void vtkPackedImageTest::tearDown()
{
}


//------------------------------------------------------------
void vtkPackedImageTest::TestDynamicAllocation()
//------------------------------------------------------------
{
  vtkPackedImage *packed = vtkPackedImage::New();
  CPPUNIT_ASSERT(packed->IsA("vtkPackedImage"));
  vtkDEL(packed);
}
//--------------------------------------------
void vtkPackedImageTest::TestPrintSelf()
//--------------------------------------------
{
   
  vtkPackedImage *packed = vtkPackedImage::New();
  packed->Print(std::cout);
  vtkDEL(packed);
}

//--------------------------------------------
void vtkPackedImageTest::TestSetGetPackType()
//--------------------------------------------
{
  vtkPackedImage *packed = vtkPackedImage::New();
  packed->SetPackType(VTK_IMG_PACK_DICOM);
  CPPUNIT_ASSERT(VTK_IMG_PACK_DICOM == packed->GetPackType());
  vtkDEL(packed);
}

//--------------------------------------------
void vtkPackedImageTest::TestSetGetImageSize()
//--------------------------------------------
{
  vtkPackedImage *packed = vtkPackedImage::New();
  packed->SetImageSize(256);
  CPPUNIT_ASSERT(256 == packed->GetImageSize());
  vtkDEL(packed);
}

//--------------------------------------------
void vtkPackedImageTest::TestImportImage_GetImageImporter()
//--------------------------------------------
{
  unsigned char progression[10] = {0,1,2,3,4,5,6,7,8,9};
  unsigned char reverseProgression[10] = {9,8,7,6,5,4,3,2,1,0};

  vtkPackedImage *packed = vtkPackedImage::New();
  packed->ImportImage(progression, 10, 0); //not copy
  unsigned char *point = packed->GetImagePointer();

  int count = 0, size = 10;
  for(;count < size; count++)
  {
    CPPUNIT_ASSERT(progression[count] == point[count]);
  }

  packed->ImportImage(reverseProgression, 10, 1); // copy
  unsigned char *pointCopy = packed->GetImagePointer();

  count = 0;
  for(;count < size; count++)
  {
    CPPUNIT_ASSERT(reverseProgression[count] == pointCopy[count]);
  }

  vtkDEL(packed);

  //
}