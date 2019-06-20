/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testRWIApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testRWIApp_H__
#define __testRWIApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testRWILogic.h"

// ============================================================================
class testRWIApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testRWILogic *m_logic;
};

DECLARE_APP(testRWIApp)

#endif //__testRWIApp_H__









