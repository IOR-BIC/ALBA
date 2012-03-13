/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testGuiApp.h,v $
  Language:  C++
  Date:      $Date: 2005-06-06 15:11:50 $
  Version:   $Revision: 1.1.1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testGuiApp_H__
#define __testGuiApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testGuiLogic.h" 

// ============================================================================
class testGuiApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testGuiLogic *m_logic;

};

DECLARE_APP(testGuiApp)

#endif //__testGuiApp_H__

