/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkImageUnPackerTest
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

#include "vtkImageUnPackerTest.h"
#include "vtkImageUnPacker.h"

#include "vtkPackedImage.h"
#include "vtkImageData.h"


//-------------------------------------------------------------------------
void vtkImageUnPackerTest::BeforeTest()
//-------------------------------------------------------------------------
{
  m_Filter = NULL;
}

//-------------------------------------------------------------------------
void vtkImageUnPackerTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  m_Filter = vtkImageUnPacker::New();
  m_Filter->Delete();
}
//-------------------------------------------------------------------------
void vtkImageUnPackerTest::TestExecute()
//-------------------------------------------------------------------------
{
  m_Filter = vtkImageUnPacker::New();

  vtkPackedImage *input = vtkPackedImage::New();
  unsigned char *ptr = new unsigned char[100];
  for (int i=0;i<100;i++)
  {
    ptr[i] = 0;
  }

  input->SetArray(ptr,100,true);

  m_Filter->SetInput(input);
  m_Filter->Update();

  //The filter update method is empty so the output values are the same of the default values
  for (int i=0;i<6;i++)
  {
  	CPPUNIT_ASSERT( m_Filter->GetOutput()->GetExtent()[i] == 0 );
  }

  CPPUNIT_ASSERT( m_Filter->GetOutput()->GetScalarType() == VTK_UNSIGNED_CHAR );

  CPPUNIT_ASSERT( m_Filter->GetOutput()->GetNumberOfScalarComponents() == 1 );


  m_Filter->Delete();
  input->Delete();
  delete []ptr;

}