/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateRefSys
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateRefSys_H__
#define __albaOpCreateRefSys_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMERefSys;
class albaVME;

//----------------------------------------------------------------------------
// albaOpCreateRefSys :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpCreateRefSys: public albaOp
{
public:
  albaOpCreateRefSys(const wxString &label = "CreateRefSys");
 ~albaOpCreateRefSys(); 

  albaTypeMacro(albaOpCreateRefSys, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

  albaVMERefSys *m_RefSys;
};
#endif
