/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTimeBarLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-04 15:01:48 $
  Version:   $Revision: 1.1 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first
#include "mafEvent.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mmgMDIFrame;
class mmgTimeBar;
class mmgSashPanel;
// ============================================================================
class testTimeBarLogic : public mafEventListener
// ============================================================================
{
public:
                testTimeBarLogic();
  virtual void  OnEvent(mafEvent& e);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
 void CreateMenu();
 void CreateLogBar();
 void CreateSideBar();
 void CreateTimeBar();

 mmgMDIFrame    *m_win;
 wxMenuBar      *m_menu_bar;
 mmgSashPanel   *m_log_bar; 
 mmgSashPanel   *m_side_bar;
 mmgSashPanel   *m_time_bar;
 mmgTimeBar     *m_time_panel;

 double          m_time_bounds[2];
};

#endif // __testTimeBarLogic_H__





