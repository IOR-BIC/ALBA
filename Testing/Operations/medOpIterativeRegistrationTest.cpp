/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpIterativeRegistrationTest.cpp,v $
Language:  C++
Date:      $Date: 2011-03-07 14:38:52 $
Version:   $Revision: 1.1.2.1 $
Authors:   Alberto Losi
==========================================================================
Copyright (c) 2002/2004 
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
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------



#include <cppunit/config/SourcePrefix.h>
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "medOpIterativeRegistrationTest.h"
#include "medOpIterativeRegistration.h"
#include "mafVMEGeneric.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

//-----------------------------------------------------------
void medOpIterativeRegistrationTest::setUp() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpIterativeRegistrationTest::tearDown() 
//-----------------------------------------------------------
{
}
//-----------------------------------------------------------
void medOpIterativeRegistrationTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpIterativeRegistration *op = new medOpIterativeRegistration();
  cppDEL(op);
}
// //-----------------------------------------------------------
// void medOpIterativeRegistrationTest::TestOnEvent()
// //-----------------------------------------------------------
// {
//   // Cannot test: All GUI Interaction
// }
//-----------------------------------------------------------
void medOpIterativeRegistrationTest::TestCopy()
//-----------------------------------------------------------
{
  medOpIterativeRegistration *op = new medOpIterativeRegistration();
  
  mafOp *op_cpy = op->Copy();

  CPPUNIT_ASSERT(op_cpy->IsA("medOpIterativeRegistration"));

  cppDEL(op);
  cppDEL(op_cpy);
}
//-----------------------------------------------------------
void medOpIterativeRegistrationTest::TestAccept()
//-----------------------------------------------------------
{
  medOpIterativeRegistration *op = new medOpIterativeRegistration();

  mafVMEGeneric *vme;
  mafNEW(vme);

  mafVMESurface *surf;
  mafNEW(surf);

  mafVMEVolumeGray *vol;
  mafNEW(vol);

  CPPUNIT_ASSERT(op->Accept(NULL) == false); // Always returns true
  CPPUNIT_ASSERT(op->Accept(vme) == false);

  CPPUNIT_ASSERT(op->Accept(surf) == true);
  CPPUNIT_ASSERT(op->Accept(vol) == true);

  mafDEL(vme);
  mafDEL(surf);
  mafDEL(vol);
  
  cppDEL(op);
}
// //-----------------------------------------------------------
// void medOpIterativeRegistrationTest::TestOpRun()
// //-----------------------------------------------------------
// {
//   Cannot test: All GUI creation
// }
//-----------------------------------------------------------
void medOpIterativeRegistrationTest::TestVmeAccept()
//-----------------------------------------------------------
{
  mafVMEGeneric *vme;
  mafNEW(vme);

  CPPUNIT_ASSERT(medOpIterativeRegistration::VmeAccept(NULL) == false);
  CPPUNIT_ASSERT(medOpIterativeRegistration::VmeAccept(vme) == true);

  mafDEL(vme);
}
//-----------------------------------------------------------
void medOpIterativeRegistrationTest::TestOpDoUndo()
//-----------------------------------------------------------
{
  medOpIterativeRegistration *op = new medOpIterativeRegistration();

  mafVMESurface *sourceVme;
  mafNEW(sourceVme);

  vtkTransform *transf;
  vtkNEW(transf);

  transf->Translate(10,10,10);
  //transf->RotateX(45);

  vtkMatrix4x4 *rotoTrasl;
  rotoTrasl = transf->GetMatrix();
  
  op->m_SourceVME = sourceVme;
  vtkNEW(op->m_RegistrationMatrix);
  op->m_RegistrationMatrix->DeepCopy(rotoTrasl);
  vtkNEW(op->m_UndoSourceAbsPose);
  op->m_UndoSourceAbsPose->DeepCopy(sourceVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

  op->OpDo();

  vtkMatrix4x4 * absPose = sourceVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix();

  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
       CPPUNIT_ASSERT(rotoTrasl->GetElement(i,j) == absPose->GetElement(i,j));
    }
  }

  op->OpUndo();

  absPose = sourceVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix();

  vtkMatrix4x4 *identity;
  vtkNEW(identity);
  identity->Identity();

  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      CPPUNIT_ASSERT(identity->GetElement(i,j) == absPose->GetElement(i,j));
    }
  }

  vtkDEL(sourceVme);
  vtkDEL(transf);
  vtkDEL(identity);
  cppDEL(op);
}