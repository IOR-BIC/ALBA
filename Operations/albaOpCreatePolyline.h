/*=========================================================================
 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreatePolyline
 Authors: Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpCreatePolyline_H__
#define __albaOpCreatePolyline_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEPolyline;
class albaGUI;
class albaEvent;
class albaInteractor2DMeasure_Point;

//----------------------------------------------------------------------------
// Class Name: albaOpCreatePolyline
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpCreatePolyline: public albaOp
{
public:
  albaOpCreatePolyline(const wxString &label = "Create Polyline");
  ~albaOpCreatePolyline(); 

  albaTypeMacro(albaOpCreatePolyline, albaOp);

  albaOp* Copy();

	/** Builds operation's interface. */
	virtual void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Receive events coming from the user interface.*/
	virtual void OnEvent(albaEventBase *alba_event);

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char** GetIcon();

protected: 

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	/** Create the Operation GUI */
	virtual void CreateGui();

	void UpdateGui();

	void CreatePolyline();

	albaInteractor2DMeasure_Point* m_PointInteractor;
	albaVMEPolyline *m_Polyline;
};
#endif
