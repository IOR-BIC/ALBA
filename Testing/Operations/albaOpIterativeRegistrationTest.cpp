/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpIterativeRegistrationTest
 Authors: Alberto Losi
 
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
#include <wx/txtstrm.h>
#include <wx/tokenzr.h>
#include <wx/wfstream.h>

#include "albaOpIterativeRegistrationTest.h"
#include "albaOpIterativeRegistration.h"
#include "albaVMEGeneric.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"

//-----------------------------------------------------------
void albaOpIterativeRegistrationTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  albaOpIterativeRegistration *op = new albaOpIterativeRegistration();
  cppDEL(op);
}
// //-----------------------------------------------------------
// void albaOpIterativeRegistrationTest::TestOnEvent()
// //-----------------------------------------------------------
// {
//   // Cannot test: All GUI Interaction
// }
//-----------------------------------------------------------
void albaOpIterativeRegistrationTest::TestCopy()
//-----------------------------------------------------------
{
  albaOpIterativeRegistration *op = new albaOpIterativeRegistration();
  
  albaOp *op_cpy = op->Copy();

  CPPUNIT_ASSERT(op_cpy->IsA("albaOpIterativeRegistration"));

  cppDEL(op);
  cppDEL(op_cpy);
}
//-----------------------------------------------------------
void albaOpIterativeRegistrationTest::TestAccept()
//-----------------------------------------------------------
{
  albaOpIterativeRegistration *op = new albaOpIterativeRegistration();

  albaVMEGeneric *vme;
  albaNEW(vme);

  albaVMESurface *surf;
  albaNEW(surf);

  albaVMEVolumeGray *vol;
  albaNEW(vol);

  CPPUNIT_ASSERT(op->Accept(NULL) == false); // Always returns true
  CPPUNIT_ASSERT(op->Accept(vme) == false);

  CPPUNIT_ASSERT(op->Accept(surf) == true);
  CPPUNIT_ASSERT(op->Accept(vol) == true);

  albaDEL(vme);
  albaDEL(surf);
  albaDEL(vol);
  
  cppDEL(op);
}
// //-----------------------------------------------------------
// void albaOpIterativeRegistrationTest::TestOpRun()
// //-----------------------------------------------------------
// {
//   Cannot test: All GUI creation
// }
//-----------------------------------------------------------
void albaOpIterativeRegistrationTest::TestVmeAccept()
//-----------------------------------------------------------
{
  albaVMEGeneric *vme;
  albaNEW(vme);

  CPPUNIT_ASSERT(albaOpIterativeRegistration::VmeAccept(NULL) == false);
  CPPUNIT_ASSERT(albaOpIterativeRegistration::VmeAccept(vme) == true);

  albaDEL(vme);
}
//-----------------------------------------------------------
void albaOpIterativeRegistrationTest::TestOpDoUndo()
//-----------------------------------------------------------
{
  albaOpIterativeRegistration *op = new albaOpIterativeRegistration();

  albaVMESurface *sourceVme;
  albaNEW(sourceVme);

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