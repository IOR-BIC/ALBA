/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testSashApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testSashApp_H__
#define __testSashApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testSashLogic.h" 
// ============================================================================
class testSashApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testSashLogic *m_logic;

};

DECLARE_APP(testSashApp)

#endif //__testSashApp_H__
