/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testLogLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:27 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testLogLogic_H__
#define __testLogLogic_H__
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
class testLogLogic : public mafEventListener
// ============================================================================
{
public:
                testLogLogic();
  virtual void  OnEvent(mafEvent& e);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mmgMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;
 mmgSashPanel  *m_log_bar; 
};

#endif // __testLogLogic_H__

