/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testFrameLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:46:29 $
  Version:   $Revision: 1.1 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first
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

