/*=========================================================================

 Program: MAF2
 Module: mafPrintSupport
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class MAF_EXPORT mafPrintSupport
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
