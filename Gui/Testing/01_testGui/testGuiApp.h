/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testGuiApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:23:49 $
  Version:   $Revision: 1.2 $
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

