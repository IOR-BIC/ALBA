/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: testVMEChooserApp
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __testVMEChooserApp_H__
#define __testVMEChooserApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaLogicWithManagers.h" 

// ============================================================================
class testVMEChooserApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    albaLogicWithManagers *m_logic;
};
DECLARE_APP(testVMEChooserApp)
#endif //__testVMEChooserApp_H__