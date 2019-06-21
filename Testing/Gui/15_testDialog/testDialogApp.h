/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testDialogApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testDialogApp_H__
#define __testDialogApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testDialogLogic.h"

// ============================================================================
class testDialogApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testDialogLogic *m_logic;
};

DECLARE_APP(testDialogApp)

#endif //__testDialogApp_H__








