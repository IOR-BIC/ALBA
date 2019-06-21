/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateGenericVme
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaOpCreateGenericVme_H__
#define __albaOpCreateGenericVme_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaOp.h"
//----------------------------------------------------------------------------
// Forward Refs:
//----------------------------------------------------------------------------
class albaNodeGeneric;
// ============================================================================
class albaOpCreateGenericVme : public albaOp
// ============================================================================
{
public:
                albaOpCreateGenericVme(wxString label);
               ~albaOpCreateGenericVme();
	virtual void OnEvent(albaEvent& e);
  albaOp* Copy();

  bool Accept(albaNode* vme) {return vme != NULL;};
  void OpRun();
  void OpDo();
  void OpUndo();

protected:
  albaNodeGeneric  *m_vme; 
};
#endif // __albaOpCreateGenericVme_H__
