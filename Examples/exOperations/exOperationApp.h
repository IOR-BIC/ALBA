/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exOperationApp.h,v $
  Language:  C++
  Date:      $Date: 2005-11-08 09:46:01 $
  Version:   $Revision: 1.3 $
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
class exOperationApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();
protected:
  mafLogicWithManagers *m_Logic;

  void OnFatalException();
};
DECLARE_APP(exOperationApp)
#endif //__testViewApp_H__