/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateVmeSurface.h,v $
  Language:  C++
  Date:      $Date: 2006-05-24 11:17:54 $
  Version:   $Revision: 1.3 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmoCreateVmeSurface_H__
#define __mmoCreateVmeSurface_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafVME.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class mafVMESurface;
// ============================================================================
class mmoCreateVmeSurface : public mafOp
// ============================================================================
{
public:
                mmoCreateVmeSurface(wxString label);
               ~mmoCreateVmeSurface();
	virtual void OnEvent(mafEventBase *event);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return vme && vme->IsMAFType(mafVME);};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  mafVMESurface  *m_vme; 
};
#endif // __mmoCreateVmeSurface_H__
