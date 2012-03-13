/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testPicFactoryLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:41 $
  Version:   $Revision: 1.3 $
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
#include "mafObserver.h"

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafGUIMDIFrame;
class mafGUISashPanel;
// ============================================================================
class testPicFactoryLogic : public mafObserver
// ============================================================================
{
public:
                testPicFactoryLogic();
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
};

#endif // __testPicFactoryLogic_H__






