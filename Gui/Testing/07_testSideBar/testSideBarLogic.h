/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSideBarLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:48 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testSideBarLogic_H__
#define __testSideBarLogic_H__
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
class testSideBarGui;
// ============================================================================
class testSideBarLogic : public mafEventListener
// ============================================================================
{
public:
                testSideBarLogic();
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
 testSideBarGui    *m_gui;
};

#endif // __testSideBarLogic_H__




