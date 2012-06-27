/*=========================================================================

 Program: MAF2
 Module: testRWILogic
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testRWILogic_H__
#define __testRWILogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h" 
#include <wx/frame.h>
#include "mafObserver.h"
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
class testRWILogic : public mafObserver
// ============================================================================
{
public:
   testRWILogic();
  ~testRWILogic();
   void OnEvent(mafEventBase *maf_event);
   void Show();

   wxFrame *m_win;
};

#endif //__testRWILogic_H__









