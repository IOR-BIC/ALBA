/*=========================================================================

 Program: MAF2
 Module: mafOpCreateGenericVme
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafOpCreateGenericVme_H__
#define __mafOpCreateGenericVme_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVMEGeneric;
// ============================================================================
class __declspec( dllexport ) mafOpCreateGenericVme : public mafOp
// ============================================================================
{
public:
  mafOpCreateGenericVme(wxString label = "CreateGenericVme");
  ~mafOpCreateGenericVme();
  virtual void OnEvent(mafEventBase *event);
  mafOp* Copy();
  mafTypeMacro(mafOpCreateGenericVme, mafOp);

  bool Accept(mafVME* vme) {return vme != NULL;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  mafVMEGeneric  *m_vme; 
};
#endif // __mafOpCreateGenericVme_H__
