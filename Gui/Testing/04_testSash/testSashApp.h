/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testSashApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:24:18 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testSashApp_H__
#define __testSashApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testSashLogic.h" 
// ============================================================================
class testSashApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testSashLogic *m_logic;

};

DECLARE_APP(testSashApp)

#endif //__testSashApp_H__
