/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoClassicICPRegistration.h,v $
  Language:  C++
  Date:      $Date: 2006-12-11 09:13:15 $
  Version:   $Revision: 1.1 $
  Authors:   Matteo Giacomoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoClassicICPRegistration_H__
#define __mmoClassicICPRegistration_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmgGui;
class mafEvent;
class mafNode;
class mafVME;
class mafVMESurface;
class mafString;

//----------------------------------------------------------------------------
// mmoClassicICPRegistration :
//----------------------------------------------------------------------------
class mmoClassicICPRegistration: public mafOp
{
public:
	mmoClassicICPRegistration(wxString label);
	~mmoClassicICPRegistration(); 
	void   OnEvent(mafEventBase *maf_event);
	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode* vme);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

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
