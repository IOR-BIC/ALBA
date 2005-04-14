/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testCheckTreeLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 15:17:31 $
  Version:   $Revision: 1.2 $
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

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mmgMDIFrame;
class mmgSashPanel;
class mmgCheckTree;
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
class testCheckTreeLogic : public mafEventListener
// ============================================================================
{
public:
                testCheckTreeLogic();
               ~testCheckTreeLogic();
  virtual void  OnEvent(mafEvent& e);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();

protected:
  void CreateMenu();
  void CreateLogBar();
  void CreateSideBar();
  void CreateToolBar();

  mmgMDIFrame   *m_win;
  wxMenuBar     *m_menu_bar;
  mmgSashPanel  *m_log_bar; 
  mmgSashPanel  *m_side_bar;
  mmgCheckTree  *m_tree;
  wxNotebook    *m_notebook;

  mafNodeA    *m_nodea;
  mafNodeB    *m_nodeb;

  wxToolBar *m_toolbar;
};

#endif // __testCheckTreeLogic_H__
