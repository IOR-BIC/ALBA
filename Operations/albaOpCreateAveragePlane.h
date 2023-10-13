/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateAveragePlane
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpCreateAveragePlane_H__
#define __albaOpCreateAveragePlane_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMELandmarkCloud;
class albaVME;

//----------------------------------------------------------------------------
// albaOpCreateAveragePlane :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpCreateAveragePlane: public albaOp
{
public:
  albaOpCreateAveragePlane(const wxString &label = "CreateGroup");
  ~albaOpCreateAveragePlane(); 

  albaTypeMacro(albaOpCreateAveragePlane, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

	/** undo the operation. */
	void OpUndo();

protected: 

	albaVME *m_SelVME;

	static int PlaneAccept(albaVME *vme);

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME *node);
};
#endif
