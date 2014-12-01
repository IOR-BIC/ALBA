/*=========================================================================

 Program: MAF2
 Module: medOpClassicICPRegistration
 Authors: Stefania Paperini, Stefano Perticoni, porting Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpClassicICPRegistration_H__
#define __medOpClassicICPRegistration_H__

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
class mafNode;
class mafVME;
class mafVMESurface;
class mafString;


/**
  class name: medOpClassicICPRegistration 
  Operation that use mafClassicICPRegistration, for matching two 
  surfaces using the iterative closest point (ICP) algorithm.
*/

class MAF_EXPORT medOpClassicICPRegistration: public mafOp
{
public:
  /** constructor */
	medOpClassicICPRegistration(wxString label);
  /** destructor */
	~medOpClassicICPRegistration();
  /** method allows to handle events from other objects*/
	void   OnEvent(mafEventBase *maf_event);
  /** clone the object and retrieve a copy*/
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Set target. */
	void SetTarget(mafNode* node);

protected:
	/** Create the gui */
  virtual void CreateGui();

	/** Allow to choose the target surface for the registration. */
	void OnChooseTarget();

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	mafVME*					m_Target;				// m_input, inherited from mafOp, is used as source
	mafVMESurface*					m_Registered;   // the output: a copy of m_input registered on m_target
	mafString					m_InputName;
	mafString					m_TargetName;
	mafString					m_ReportFilename;
	double						m_Convergence;
};
#endif
