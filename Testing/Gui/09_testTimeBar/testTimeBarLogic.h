/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testTimeBarLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testTimeBarLogic_H__
#define __testTimeBarLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaGUIMDIFrame;
class albaGUITimeBar;
class albaGUISashPanel;
// ============================================================================
class testTimeBarLogic : public albaObserver
// ============================================================================
{
public:
                testTimeBarLogic();
  virtual void  OnEvent(albaEventBase *alba_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
 void CreateMenu();
 void CreateLogBar();
 void CreateSideBar();
 void CreateTimeBar();

 albaGUIMDIFrame    *m_win;
 wxMenuBar      *m_menu_bar;
 albaGUISashPanel   *m_log_bar; 
 albaGUISashPanel   *m_side_bar;
 albaGUISashPanel   *m_time_bar;
 albaGUITimeBar     *m_time_panel;

 double          m_time_bounds[2];
};

#endif // __testTimeBarLogic_H__





