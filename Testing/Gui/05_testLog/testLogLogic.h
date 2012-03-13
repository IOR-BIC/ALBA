/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testLogLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:40 $
  Version:   $Revision: 1.3 $
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
#include "mafObserver.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUISashPanel;

// ============================================================================
class testLogLogic : public mafObserver
// ============================================================================
{
public:
                testLogLogic();
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mafGUIMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;
 mafGUISashPanel  *m_log_bar; 
};

#endif // __testLogLogic_H__

