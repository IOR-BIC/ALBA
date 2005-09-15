/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exInteractionApp.h,v $
  Language:  C++
  Date:      $Date: 2005-09-15 09:45:49 $
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
class exInteractionApp : public wxApp
// ============================================================================
{
public:
  bool OnInit();
  int  OnExit();
protected:
  mafLogicWithManagers *m_logic;
};
DECLARE_APP(exInteractionApp)
#endif //__testViewApp_H__