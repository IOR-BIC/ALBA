/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testStatusBarApp.h,v $
  Language:  C++
  Date:      $Date: 2005-06-06 15:11:51 $
  Version:   $Revision: 1.1.1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testStatusBarApp_H__
#define __testStatusBarApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testStatusBarLogic.h" 

// ============================================================================
class testStatusBarApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testStatusBarLogic *m_logic;

};

DECLARE_APP(testStatusBarApp)

#endif //__testStatusBarApp_H__




