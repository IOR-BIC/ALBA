/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpClassicICPRegistration.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 08:00:03 $
  Version:   $Revision: 1.3.2.3 $
  Authors:   Stefania Paperini, Stefano Perticoni, porting Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpClassicICPRegistration_H__
#define __medOpClassicICPRegistration_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
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

class MED_EXPORT medOpClassicICPRegistration: public mafOp
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
