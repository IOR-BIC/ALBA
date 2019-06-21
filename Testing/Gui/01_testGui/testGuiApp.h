/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testGuiApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testGuiApp_H__
#define __testGuiApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testGuiLogic.h" 

// ============================================================================
class testGuiApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testGuiLogic *m_logic;

};

DECLARE_APP(testGuiApp)

#endif //__testGuiApp_H__

