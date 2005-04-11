/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSashLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:18 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testSashLogic_H__
#define __testSashLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mmgMDIFrame;
class mmgSashPanel;

// ============================================================================
class testSashLogic : public mafEventListener
// ============================================================================
{
public:
                testSashLogic();
  virtual void  OnEvent(mafEvent& e);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mmgMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;
 mmgSashPanel  *m_log_bar;
 mmgSashPanel  *m_side_bar;
 mmgSashPanel  *m_time_bar;
};

#endif // __testSashLogic_H__

