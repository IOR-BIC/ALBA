/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exVMEManApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-18 21:09:12 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __exVMEManApp_H__
#define __exVMEManApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafLogicWithManagers.h" 

// ============================================================================
class exVMEManApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    mafLogicWithManagers *m_logic;

};

DECLARE_APP(exVMEManApp)

#endif //__exVMEManApp_H__






