/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpInteractionDebugger.h,v $
  Language:  C++
  Date:      $Date: 2008-04-09 14:22:31 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni   
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medGizmoInteractionDebugger_H__
#define __medGizmoInteractionDebugger_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
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
class medOpInteractionDebugger: public mafOp
{
public:
	medOpInteractionDebugger(const wxString &label = "medOpInteractionDebugger");
	~medOpInteractionDebugger(); 
	
  mafTypeMacro(medOpInteractionDebugger, mafOp);

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
