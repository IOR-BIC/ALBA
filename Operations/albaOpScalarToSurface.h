/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpScalarToSurface
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpScalarToSurface_H__
#define __albaOpScalarToSurface_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMESurface;

//----------------------------------------------------------------------------
// albaOpScalarToSurface :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpScalarToSurface: public albaOp
{
public:
  /** Constructor. */
  albaOpScalarToSurface(const wxString &label = "ScalarToSurface");

  /** Destructor. */
 ~albaOpScalarToSurface(); 

 /** RTTI Macro. */
  albaTypeMacro(albaOpScalarToSurface, albaOp);

  /** Copy the operation. */
  albaOp* Copy();

  /** Builds the output surface. */
  void OpRun();

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  albaVMESurface *m_Surface;
};
#endif
