/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testVMEChooserApp.h,v $
  Language:  C++
  Date:      $Date: 2005-05-18 15:27:21 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testVMEChooserApp_H__
#define __testVMEChooserApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafLogicWithManagers.h" 

// ============================================================================
class testVMEChooserApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    mafLogicWithManagers *m_logic;
};
DECLARE_APP(testVMEChooserApp)
#endif //__testVMEChooserApp_H__