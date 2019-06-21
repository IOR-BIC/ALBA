/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testCheckTreeLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testCheckTreeLogic_H__
#define __testCheckTreeLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaEvent.h"
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include "albaObserver.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaGUIMDIFrame;
class albaGUISashPanel;
class albaGUICheckTree;
class testCheckTreeGui;

//-------------------------------------------------------------------------
// TestNode
//-------------------------------------------------------------------------
#include "albaNode.h"
class albaNodeA: public albaNode
{
public:
  albaTypeMacro(albaNodeA,albaNode);
};
class albaNodeB: public albaNode
{
public:
  albaTypeMacro(albaNodeB,albaNode);
};


// ============================================================================
class testCheckTreeLogic : public albaObserver
// ============================================================================
{
public:
                testCheckTreeLogic();
               ~testCheckTreeLogic();
  virtual void  OnEvent(albaEventBase *alba_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
  void CreateMenu();
  void CreateLogBar();
  void CreateSideBar();
  void CreateToolBar();

  albaGUIMDIFrame   *m_win;
  wxMenuBar     *m_menu_bar;
  albaGUISashPanel  *m_log_bar; 
  albaGUISashPanel  *m_side_bar;
  albaGUICheckTree  *m_tree;
  wxNotebook    *m_notebook;

  albaNodeA    *m_nodea;
  albaNodeB    *m_nodeb;

  wxToolBar *m_toolbar;
};

#endif // __testCheckTreeLogic_H__
