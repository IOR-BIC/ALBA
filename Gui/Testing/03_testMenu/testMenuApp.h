/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testMenuApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:08 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testMenuApp_H__
#define __testMenuApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testMenuLogic.h" 

// ============================================================================
class testMenuApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testMenuLogic *m_logic;

};

DECLARE_APP(testMenuApp)

#endif //__testMenuApp_H__
