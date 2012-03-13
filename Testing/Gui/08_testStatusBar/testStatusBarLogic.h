/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testStatusBarLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.3 $
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
#include "mafObserver.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUISashPanel;
class testStatusBarGui;
// ============================================================================
class testStatusBarLogic : public mafObserver
// ============================================================================
{
public:
                testStatusBarLogic();
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
 void CreateMenu();
 void CreateLogBar();
 void CreateSideBar();

 mafGUIMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;
 mafGUISashPanel  *m_log_bar; 
 mafGUISashPanel  *m_side_bar;

 int            m_progress;
 wxString       m_progress_text;
};

#endif // __testStatusBarLogic_H__





