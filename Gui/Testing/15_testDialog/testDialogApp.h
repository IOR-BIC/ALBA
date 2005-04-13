/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: testDialogApp.h,v $
  Language:  C++
  Date:      $Date: 2005-04-13 21:04:15 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __testDialogApp_H__
#define __testDialogApp_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "testDialogLogic.h"

// ============================================================================
class testDialogApp : public wxApp
// ============================================================================
{
public:
    bool OnInit();
    int  OnExit();
protected:
    testDialogLogic *m_logic;
};

DECLARE_APP(testDialogApp)

#endif //__testDialogApp_H__








