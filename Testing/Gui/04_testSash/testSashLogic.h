/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSashLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:40 $
  Version:   $Revision: 1.3 $
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
#include "mafObserver.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUISashPanel;

// ============================================================================
class testSashLogic : public mafObserver
// ============================================================================
{
public:
                testSashLogic();
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mafGUIMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;
 mafGUISashPanel  *m_log_bar;
 mafGUISashPanel  *m_side_bar;
 mafGUISashPanel  *m_time_bar;
};

#endif // __testSashLogic_H__

