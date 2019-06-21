/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testOpManApp
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testOpManApp_H__
#define __testOpManApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaLogicWithManagers.h" 

// ============================================================================
class testOpManApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    albaLogicWithManagers *m_logic;

};

DECLARE_APP(testOpManApp)

#endif //__testOpManApp_H__






