/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testGuiLogic.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:04:40 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testGuiLogic_H__
#define __testGuiLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
//class mafGUIMDIFrame;
class mafGUIFrame;
class testGui
  ;

// ============================================================================
class testGuiLogic : public mafObserver
// ============================================================================
{
public:
                testGuiLogic();
  virtual      ~testGuiLogic(); 
  virtual void  OnEvent(mafEventBase *maf_event);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mafGUIFrame      *m_win;
 testGui       *m_testGui;
};

#endif // __testGuiLogic_H__


