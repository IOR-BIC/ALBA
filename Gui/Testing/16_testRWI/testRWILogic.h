/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWILogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-16 10:00:19 $
  Version:   $Revision: 1.2 $
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
class mafRWI;
class vtkConeSource;
class vtkPolyDataMapper;
class vtkActor;

// ============================================================================
class testRWILogic : public mafEventListener
// ============================================================================
{
public:
   testRWILogic();
  ~testRWILogic();
   void OnEvent(mafEvent& e);
   void Show();

   wxFrame *m_win;
};

#endif //__testRWILogic_H__









