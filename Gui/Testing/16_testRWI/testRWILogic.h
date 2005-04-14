/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWILogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 16:20:19 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testRWILogic_H__
#define __testRWILogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h" 
#include <wx/frame.h>
//----------------------------------------------------------------------------
// Forward Decl:
//----------------------------------------------------------------------------
class vtkRenderWindow;
class vtkRenderer;
class mafRWIBase;
class vtkConeSource;
class vtkPolyDataMapper;
class vtkActor;
class myFrame;



class myFrame : public wxFrame
{
public:
  myFrame(wxWindow *parent,long id, wxString title) :wxFrame(parent,id,title) {};
  void OnCloseWindow(wxCloseEvent& event) {mafEventMacro(mafEvent(this,MENU_FILE_QUIT));};
  mafEventListener *m_Listener;
  DECLARE_EVENT_TABLE()
};


// ============================================================================
class testRWILogic : public mafEventListener
// ============================================================================
{
public:
   testRWILogic();
  ~testRWILogic();
   void OnEvent(mafEvent& e);
   void Show();

   myFrame *m_win;

   vtkRenderer *R;
   vtkRenderWindow *RW; 
   mafRWIBase *RWI;
   vtkConeSource *CS;
   vtkPolyDataMapper *PDM;
   vtkActor *A;

};

#endif //__testRWILogic_H__









