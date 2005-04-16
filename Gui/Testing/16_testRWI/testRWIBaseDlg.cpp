/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWIBaseDlg.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-16 10:00:18 $
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

#include "testRWIBaseDlg.h"
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
testRWIBaseDlg::testRWIBaseDlg(const wxString& title)
: mmgDialog(title)
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
}
//----------------------------------------------------------------------------
void testRWIBaseDlg::OnCloseWindow(wxCloseEvent& event)
//----------------------------------------------------------------------------
{
  vtkDEL(CS);
  vtkDEL(PDM);
  vtkDEL(A);
  vtkDEL(R);
  vtkDEL(RW);
  vtkDEL(RWI); 
  //RWI must be Deleted :
  // -- using the vtk Delete() function
  // -- before the parent window is destroyed -- so, not in the destructor of this Dialog
  wxDialog::OnCloseWindow(event);
}

