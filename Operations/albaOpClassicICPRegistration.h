/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpClassicICPRegistration
 Authors: Stefania Paperini, Stefano Perticoni, porting Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpClassicICPRegistration_H__
#define __albaOpClassicICPRegistration_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaGUI;
class albaEvent;
class albaVME;
class albaVME;
class albaVMESurface;
class albaString;


/**
  class name: albaOpClassicICPRegistration 
  Operation that use albaClassicICPRegistration, for matching two 
  surfaces using the iterative closest point (ICP) algorithm.
*/

class ALBA_EXPORT albaOpClassicICPRegistration: public albaOp
{
public:
  /** constructor */
	albaOpClassicICPRegistration(wxString label);
  /** destructor */
	~albaOpClassicICPRegistration();
  /** method allows to handle events from other objects*/
	void   OnEvent(albaEventBase *alba_event);
  /** clone the object and retrieve a copy*/
	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Set target. */
	void SetTarget(albaVME* node);

protected:
	/** Create the gui */
  virtual void CreateGui();

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME* vme);

	/** Allow to choose the target surface for the registration. */
	void OnChooseTarget();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	albaVME*					m_Target;				// m_input, inherited from albaOp, is used as source
	albaVME*					m_Registered;   // the output: a copy of m_input registered on m_target
	albaString				m_InputName;
	albaString				m_TargetName;
	albaString				m_ReportFilename;
	double					m_Convergence;
	int							m_CopySubTree;
};
#endif
