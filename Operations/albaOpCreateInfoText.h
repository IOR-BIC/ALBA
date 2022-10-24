/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateInfoText
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateInfoText_H__
#define __albaOpCreateInfoText_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEInfoText;
class albaVME;
//----------------------------------------------------------------------------
// albaOpCreateInfoText :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpCreateInfoText: public albaOp
{
public:
  albaOpCreateInfoText(const wxString &label = "Create Info Text");
  ~albaOpCreateInfoText(); 

  albaTypeMacro(albaOpCreateInfoText, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();

protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	albaVMEInfoText *m_InfoText;
};
#endif
