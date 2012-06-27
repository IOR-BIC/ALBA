/*=========================================================================

 Program: MAF2
 Module: testFrameLogic
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testFrameLogic_H__
#define __testFrameLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafGUIMDIFrame.h"
#include "mafObserver.h"
// ============================================================================
class testFrameLogic : public mafObserver
// ============================================================================
{
public:
                testFrameLogic();
  virtual      ~testFrameLogic(); 
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  Init(int argc, char** argv); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mafGUIMDIFrame   *m_win;
};
#endif // __testFrameLogic_H__
