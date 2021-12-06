/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateSpline
 Authors: Daniele Giunchi & Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateSpline_H__
#define __albaOpCreateSpline_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEPolylineSpline;
class albaGUI;
class albaEvent;
//----------------------------------------------------------------------------
// albaOpCreateSpline :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpCreateSpline: public albaOp
{
public:
  albaOpCreateSpline(const wxString &label = "Create Parametric Surface");
  ~albaOpCreateSpline(); 

  albaTypeMacro(albaOpCreateSpline, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

  albaVMEPolylineSpline *m_PolylineSpline;
};
#endif
