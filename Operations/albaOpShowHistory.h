/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpShowHistory
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpShowHistory_H__
#define __albaOpShowHistory_H__

#include "albaOp.h"
#include "albaVME.h"
#include "albaSmartPointer.h"
//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaVMEIterator;
class albaAttributesMap;
class albaGUIDialog;

//----------------------------------------------------------------------------
// albaOpShowHistory :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpShowHistory: public albaOp
{
public:
  albaOpShowHistory(const wxString &label = "Show History");
  ~albaOpShowHistory(); 

  albaTypeMacro(albaOpShowHistory, albaOp);

  albaOp* Copy();

  void OpRun();
  void OpDo();
    	
protected: 

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	typedef std::map<albaString, albaAutoPointer<albaAttribute> > albaAttributesMap;

	albaGUIDialog *m_Dialog;
	bool m_DialogIsOpened;
};
#endif
