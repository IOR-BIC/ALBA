/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWILogic.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-14 16:20:19 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "testRWILogic.h"
#include "mafDecl.h"
#include "mmgDialog.h"
#include "mmgGui.h"
#include "mmgValidator.h"

#include "mafRWIBase.h"
#include "vtkPolyData.h"
#include "vtkConeSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
//--------------------------------------------------------------------------------
//const:
//--------------------------------------------------------------------------------
enum 
{
  ID_D1 = MINID,
};

BEGIN_EVENT_TABLE(myFrame, wxFrame)
EVT_CLOSE(myFrame::OnCloseWindow)
END_EVENT_TABLE()

//--------------------------------------------------------------------------------
testRWILogic::testRWILogic()
//--------------------------------------------------------------------------------
{
  m_win = new myFrame(NULL,-1,"TestRWI");
  m_win->m_Listener = this;
  m_win->SetSize(500,400);
  mafSetFrame(m_win);

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

  RWI = new mafRWIBase(m_win,-1);
  RWI->SetRenderWindow(RW);   //SIL. 14-4-2005: Uncomment This to have a leak
  RWI->Show(true);
}
//--------------------------------------------------------------------------------
testRWILogic::~testRWILogic()
//--------------------------------------------------------------------------------
{
}
//--------------------------------------------------------------------------------
void testRWILogic::OnEvent(mafEvent& e)
//--------------------------------------------------------------------------------
{
  switch(e.GetId())
  {
    case MENU_FILE_QUIT: 
      int i=0;

      CS->Delete();
      PDM->Delete();
      A->Delete();
      R->Delete();
      RW->Delete();
      RWI->Delete(); 

      /*
      //vtkDEL(CS);
      //vtkDEL(PDM);
      //vtkDEL(A);

      //if(R) R->RemoveActor(A);

      CS->Delete();
      PDM->Delete();
      A->Delete();

      //if(R) 
      //{
      //  R->GetActors();        
      //  if(RW) RW->RemoveRenderer(R);
      //}
      R->Delete();

      //if(RW) RW->SetInteractor(NULL);
      RW->Delete();

//      if(RWI) RWI->SetRenderWindow(NULL); 
      RWI->Delete(); // must use vtkDEL, and not the delete operator
      //cppDEL(RWI); must use vtkDEL, and not the delete operator
      */
      m_win->Destroy();
    break;
  }
}
//--------------------------------------------------------------------------------
void testRWILogic::Show()
//--------------------------------------------------------------------------------
{
  m_win->Show(true);
}
