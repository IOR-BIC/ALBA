/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testFrameApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


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
