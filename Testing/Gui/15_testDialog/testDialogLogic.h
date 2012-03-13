/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testDialogLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-11-12 09:32:02 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testDialogLogic_H__
#define __testDialogLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafLogicWithGUI.h" 
#include "mafObserver.h"

// ============================================================================
class testDialogLogic : public mafObserver
// ============================================================================
{
public:
   testDialogLogic();
  ~testDialogLogic();
   void OnEvent(mafEventBase *maf_event);
   void Show();
   wxFrame *m_win;
protected:
   int m_test_variable;
};
#endif //__testDialogLogic_H__
