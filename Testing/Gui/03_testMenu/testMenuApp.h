/*=========================================================================

 Program: MAF2
 Module: testMenuApp
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testMenuApp_H__
#define __testMenuApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testMenuLogic.h" 

// ============================================================================
class testMenuApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testMenuLogic *m_logic;

};

DECLARE_APP(testMenuApp)

#endif //__testMenuApp_H__
