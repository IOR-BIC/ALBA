/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testPicFactoryLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:25:17 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testPicFactoryLogic_H__
#define __testPicFactoryLogic_H__
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
// ============================================================================
class testPicFactoryLogic : public mafEventListener
// ============================================================================
{
public:
                testPicFactoryLogic();
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
};

#endif // __testPicFactoryLogic_H__






