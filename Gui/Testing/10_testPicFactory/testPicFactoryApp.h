/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testPicFactoryApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:25:17 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testPicFactoryApp_H__
#define __testPicFactoryApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testPicFactoryLogic.h" 
// ============================================================================
class testPicFactoryApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testPicFactoryLogic *m_logic;

};

DECLARE_APP(testPicFactoryApp)

#endif //__testPicFactoryApp_H__





