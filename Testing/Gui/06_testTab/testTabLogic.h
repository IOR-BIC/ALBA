/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTabLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.3 $
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
#include <wx/notebook.h>
#include "mafEvent.h"
#include "mafObserver.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUISashPanel;
class testTabGui;
// ============================================================================
class testTabLogic : public mafObserver
// ============================================================================
{
public:
  testTabLogic();
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
 wxNotebook    *m_notebook;
 testTabGui    *m_gui;
};

#endif // __testTabLogic_H__



