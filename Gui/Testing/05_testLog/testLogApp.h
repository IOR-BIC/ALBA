/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testLogApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:47:37 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testLogApp_H__
#define __testLogApp_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h" //important: mafDefines should always be included as first
#include "testLogLogic.h" 

// ============================================================================
class testLogApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testLogLogic *m_logic;

};

DECLARE_APP(testLogApp)

#endif //__testLogApp_H__
