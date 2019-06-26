/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testRWILogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testRWILogic_H__
#define __testRWILogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h" 
#include <wx/frame.h>
#include "albaObserver.h"
//----------------------------------------------------------------------------
// Forward Decl:
//----------------------------------------------------------------------------
class vtkRenderWindow;
class vtkRenderer;
class albaRWIBase;
class albaRWI;
class vtkConeSource;
class vtkPolyDataMapper;
class vtkActor;

// ============================================================================
class testRWILogic : public albaObserver
// ============================================================================
{
public:
   testRWILogic();
  ~testRWILogic();
   void OnEvent(albaEventBase *alba_event);
   void Show();

   wxFrame *m_win;
};

#endif //__testRWILogic_H__









