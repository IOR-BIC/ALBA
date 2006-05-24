/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateGenericVme.h,v $
  Language:  C++
  Date:      $Date: 2006-05-24 11:18:28 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmoCreateGenericVme_H__
#define __mmoCreateGenericVme_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafNodeGeneric;
// ============================================================================
class mmoCreateGenericVme : public mafOp
// ============================================================================
{
public:
                mmoCreateGenericVme(wxString label);
               ~mmoCreateGenericVme();
	virtual void OnEvent(mafEventBase *event);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return vme != NULL;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  mafNodeGeneric  *m_vme; 
};
#endif // __mmoCreateGenericVme_H__








