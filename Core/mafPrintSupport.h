/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPrintSupport.h,v $
Language:  C++
Date:      $Date: 2005-11-28 13:00:29 $
Version:   $Revision: 1.1 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPrintSupport_H__
#define __mafPrintSupport_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/print.h>

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafView;

//----------------------------------------------------------------------------
// mafPrintout :
//----------------------------------------------------------------------------
class mafPrintSupport
{
 public:
  mafPrintSupport();
  virtual ~mafPrintSupport();

  void OnPrintPreview(mafView *v);
  void OnPrint(mafView *v);
  void OnPrintSetup();
  void OnPageSetup();
 
protected:
  wxPrintData *m_PrintData;
  wxPageSetupData *m_PageSetupData;
};
#endif
