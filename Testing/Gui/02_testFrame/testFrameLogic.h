/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testFrameLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testFrameLogic_H__
#define __testFrameLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include "albaGUIMDIFrame.h"
#include "albaObserver.h"
// ============================================================================
class testFrameLogic : public albaObserver
// ============================================================================
{
public:
                testFrameLogic();
  virtual      ~testFrameLogic(); 
  virtual void  OnEvent(albaEventBase *alba_event);
          void  Show(); 
          void  Init(int argc, char** argv); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 albaGUIMDIFrame   *m_win;
};
#endif // __testFrameLogic_H__
