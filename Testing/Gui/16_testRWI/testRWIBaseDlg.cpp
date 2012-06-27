/*=========================================================================

 Program: MAF2
 Module: testRWIBaseDlg
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

#include "testRWIBaseDlg.h"
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
testRWIBaseDlg::testRWIBaseDlg(const wxString& title)
: mafGUIDialog(title)
//----------------------------------------------------------------------------
{
  CS  = NULL;
  PDM  = NULL;
  A  = NULL;
  R  = NULL;
  RW  = NULL;
  RWI = NULL;

  CS = vtkConeSource::New();
  CS->Update();

  PDM = vtkPolyDataMapper::New();
  PDM->SetInput((vtkPolyData *) CS->GetOutput());

  A = vtkActor::New();
  A->SetMapper(PDM);

  R = vtkRenderer::New();
  R->AddActor( A );
  R->SetBackground(1,0,0);

  RW = vtkRenderWindow::New();
  RW->AddRenderer(R);
  R->ResetCamera();

  RWI = new mafRWIBase(this,-1);
  ((wxWindow*)RWI)->SetSize(300,200);
  RWI->SetRenderWindow(RW);   //SIL. 14-4-2005: Uncomment This to have a leak
  RWI->Show(true);
  this->Add(RWI,1);
  
  /*
  RWI2 = new mafRWI(m_win);
  RWI2->m_rwi->Show(true);
  RWI2->m_r1->AddActor(A);
  RWI2->CameraReset();
  */
}
//----------------------------------------------------------------------------
testRWIBaseDlg::~testRWIBaseDlg( ) 
//----------------------------------------------------------------------------
{
  vtkDEL(CS);
  vtkDEL(PDM);
  vtkDEL(A);
  vtkDEL(R);
  vtkDEL(RW);
  vtkDEL(RWI); 
}
