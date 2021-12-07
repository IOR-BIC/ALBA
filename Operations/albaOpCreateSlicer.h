/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSlicer
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateSlicer_H__
#define __albaOpCreateSlicer_H__

#include "albaOp.h"
#include "albaVMEVolume.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMESlicer;

//----------------------------------------------------------------------------
// albaOpCreateSlicer :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpCreateSlicer: public albaOp
{
public:
  albaOpCreateSlicer(const wxString &label = "CreateSlicer");
 ~albaOpCreateSlicer(); 

  albaTypeMacro(albaOpCreateSlicer, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

  static bool VolumeAccept(albaVME* node) {return(node != NULL  && node->IsALBAType(albaVMEVolume));};

protected: 

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

  albaVMESlicer *m_Slicer;
  albaVME      *m_SlicedVME;
};
#endif
