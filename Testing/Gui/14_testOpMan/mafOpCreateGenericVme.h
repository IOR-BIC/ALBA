/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateGenericVme.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:00:04 $
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
                mafOpCreateGenericVme(wxString label);
               ~mafOpCreateGenericVme();
	virtual void OnEvent(mafEvent& e);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return vme != NULL;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  mafNodeGeneric  *m_vme; 
};
#endif // __mafOpCreateGenericVme_H__
