/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSurfaceParametric
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateSurfaceParametric_H__
#define __albaOpCreateSurfaceParametric_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMESurfaceParametric;
class albaGUI;
class albaEvent;
//----------------------------------------------------------------------------
// albaOpCreateSurfaceParametric :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpCreateSurfaceParametric: public albaOp
{
public:
  albaOpCreateSurfaceParametric(const wxString &label = "Create Parametric Surface");
  ~albaOpCreateSurfaceParametric(); 

  albaTypeMacro(albaOpCreateSurfaceParametric, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

  albaVMESurfaceParametric *m_SurfaceParametric;
};
#endif
