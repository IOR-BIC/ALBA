/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testFrameApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:58 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testFrameApp_H__
#define __testFrameApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testFrameLogic.h" 

// ============================================================================
class testFrameApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testFrameLogic *m_logic;

};

DECLARE_APP(testFrameApp)

#endif //__testFrameApp_H__
