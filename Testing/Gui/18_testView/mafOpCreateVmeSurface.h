/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateVmeSurface.h,v $
  Language:  C++
  Date:      $Date: 2008-03-06 12:00:35 $
  Version:   $Revision: 1.1 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafOpCreateVmeSurface_H__
#define __mafOpCreateVmeSurface_H__
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
class mafOpCreateVmeSurface : public mafOp
// ============================================================================
{
public:
                mafOpCreateVmeSurface(wxString label);
               ~mafOpCreateVmeSurface();
	virtual void OnEvent(mafEvent& e);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return vme && vme->IsMAFType(mafVME);};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  mafVMESurface  *m_vme; 
};
#endif // __mafOpCreateVmeSurface_H__
