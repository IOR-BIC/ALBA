/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputImageTest
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "albaVMEOutputImageTest.h"

#include "albaVMEOutputImage.h"
#include "albaVMEImage.h"

#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//----------------------------------------------------------------------------
void albaVMEOutputImageTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaVMEOutputImageTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputImage outputLandmarkCloud;
}
//----------------------------------------------------------------------------
void albaVMEOutputImageTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaVMEOutputImage *outputImage = new albaVMEOutputImage();
  cppDEL(outputImage);
}
//----------------------------------------------------------------------------
void albaVMEOutputImageTest::TestGetImageData()
//----------------------------------------------------------------------------
{
  vtkALBASmartPointer<vtkImageData> im;
  im->SetDimensions(5, 5, 1);
  im->SetOrigin(0, 0, 0);
  im->SetSpacing(1,1,1);

  albaSmartPointer<albaVMEImage> image;
  image->SetData(im,0.0);
  image->Update();

  albaVMEOutputImage *output = albaVMEOutputImage::SafeDownCast(image->GetOutput());
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
