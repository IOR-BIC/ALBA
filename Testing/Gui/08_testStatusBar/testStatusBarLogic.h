/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testStatusBarLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testStatusBarLogic_H__
#define __testStatusBarLogic_H__
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
class testStatusBarGui;
// ============================================================================
class testStatusBarLogic : public albaObserver
// ============================================================================
{
public:
                testStatusBarLogic();
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

 int            m_progress;
 wxString       m_progress_text;
};

#endif // __testStatusBarLogic_H__





