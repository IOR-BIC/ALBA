/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testVTKImporterApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-29 10:47:04 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testVTKImporterApp_H__
#define __testVTKImporterApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafLogicWithManagers.h" 

// ============================================================================
class testVTKImporterApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();

protected:
  mafLogicWithManagers *m_logic;
};
DECLARE_APP(testVTKImporterApp)
#endif //__testVTKImporterApp_H__