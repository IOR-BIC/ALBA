/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTimeBarLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testTimeBarLogic_H__
#define __testTimeBarLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUITimeBar;
class mafGUISashPanel;
// ============================================================================
class testTimeBarLogic : public mafObserver
// ============================================================================
{
public:
                testTimeBarLogic();
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
 void CreateMenu();
 void CreateLogBar();
 void CreateSideBar();
 void CreateTimeBar();

 mafGUIMDIFrame    *m_win;
 wxMenuBar      *m_menu_bar;
 mafGUISashPanel   *m_log_bar; 
 mafGUISashPanel   *m_side_bar;
 mafGUISashPanel   *m_time_bar;
 mafGUITimeBar     *m_time_panel;

 double          m_time_bounds[2];
};

#endif // __testTimeBarLogic_H__





