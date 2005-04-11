/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTreeLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:25:25 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testTreeLogic_H__
#define __testTreeLogic_H__
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
class testTreeGui;
// ============================================================================
class testTreeLogic : public mafEventListener
// ============================================================================
{
public:
                testTreeLogic();
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
};

#endif // __testTreeLogic_H__





