/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testMenuLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testMenuLogic_H__
#define __testMenuLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGUIMDIFrame.h"
#include "albaObserver.h"

// ============================================================================
class testMenuLogic : public albaObserver
// ============================================================================
{
public:
                testMenuLogic();
  virtual void  OnEvent(albaEventBase *alba_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 albaGUIMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;

};

#endif // __testMenuLogic_H__

