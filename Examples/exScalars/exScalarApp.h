/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exScalarApp.h,v $
  Language:  C++
  Date:      $Date: 2006-06-14 13:34:46 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testViewApp_H__
#define __testViewApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafLogicWithManagers.h" 

// ============================================================================
class exScalarApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();
protected:
  mafLogicWithManagers *m_Logic;

  void OnFatalException();
};
DECLARE_APP(exScalarApp)
#endif //__testViewApp_H__
