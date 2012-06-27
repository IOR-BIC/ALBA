/*=========================================================================

 Program: MAF2
 Module: testTabLogic
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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



