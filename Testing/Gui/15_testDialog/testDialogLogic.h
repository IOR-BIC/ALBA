/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testDialogLogic
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testDialogLogic_H__
#define __testDialogLogic_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaLogicWithGUI.h" 
#include "albaObserver.h"

// ============================================================================
class testDialogLogic : public albaObserver
// ============================================================================
{
public:
   testDialogLogic();
  ~testDialogLogic();
   void OnEvent(albaEventBase *alba_event);
   void Show();
   wxFrame *m_win;
protected:
   int m_test_variable;
};
#endif //__testDialogLogic_H__
