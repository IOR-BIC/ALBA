/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpMeshQuality
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpMeshQuality_H__
#define __albaOpMeshQuality_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkMeshQuality;
class albaGUIDialog;
class albaRWI;
class albaInteractorCameraMove;
class vtkPolyDataMapper;
class vtkActor;
class vtkScalarBarActor;
class vtkTriangleQualityRatio;
class vtkFeatureEdges;
class vtkTubeFilter;


/**
class name: albaOpMeshQuality

Check the quality of the mesh, using an algorithm for checking the ratio of each triangle and then giving statistics about that.
 */
class ALBA_EXPORT albaOpMeshQuality: public albaOp
{
public:
	albaOpMeshQuality(const wxString &label = "Check Mesh Quality");
	~albaOpMeshQuality(); 

	virtual void OnEvent(albaEventBase *alba_event);

	albaTypeMacro(albaOpMeshQuality, albaOp);

	albaOp* Copy();

	/** Builds operation's interface. */
	void OpRun();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	void CreateOpDialog();
	void DeleteOpDialog();
	void CreatePolydataPipeline();

	albaGUIDialog								*m_Dialog;
	albaRWI									*m_Rwi;
	albaInteractorCameraMove						*m_Picker;
	vtkPolyDataMapper				*m_Mapper;
	vtkActor								*m_Actor;
	vtkScalarBarActor				*m_BarActor;

	wxStaticText						*m_LabelAverageAspectRatio;
	wxStaticText						*m_LabelMaxAspectRatio;
	wxStaticText						*m_LabelMinAspectRatio;

	vtkTriangleQualityRatio	*m_CheckMeshQuality;

  vtkFeatureEdges *m_FeatureEdgeFilter;
  vtkPolyDataMapper *m_MapperFeatureEdge;
  vtkTubeFilter *m_TubeFilter;
  vtkActor *m_ActorFeatureEdge;

  double m_Angle;

};
#endif
