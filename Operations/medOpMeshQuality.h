/*=========================================================================

 Program: MAF2Medical
 Module: medOpMeshQuality
 Authors: Matteo Giacomoni - Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpMeshQuality_H__
#define __medOpMeshQuality_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class vtkMeshQuality;
class mafGUIDialog;
class mafRWI;
class mafInteractorCameraMove;
class vtkPolyDataMapper;
class vtkActor;
class vtkScalarBarActor;
class vtkTriangleQualityRatio;
class vtkFeatureEdges;
class vtkTubeFilter;


/**
class name: medOpMeshQuality

Check the quality of the mesh, using an algorithm for checking the ratio of each triangle and then giving statistics about that.
 */
class MAF_EXPORT medOpMeshQuality: public mafOp
{
public:
	medOpMeshQuality(const wxString &label = "Check Mesh Quality");
	~medOpMeshQuality(); 

	virtual void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(medOpMeshQuality, mafOp);

	mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafNode *node);

	/** Builds operation's interface. */
	void OpRun();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

	void CreateOpDialog();
	void DeleteOpDialog();
	void CreatePolydataPipeline();

	mafGUIDialog								*m_Dialog;
	mafRWI									*m_Rwi;
	mafInteractorCameraMove						*m_Picker;
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
