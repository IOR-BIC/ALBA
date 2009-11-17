/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkImageUnPackerTest.cpp,v $
Language:  C++
Date:      $Date: 2009-11-17 08:53:33 $
Version:   $Revision: 1.1.2.1 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "vtkImageUnPackerTest.h"
#include "vtkImageUnPacker.h"

#include "vtkPackedImage.h"
#include "vtkImageData.h"


//-------------------------------------------------------------------------
void vtkImageUnPackerTest::setUp()
//-------------------------------------------------------------------------
{
  m_Filter = NULL;
}
//-------------------------------------------------------------------------
void vtkImageUnPackerTest::tearDown()
//-------------------------------------------------------------------------
{
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

  input->SetArray(ptr,100,TRUE);

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