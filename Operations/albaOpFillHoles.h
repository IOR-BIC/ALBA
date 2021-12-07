/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpFillHoles
 Authors: Matteo Giacomoni, Josef Kohout
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpFillHoles_H__
#define __albaOpFillHoles_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaGUIButton;
class albaGUIDialog;
class albaRWI;
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
   class name: albaOpFillHoles
   Operation which takes in input a polydata and retrieve the same polydata with filled hole.
*/
class ALBA_EXPORT albaOpFillHoles: public albaOp
{
public:
  /** constructor*/
	albaOpFillHoles(const wxString &label = "Fill Holes");
  /** destructor */
	~albaOpFillHoles(); 

  /** Precess events coming from other objects */
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

  /** RTTI macro */
	albaTypeMacro(albaOpFillHoles, albaOp);

  /** copy the  object */
	/*virtual*/ albaOp* Copy();

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

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

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

	albaGUIDialog						*m_Dialog;
	albaRWI									*m_Rwi;
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

	albaGUIButton  *m_ButtonOk;
	albaGUIButton  *m_ButtonFill;
  albaGUIButton  *m_ButtonUndo;

  //GUI controls
  wxComboBox* m_SmoothTypeCtrl; 
  wxTextCtrl* m_SmoothingStepsCtrl;

  std::vector<vtkPolyData*> m_VTKResult;
};
#endif
