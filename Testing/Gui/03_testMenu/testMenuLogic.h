/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testMenuLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:40 $
  Version:   $Revision: 1.3 $
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
#include "mafGUIMDIFrame.h"
#include "mafObserver.h"

// ============================================================================
class testMenuLogic : public mafObserver
// ============================================================================
{
public:
                testMenuLogic();
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mafGUIMDIFrame   *m_win;
 wxMenuBar     *m_menu_bar;

};

#endif // __testMenuLogic_H__

