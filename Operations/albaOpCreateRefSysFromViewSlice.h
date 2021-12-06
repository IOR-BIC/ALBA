/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateRefSysFromViewSliceFromViewSlice
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpCreateRefSysFromViewSlice_H__
#define __albaOpCreateRefSysFromViewSlice_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMERefSys;
class albaVME;

//----------------------------------------------------------------------------
// albaOpCreateRefSysFromViewSlice :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpCreateRefSysFromViewSlice: public albaOp
{
public:
  albaOpCreateRefSysFromViewSlice(const wxString &label = "RefSysFromViewSlice");
 ~albaOpCreateRefSysFromViewSlice(); 

  albaTypeMacro(albaOpCreateRefSysFromViewSlice, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

  albaVMERefSys *m_RefSys;
	albaView *m_View;
};
#endif
