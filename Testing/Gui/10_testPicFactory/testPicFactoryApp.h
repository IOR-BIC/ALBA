/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testPicFactoryApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testPicFactoryApp_H__
#define __testPicFactoryApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testPicFactoryLogic.h" 
// ============================================================================
class testPicFactoryApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testPicFactoryLogic *m_logic;

};

DECLARE_APP(testPicFactoryApp)

#endif //__testPicFactoryApp_H__





