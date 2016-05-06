/*=========================================================================

 Program: MAF2
 Module: mafOpCreateVmeSurface
 Authors: Silvano Imboden
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class __declspec( dllexport ) mafOpCreateVmeSurface : public mafOp
// ============================================================================
{
public:
                mafOpCreateVmeSurface(wxString label = "CreateVmeSurface");
               ~mafOpCreateVmeSurface();
	virtual void OnEvent(mafEventBase *event);
  mafOp* Copy();
  mafTypeMacro(mafOpCreateVmeSurface, mafOp);

  bool Accept(mafVME* vme) {return vme && vme->IsMAFType(mafVME);};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  mafVMESurface  *m_vme; 
};
#endif // __mafOpCreateVmeSurface_H__
