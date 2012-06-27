/*=========================================================================

 Program: MAF2
 Module: testFrameApp
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testFrameApp_H__
#define __testFrameApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testFrameLogic.h" 

// ============================================================================
class testFrameApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testFrameLogic *m_logic;

};

DECLARE_APP(testFrameApp)

#endif //__testFrameApp_H__
