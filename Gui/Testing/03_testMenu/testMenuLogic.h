/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testMenuLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:46:54 $
  Version:   $Revision: 1.1 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first
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

