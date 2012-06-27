/*=========================================================================

 Program: MAF2
 Module: testStatusBarApp
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testStatusBarApp_H__
#define __testStatusBarApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testStatusBarLogic.h" 

// ============================================================================
class testStatusBarApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testStatusBarLogic *m_logic;

};

DECLARE_APP(testStatusBarApp)

#endif //__testStatusBarApp_H__




