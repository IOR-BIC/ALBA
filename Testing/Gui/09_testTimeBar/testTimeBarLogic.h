/*=========================================================================

 Program: MAF2
 Module: testTimeBarLogic
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testTimeBarLogic_H__
#define __testTimeBarLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUITimeBar;
class mafGUISashPanel;
// ============================================================================
class testTimeBarLogic : public mafObserver
// ============================================================================
{
public:
                testTimeBarLogic();
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
 void CreateMenu();
 void CreateLogBar();
 void CreateSideBar();
 void CreateTimeBar();

 mafGUIMDIFrame    *m_win;
 wxMenuBar      *m_menu_bar;
 mafGUISashPanel   *m_log_bar; 
 mafGUISashPanel   *m_side_bar;
 mafGUISashPanel   *m_time_bar;
 mafGUITimeBar     *m_time_panel;

 double          m_time_bounds[2];
};

#endif // __testTimeBarLogic_H__





