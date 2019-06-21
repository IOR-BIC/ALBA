/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testViewApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testViewApp_H__
#define __testViewApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaLogicWithManagers.h" 

// ============================================================================
class testViewApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    albaLogicWithManagers *m_logic;

};

DECLARE_APP(testViewApp)

#endif //__testViewApp_H__







