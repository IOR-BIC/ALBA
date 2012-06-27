/*=========================================================================

 Program: MAF2
 Module: testGuiApp
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

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

