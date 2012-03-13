/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testCheckTreeLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testCheckTreeLogic_H__
#define __testCheckTreeLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include <wx/notebook.h>
#include <wx/toolbar.h>
#include "mafObserver.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUISashPanel;
class mafGUICheckTree;
class testCheckTreeGui;

//-------------------------------------------------------------------------
// TestNode
//-------------------------------------------------------------------------
#include "mafNode.h"
class mafNodeA: public mafNode
{
public:
  mafTypeMacro(mafNodeA,mafNode);
};
class mafNodeB: public mafNode
{
public:
  mafTypeMacro(mafNodeB,mafNode);
};


// ============================================================================
class testCheckTreeLogic : public mafObserver
// ============================================================================
{
public:
                testCheckTreeLogic();
               ~testCheckTreeLogic();
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
  void CreateMenu();
  void CreateLogBar();
  void CreateSideBar();
  void CreateToolBar();

  mafGUIMDIFrame   *m_win;
  wxMenuBar     *m_menu_bar;
  mafGUISashPanel  *m_log_bar; 
  mafGUISashPanel  *m_side_bar;
  mafGUICheckTree  *m_tree;
  wxNotebook    *m_notebook;

  mafNodeA    *m_nodea;
  mafNodeB    *m_nodeb;

  wxToolBar *m_toolbar;
};

#endif // __testCheckTreeLogic_H__
