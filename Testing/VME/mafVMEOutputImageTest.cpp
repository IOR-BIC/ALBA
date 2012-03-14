/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEOutputImageTest.cpp,v $
Language:  C++
Date:      $Date: 2009-04-27 10:40:47 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "mafVMEOutputImageTest.h"

#include "mafVMEOutputImage.h"
#include "mafVMEImage.h"

#include "vtkMAFSmartPointer.h"
#include "vtkImageData.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void mafVMEOutputImageTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputImageTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void mafVMEOutputImageTest::tearDown()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafVMEOutputImageTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputImage outputLandmarkCloud;
}
//----------------------------------------------------------------------------
void mafVMEOutputImageTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafVMEOutputImage *outputImage = new mafVMEOutputImage();
  cppDEL(outputImage);
}
//----------------------------------------------------------------------------
void mafVMEOutputImageTest::TestGetImageData()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkImageData> im;
  im->SetDimensions(5, 5, 1);
  im->SetOrigin(0, 0, 0);
  im->SetSpacing(1,1,1);
  im->Update();

  mafSmartPointer<mafVMEImage> image;
  image->SetData(im,0.0);
  image->Update();

  mafVMEOutputImage *output = mafVMEOutputImage::SafeDownCast(image->GetOutput());
  output->GetVTKData()->Update();
  output->Update();
  
  m_Result = output != NULL;
  TEST_RESULT;

  m_Result = output->GetImageData()->GetNumberOfPoints() == im->GetNumberOfPoints();
  TEST_RESULT;

  m_Result = output->GetImageData()->GetNumberOfCells() == im->GetNumberOfCells();
  TEST_RESULT;

  m_Result = output->GetImageData()->GetDimensions()[0] == im->GetDimensions()[0] && output->GetImageData()->GetDimensions()[1] == im->GetDimensions()[1] && output->GetImageData()->GetDimensions()[2] == im->GetDimensions()[2];
  TEST_RESULT;

  m_Result = output->GetImageData()->GetOrigin()[0] == im->GetOrigin()[0] && output->GetImageData()->GetOrigin()[1] == im->GetOrigin()[1] && output->GetImageData()->GetOrigin()[2] == im->GetOrigin()[2];
  TEST_RESULT;
}
