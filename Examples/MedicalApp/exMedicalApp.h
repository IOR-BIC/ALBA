/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exMedicalApp.h,v $
  Language:  C++
  Date:      $Date: 2006-12-13 09:19:29 $
  Version:   $Revision: 1.2 $
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
class exMedicalApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();
protected:
  mafLogicWithManagers *m_logic;
};
DECLARE_APP(exMedicalApp)
#endif //__testViewApp_H__
