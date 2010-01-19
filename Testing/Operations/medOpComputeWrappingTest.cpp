/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpComputeWrappingTest.cpp,v $
Language:  C++
Date:      $Date: 2010-01-19 12:44:42 $
Version:   $Revision: 1.1.2.1 $
Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2007
CINECA - Interuniversity Consortium (www.cineca.it)
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

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpComputeWrappingTest.h"
#include "medOpComputeWrapping.h"

#include "mafVMEVolumeGray.h"
#include "medVMEComputeWrapping.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpComputeWrappingTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpComputeWrapping *create=new medOpComputeWrapping();
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeWrappingTest::TestOpRun() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;

  medOpComputeWrapping *create=new medOpComputeWrapping();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  medVMEComputeWrapping *labeledvolume = NULL;
	labeledvolume = medVMEComputeWrapping::SafeDownCast(create->GetOutput());
  
  CPPUNIT_ASSERT(labeledvolume != NULL);

  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeWrappingTest::TestCopy() 
//-----------------------------------------------------------
{
  medOpComputeWrapping *create=new medOpComputeWrapping();
  medOpComputeWrapping *create2 = medOpComputeWrapping::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  mafDEL(create2);
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeWrappingTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;

  medOpComputeWrapping *create=new medOpComputeWrapping();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeWrappingTest::TestOpDo() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMEVolumeGray> volume;
  int numOfChildren = volume->GetNumberOfChildren();
  CPPUNIT_ASSERT(numOfChildren == 0);

  medOpComputeWrapping *create=new medOpComputeWrapping();
  create->TestModeOn();
  create->SetInput(volume);
  create->OpRun();

  create->OpDo();
  numOfChildren = volume->GetNumberOfChildren();

  CPPUNIT_ASSERT(numOfChildren == 1);
  mafDEL(create);
}