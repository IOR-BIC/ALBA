/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateAverageLandmark
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpCreateAverageLandmark_H__
#define __albaOpCreateAverageLandmark_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMELandmarkCloud;
class albaVME;

//----------------------------------------------------------------------------
// albaOpCreateAverageLandmark :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpCreateAverageLandmark: public albaOp
{
public:
  albaOpCreateAverageLandmark(const wxString &label = "CreateGroup");
  ~albaOpCreateAverageLandmark(); 

  albaTypeMacro(albaOpCreateAverageLandmark, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

	/** undo the operation. */
	void OpUndo();

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	// Limb cloud from motion: animated, we will insert stick tip here
	albaVMELandmarkCloud  *m_LimbCloud;
	int m_NewIndex;
};
#endif
