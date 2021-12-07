/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateGroup
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateGroup_H__
#define __albaOpCreateGroup_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEGroup;
class albaVME;
//----------------------------------------------------------------------------
// albaOpCreateGroup :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpCreateGroup: public albaOp
{
public:
  albaOpCreateGroup(const wxString &label = "CreateGroup");
  ~albaOpCreateGroup(); 

  albaTypeMacro(albaOpCreateGroup, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  albaVMEGroup *m_Group;
};
#endif
