/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testGuiLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:49 $
  Version:   $Revision: 1.2 $
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
//----------------------------------------------------------------------------
// Forward References:
//----------------------------------------------------------------------------
//class mmgMDIFrame;
class mmgFrame;
class testGui
  ;

// ============================================================================
class testGuiLogic : public mafEventListener
// ============================================================================
{
public:
                testGuiLogic();
  virtual      ~testGuiLogic(); 
  virtual void  OnEvent(mafEvent& e);
          void  Show(); 
          void  OnQuit();
      wxWindow* GetTopWin();
protected:
 mmgFrame      *m_win;
 testGui       *m_testGui;
};

#endif // __testGuiLogic_H__


