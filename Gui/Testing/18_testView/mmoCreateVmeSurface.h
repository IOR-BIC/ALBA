/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoCreateVmeSurface.h,v $
  Language:  C++
  Date:      $Date: 2005-04-21 13:20:03 $
  Version:   $Revision: 1.1 $
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
	virtual void OnEvent(mafEvent& e);
  mafOp* Copy();

  bool Accept(mafNode* vme) {return vme && vme->IsMAFType(mafVME);};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  void OpStop(int result);
  mafVMESurface  *m_vme; 
};
#endif // __mmoCreateVmeSurface_H__










