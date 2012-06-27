/*=========================================================================

 Program: MAF2
 Module: testGuiLogic
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testGuiLogic_H__
#define __testGuiLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
//class mafGUIMDIFrame;
class mafGUIFrame;
class testGui
  ;

// ============================================================================
class testGuiLogic : public mafObserver
// ============================================================================
{
public:
                testGuiLogic();
  virtual      ~testGuiLogic(); 
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mafGUIFrame      *m_win;
 testGui       *m_testGui;
};

#endif // __testGuiLogic_H__


