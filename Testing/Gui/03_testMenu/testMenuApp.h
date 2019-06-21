/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testMenuApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


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
