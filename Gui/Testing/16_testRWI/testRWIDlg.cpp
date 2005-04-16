/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWIDlg.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-16 10:00:19 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
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

#include "testRWIDlg.h"
#include "wx/busyinfo.h"
#include "mafDecl.h"
#include "mmgValidator.h"

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
: mmgDialog(title)
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
  RWI->m_rwi->Show(true);
  RWI->m_r1->AddActor(A);
  RWI->CameraReset();

  Add(RWI->m_rwi,1);
}
//----------------------------------------------------------------------------
testRWIDlg::~testRWIDlg( ) 
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void testRWIDlg::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{
  vtkDEL(CS);
  vtkDEL(PDM);
  vtkDEL(A);
  cppDEL(RWI); 
  //RWI (which is here a mafRWI, not a mafRWIBase) must be Deleted :
  // -- using the delete operator
  // -- before the parent window is destroyed -- so, not in the destructor of this Dialog
  wxDialog::OnCloseWindow(event);
}

