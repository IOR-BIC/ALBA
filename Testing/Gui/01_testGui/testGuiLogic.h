/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testGuiLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testGuiLogic_H__
#define __testGuiLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaObserver.h"
//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
//class albaGUIMDIFrame;
class albaGUIFrame;
class testGui
  ;

// ============================================================================
class testGuiLogic : public albaObserver
// ============================================================================
{
public:
                testGuiLogic();
  virtual      ~testGuiLogic(); 
  virtual void  OnEvent(albaEventBase *alba_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 albaGUIFrame      *m_win;
 testGui       *m_testGui;
};

#endif // __testGuiLogic_H__


