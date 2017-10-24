/*=========================================================================

 Program: MAF2
 Module: mafOpClassicICPRegistration
 Authors: Stefania Paperini, Stefano Perticoni, porting Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpClassicICPRegistration_H__
#define __mafOpClassicICPRegistration_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafGUI;
class mafEvent;
class mafVME;
class mafVME;
class mafVMESurface;
class mafString;


/**
  class name: mafOpClassicICPRegistration 
  Operation that use mafClassicICPRegistration, for matching two 
  surfaces using the iterative closest point (ICP) algorithm.
*/

class MAF_EXPORT mafOpClassicICPRegistration: public mafOp
{
public:
  /** constructor */
	mafOpClassicICPRegistration(wxString label);
  /** destructor */
	~mafOpClassicICPRegistration();
  /** method allows to handle events from other objects*/
	void   OnEvent(mafEventBase *maf_event);
  /** clone the object and retrieve a copy*/
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME* vme);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Set target. */
	void SetTarget(mafVME* node);

protected:
	/** Create the gui */
  virtual void CreateGui();

	/** Allow to choose the target surface for the registration. */
	void OnChooseTarget();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	mafVME*					m_Target;				// m_input, inherited from mafOp, is used as source
	mafVME*					m_Registered;   // the output: a copy of m_input registered on m_target
	mafString				m_InputName;
	mafString				m_TargetName;
	mafString				m_ReportFilename;
	double					m_Convergence;
	int							m_CopySubTree;
};
#endif
