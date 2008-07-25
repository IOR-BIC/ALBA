/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exOperationApp.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:00:01 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __exOperationApp_H__
#define __exOperationApp_H__
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
#endif //__exOperationApp_H__
