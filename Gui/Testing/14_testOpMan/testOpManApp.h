/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testOpManApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-12 14:01:00 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testOpManApp_H__
#define __testOpManApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafLogicWithManagers.h" 

// ============================================================================
class testOpManApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    mafLogicWithManagers *m_logic;

};

DECLARE_APP(testOpManApp)

#endif //__testOpManApp_H__






