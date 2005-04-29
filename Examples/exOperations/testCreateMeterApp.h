/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testCreateMeterApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-29 10:47:03 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testCreateMeterApp_H__
#define __testCreateMeterApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafLogicWithManagers.h" 

// ============================================================================
class testCreateMeterApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();

protected:
  mafLogicWithManagers *m_logic;
};
DECLARE_APP(testCreateMeterApp)
#endif //__testCreateMeterApp_H__