/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSideBarLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:48:39 $
  Version:   $Revision: 1.1 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first
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




