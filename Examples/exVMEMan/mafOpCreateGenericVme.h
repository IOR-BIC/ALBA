/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateGenericVme.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:03:10 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
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
class mafNodeGeneric;
// ============================================================================
class mafOpCreateGenericVme : public mafOp
// ============================================================================
{
public:
  mafOpCreateGenericVme(wxString label = "CreateGenericVme");
  ~mafOpCreateGenericVme();
  virtual void OnEvent(mafEventBase *event);
  mafOp* Copy();
  mafTypeMacro(mafOpCreateGenericVme, mafOp);

  bool Accept(mafNode* vme) {return vme != NULL;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  mafNodeGeneric  *m_vme; 
};
#endif // __mafOpCreateGenericVme_H__
