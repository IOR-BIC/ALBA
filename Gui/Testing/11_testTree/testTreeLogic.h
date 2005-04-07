/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testTreeLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 18:37:23 $
  Version:   $Revision: 1.1 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first
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





