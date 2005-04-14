/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testRWIApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-14 16:20:18 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testRWIApp_H__
#define __testRWIApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testRWILogic.h"

// ============================================================================
class testRWIApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testRWILogic *m_logic;
};

DECLARE_APP(testRWIApp)

#endif //__testRWIApp_H__









