/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: exInteractionApp.h,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
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
