/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medOpFillHoles.h,v $
Language:  C++
Date:      $Date: 2011-05-26 08:08:41 $
Version:   $Revision: 1.1.2.7 $
Authors:   Matteo Giacomoni, Josef Kohout
==========================================================================
Copyright (c) 2007
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __medOpFillHoles_H__
#define __medOpFillHoles_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
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

/**
   class name: medOpFillHoles
   Operation which takes in input a polydata and retrieve the same polydata with filled hole.
*/
class MED_EXPORT medOpFillHoles: public mafOp
{
public:
  /** constructor*/
	medOpFillHoles(const wxString &label = "Fill Holes");
  /** destructor */
	~medOpFillHoles(); 

  /** Precess events coming from other objects */
	/*virtual*/ void OnEvent(mafEventBase *maf_event);

  /** RTTI macro */
	mafTypeMacro(medOpFillHoles, mafOp);

  /** copy the  object */
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
  //----------------------------------------------------------------------------
  // Constants:
  //----------------------------------------------------------------------------
  enum FILTER_SURFACE_ID
  {
    ID_OK = MINID,
    ID_CANCEL,
    ID_FILL,
    ID_ALL,
    ID_SMOOTH,
    ID_SMOOTH_TYPE,
    ID_SMOOTH_STEPS,
    ID_DIAMETER_LABEL,
    ID_DIAMETER,
    ID_UNDO,
  };

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);

  /** create external dialog */
	void CreateOpDialog();
  /** delete external dialog */
	void DeleteOpDialog();
  /** create visual pipeline */
	void CreatePolydataPipeline();
  /** fill holes */
	void Fill();
  /** select a hole in the data structure*/
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
  int     m_PointID;

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
