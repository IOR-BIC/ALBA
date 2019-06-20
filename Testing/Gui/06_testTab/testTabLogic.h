/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testTabLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testTabLogic_H__
#define __testTabLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/notebook.h>
#include "albaEvent.h"
#include "albaObserver.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaGUIMDIFrame;
class albaGUISashPanel;
class testTabGui;
// ============================================================================
class testTabLogic : public albaObserver
// ============================================================================
{
public:
  testTabLogic();
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
 testTabGui    *m_gui;
};

#endif // __testTabLogic_H__



