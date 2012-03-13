/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testFrameLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:40 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
