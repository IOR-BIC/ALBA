/*=========================================================================

 Program: MAF2
 Module: mafOpInteractionDebugger
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medGizmoInteractionDebugger_H__
#define __medGizmoInteractionDebugger_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"
#include "vtkPoints.h"
#include "mafVMEPolyline.h"
#include "medVMEPolylineGraph.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafEvent;
class medGizmoInteractionDebugger;

/** Facility for medical interaction stuff debug:  this class should be expanded and
refactored incrementally to ease interaction stuff debug */
class MAF_EXPORT mafOpInteractionDebugger: public mafOp
{
public:
	mafOpInteractionDebugger(const wxString &label = "mafOpInteractionDebugger");
	~mafOpInteractionDebugger(); 
	
  mafTypeMacro(mafOpInteractionDebugger, mafOp);

  virtual void OnEvent(mafEventBase *maf_event);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

  /** Builds operation's interface. */
	void OpRun();

	
  static bool ConstrainAccept(mafNode* node) {return (node != NULL && \
    (node->IsMAFType(medVMEPolylineGraph))) ;};

  void OpDo();

protected:
  /** Create the dialog interface for the importer. */
  virtual void CreateGui();  

private:

  void OnChooseConstrainVme(mafNode *vme);
  void BuildVMEPolyline( vtkPoints * in_points, mafVMEPolyline *polyline );
	void BuildPolyline3(vtkPoints *in_points);
	void BuildPolyline2(vtkPoints *in_points);
	void BuildPolyline1(vtkPoints *in_points);
	void AddMAFVMEPolylineTestConstrain1ToTree();
	void AddMAFVMEPolylineTestConstrain2ToTree();
	void AddMAFVMEPolylineTestConstrain3ToTree();
  void AddMEDGizmoDebuggerToTree();
	void BuildGraph1( vtkPolyData *inputPolyData );
	void AddMEDVMEPolylineGraphTestConstrain1ToTree();
  void RemoveTestConstraintGraph1FromTree();
  medGizmoInteractionDebugger *m_GizmoDebugger;
  mafVME *m_Constrain;

  vtkIdType m_ActiveBranchId;
};
#endif
