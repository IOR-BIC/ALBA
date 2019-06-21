/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateVmeSurface
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaOpCreateVmeSurface_H__
#define __albaOpCreateVmeSurface_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaVME.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaVMESurface;
// ============================================================================
class albaOpCreateVmeSurface : public albaOp
// ============================================================================
{
public:
                albaOpCreateVmeSurface(wxString label);
               ~albaOpCreateVmeSurface();
	virtual void OnEvent(albaEvent& e);
  albaOp* Copy();

  bool Accept(albaNode* vme) {return vme && vme->IsALBAType(albaVME);};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  albaVMESurface  *m_vme; 
};
#endif // __albaOpCreateVmeSurface_H__
