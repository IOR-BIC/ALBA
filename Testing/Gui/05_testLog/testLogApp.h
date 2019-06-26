/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testLogApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testLogApp_H__
#define __testLogApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testLogLogic.h" 
// ============================================================================
class testLogApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testLogLogic *m_logic;

};

DECLARE_APP(testLogApp)

#endif //__testLogApp_H__
