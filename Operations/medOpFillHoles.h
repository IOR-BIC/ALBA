/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medOpFillHoles.h,v $
Language:  C++
Date:      $Date: 2009-07-02 08:15:31 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni, Josef Kohout
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __medOpFillHoles_H__
#define __medOpFillHoles_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafGUIButton;
class mafGUIDialog;
class mafRWI;
class mmiSelectPoint;
class vtkFeatureEdges;
class vtkHoleConnectivity;
class vtkPolyDataMapper;
class vtkActor;
class vtkAppendPolyData;
class vtkFillingHole;
class vtkSphereSource;
class vtkPolyData;
class vtkGlyph3D;

//----------------------------------------------------------------------------
// medOpFillHoles :
//----------------------------------------------------------------------------
/** */
class medOpFillHoles: public mafOp
{
public:
	medOpFillHoles(const wxString &label = "Fill Holes");
	~medOpFillHoles(); 

	/*virtual*/ void OnEvent(mafEventBase *maf_event);

	mafTypeMacro(medOpFillHoles, mafOp);

	/*virtual*/ mafOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(mafNode *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	/** Makes the undo for the operation. */
	/*virtual*/ void OpUndo();

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);

	void CreateOpDialog();
	void DeleteOpDialog();
	void CreatePolydataPipeline();
	void Fill();
	void SelectHole(int pointID);

	vtkPolyData							*m_OriginalPolydata;
  std::vector<vtkPolyData*> m_UndoStack;

	mafGUIDialog						*m_Dialog;
	mafRWI									*m_Rwi;
	mmiSelectPoint					*m_Picker;

	vtkFeatureEdges					*m_ExctractFreeEdges;
	vtkHoleConnectivity			*m_ExctractHole;
  vtkGlyph3D              *m_Glyph;

	vtkPolyDataMapper				*m_MapperSelectedHole;
	vtkPolyDataMapper				*m_MapperSurface;
	vtkPolyDataMapper				*m_MapperHoles;
	vtkActor								*m_ActorHoles;
	vtkActor								*m_ActorSelectedHole;
	vtkActor								*m_ActorSurface;

	vtkSphereSource					*m_Sphere;

	double	m_CoordPointSelected[3];
	bool		m_SelectedPoint;
	int			m_AllHoles;
	int			m_Smooth;
  int     m_SmoothType;
  int     m_ThinPlateSmoothingSteps;

	double	m_Diameter;

	mafGUIButton  *m_ButtonOk;
	mafGUIButton  *m_ButtonFill;
  mafGUIButton  *m_ButtonUndo;

  //GUI controls
  wxComboBox* m_SmoothTypeCtrl; 
  wxTextCtrl* m_SmoothingStepsCtrl;

  std::vector<vtkPolyData*> m_VTKResult;
};
#endif
