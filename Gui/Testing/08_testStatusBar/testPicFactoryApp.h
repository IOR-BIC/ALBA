/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testPicFactoryApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:42:26 $
  Version:   $Revision: 1.1 $
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
#include "mafDefines.h" //important: mafDefines should always be included as first
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





