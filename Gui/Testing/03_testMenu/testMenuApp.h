/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testMenuApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-01 09:46:54 $
  Version:   $Revision: 1.1 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first
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
