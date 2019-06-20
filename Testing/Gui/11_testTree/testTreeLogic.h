/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testTreeLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testTreeLogic_H__
#define __testTreeLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include <wx/notebook.h>
#include "albaObserver.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaGUIMDIFrame;
class albaGUISashPanel;
class testTreeGui;
// ============================================================================
class testTreeLogic : public albaObserver
// ============================================================================
{
public:
                testTreeLogic();
  virtual void  OnEvent(albaEventBase *alba_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
 void CreateMenu();
 void CreateLogBar();
 void CreateSideBar();

 albaGUIMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;
 albaGUISashPanel  *m_log_bar; 
 albaGUISashPanel  *m_side_bar;
 wxNotebook    *m_notebook;
};

#endif // __testTreeLogic_H__





