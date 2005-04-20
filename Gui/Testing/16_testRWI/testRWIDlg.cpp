/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWIDlg.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-20 14:36:41 $
  Version:   $Revision: 1.2 $
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
  vtkDEL(CS);
  vtkDEL(PDM);
  vtkDEL(A);
  cppDEL(RWI); 
}
