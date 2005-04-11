/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTabLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:37 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testTabLogic_H__
#define __testTabLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include <wx/notebook.h>
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mmgMDIFrame;
class mmgSashPanel;
class testTabGui;
// ============================================================================
class testTabLogic : public mafEventListener
// ============================================================================
{
public:
                testTabLogic();
  virtual void  OnEvent(mafEvent& e);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
 void CreateMenu();
 void CreateLogBar();
 void CreateSideBar();

 mmgMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;
 mmgSashPanel  *m_log_bar; 
 mmgSashPanel  *m_side_bar;
 wxNotebook    *m_notebook;
 testTabGui    *m_gui;
};

#endif // __testTabLogic_H__



