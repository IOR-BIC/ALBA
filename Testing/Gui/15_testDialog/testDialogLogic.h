/*=========================================================================

 Program: MAF2
 Module: testDialogLogic
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
