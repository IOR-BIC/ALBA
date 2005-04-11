/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testMenuLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:08 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testMenuLogic_H__
#define __testMenuLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mmgMDIFrame.h"
// ============================================================================
class testMenuLogic : public mafEventListener
// ============================================================================
{
public:
                testMenuLogic();
  virtual void  OnEvent(mafEvent& e);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mmgMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;

};

#endif // __testMenuLogic_H__

