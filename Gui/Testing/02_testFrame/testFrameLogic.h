/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testFrameLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:59 $
  Version:   $Revision: 1.2 $
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
#include "mmgMDIFrame.h"
// ============================================================================
class testFrameLogic : public mafEventListener
// ============================================================================
{
public:
                testFrameLogic();
  virtual      ~testFrameLogic(); 
  virtual void  OnEvent(mafEvent& e);
          void  Show(); 
          void  Init(int argc, char** argv); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mmgMDIFrame   *m_win;

};

#endif // __testFrameLogic_H__

