/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testStatusBarLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:57 $
  Version:   $Revision: 1.4 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testStatusBarLogic_H__
#define __testStatusBarLogic_H__
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
class testStatusBarGui;
// ============================================================================
class testStatusBarLogic : public mafEventListener
// ============================================================================
{
public:
                testStatusBarLogic();
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

 int            m_progress;
 wxString       m_progress_text;
};

#endif // __testStatusBarLogic_H__





