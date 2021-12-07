/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpInteractionDebugger
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaGizmoInteractionDebugger_H__
#define __albaGizmoInteractionDebugger_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"
#include "vtkPoints.h"
#include "albaVMEPolyline.h"
#include "albaVMEPolylineGraph.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaEvent;
class albaGizmoInteractionDebugger;

/** Facility for medical interaction stuff debug:  this class should be expanded and
refactored incrementally to ease interaction stuff debug */
class ALBA_EXPORT albaOpInteractionDebugger: public albaOp
{
public:
	albaOpInteractionDebugger(const wxString &label = "albaOpInteractionDebugger");
	~albaOpInteractionDebugger(); 
	
  albaTypeMacro(albaOpInteractionDebugger, albaOp);

  virtual void OnEvent(albaEventBase *alba_event);

  albaOp* Copy();

  /** Builds operation's interface. */
	void OpRun();
	
  static bool ConstrainAccept(albaVME* node) {return (node != NULL && \
    (node->IsALBAType(albaVMEPolylineGraph))) ;};

  void OpDo();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  

private:

  void OnChooseConstrainVme(albaVME *vme);
  void BuildVMEPolyline( vtkPoints * in_points, albaVMEPolyline *polyline );
	void BuildPolyline3(vtkPoints *in_points);
	void BuildPolyline2(vtkPoints *in_points);
	void BuildPolyline1(vtkPoints *in_points);
	void AddALBAVMEPolylineTestConstrain1ToTree();
	void AddALBAVMEPolylineTestConstrain2ToTree();
	void AddALBAVMEPolylineTestConstrain3ToTree();
  void AddMEDGizmoDebuggerToTree();
	void BuildGraph1( vtkPolyData *inputPolyData );
	void AddalbaVMEPolylineGraphTestConstrain1ToTree();
  void RemoveTestConstraintGraph1FromTree();
  albaGizmoInteractionDebugger *m_GizmoDebugger;
  albaVME *m_Constrain;

  vtkIdType m_ActiveBranchId;
};
#endif
