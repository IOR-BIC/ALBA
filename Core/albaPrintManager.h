/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPrintSupport
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPrintSupport_H__
#define __albaPrintSupport_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include <wx/print.h>

//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaView;

//----------------------------------------------------------------------------
// albaPrintout :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaPrintManager
{
 public:
  albaPrintManager();
  virtual ~albaPrintManager();

  void OnPrintPreview(albaView *v);
	void OnPrint(albaVMEImage *img);
  void OnPrint(albaView *v);
  void OnPrintSetup();
  void OnPageSetup();
	
protected:
  wxPrintData *m_PrintData;
  wxPageSetupData *m_PageSetupData;
};
#endif
