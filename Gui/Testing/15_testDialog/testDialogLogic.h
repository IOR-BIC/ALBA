/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testDialogLogic.h,v $
  Language:  C++
  Date:      $Date: 2005-04-13 21:04:15 $
  Version:   $Revision: 1.1 $
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

// ============================================================================
class testDialogLogic : public mafEventListener
// ============================================================================
{
public:
   testDialogLogic();
  ~testDialogLogic();
   void OnEvent(mafEvent& e);
   void Show();
   wxFrame *m_win;
protected:
   int m_test_variable;
};

#endif //__testDialogLogic_H__








