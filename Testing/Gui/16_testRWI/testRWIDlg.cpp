/*=========================================================================

 Program: MAF2
 Module: testRWIDlg
 Authors: Silvano Imboden
 
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

#include "testRWIDlg.h"
#include "wx/busyinfo.h"
#include "mafDecl.h"
#include "mafGUIValidator.h"

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
: mafGUIDialog(title)
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
  
  RWI = new mafRWI(this);
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
