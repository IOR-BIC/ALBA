/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testSashLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testSashLogic_H__
#define __testSashLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaGUIMDIFrame;
class albaGUISashPanel;

// ============================================================================
class testSashLogic : public albaObserver
// ============================================================================
{
public:
                testSashLogic();
  virtual void  OnEvent(albaEventBase *alba_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 albaGUIMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;
 albaGUISashPanel  *m_log_bar;
 albaGUISashPanel  *m_side_bar;
 albaGUISashPanel  *m_time_bar;
};

#endif // __testSashLogic_H__

