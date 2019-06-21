/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testRWIDlg
 Authors: Silvano Imboden
 
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

#include "testRWIDlg.h"
#include "wx/busyinfo.h"
#include "albaDecl.h"
#include "albaGUIValidator.h"

//----------------------------------------------------------------------------
// Event ids
//----------------------------------------------------------------------------
enum 
{
  ID_ENABLE = MINID,
  ID_DISABLE,
};
//----------------------------------------------------------------------------
testRWIDlg::testRWIDlg(const wxString& title)
: albaGUIDialog(title)
//----------------------------------------------------------------------------
{
  CS  = NULL;
  PDM = NULL;
  A   = NULL;
  RWI = NULL;

  CS = vtkConeSource::New();
  CS->Update();

  PDM = vtkPolyDataMapper::New();
  PDM->SetInput((vtkPolyData *) CS->GetOutput());

  A = vtkActor::New();
  A->SetMapper(PDM);
  
  RWI = new albaRWI(this);
  RWI->SetSize(0,0,400,300);
  RWI->m_RwiBase->Show(true);
  RWI->m_RenFront->AddActor(A);
  RWI->CameraReset();

  Add(RWI->m_RwiBase,1);
}
//----------------------------------------------------------------------------
testRWIDlg::~testRWIDlg( ) 
//----------------------------------------------------------------------------
{
  vtkDEL(CS);
  vtkDEL(PDM);
  vtkDEL(A);
  cppDEL(RWI); 
}
