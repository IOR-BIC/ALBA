/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipePolylineGraph
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVisualPipePolylineGraph_H__
#define __albaVisualPipePolylineGraph_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkDataSetMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class albaAxes;
class vtkColorTransferFunction;
class vtkSphereSource;
class vtkALBATubeFilter;
class vtkGlyph3D;
class vtkActor2D;

//----------------------------------------------------------------------------
// albaVisualPipePolylineGraph :
//----------------------------------------------------------------------------
class ALBA_EXPORT albaVisualPipePolylineGraph : public albaPipe
{
public:
  albaTypeMacro(albaVisualPipePolylineGraph,albaPipe);

  albaVisualPipePolylineGraph();
  virtual     ~albaVisualPipePolylineGraph();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  virtual void Create(albaSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_POLYLINE_GRAPH_WIDGET_ID
  {
    ID_SCALARS = Superclass::ID_LAST,
    ID_LUT,
    ID_SCALAR_MAP_ACTIVE,
    ID_USE_VTK_PROPERTY,
    ID_TUBE_RADIUS,
    ID_TUBE_RESOLUTION,
    ID_TUBE_CAPPING,
    ID_SPHERE_RADIUS,
    ID_SPHERE_RESOLUTION,
    ID_SCALAR_DIMENSION,
    ID_SCALAR,
    ID_POLYLINE_REPRESENTATION,
    ID_SHOW_BRANCH_ID,
    ID_LAST
  };

  enum PIPE_POLYLINE_GRAPH_TYPE_SCALARS
  {
    POINT_TYPE = 0,
    CELL_TYPE,
  };

  enum POLYLINE_REPRESENTATION
  {
    POLYLINE = 0,
    TUBE,
    GLYPH,
    GLYPH_UNCONNECTED,
  };

  /** Set the visual representation of the polyline.
  Acceptable values are 0 (POLYLINE), 1 (TUBE) or 2 (SPHERE GLYPHED).*/
  void SetRepresentation(int representation);

  /** Get assembly front/back */
  virtual vtkALBAAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkALBAAssembly *GetAssemblyBack(){return m_AssemblyBack;};


  /** Core of the pipe */
  virtual void ExecutePipe();

  /** Add/RemoveTo Assembly Front/back */
  virtual void AddActorsToAssembly(vtkALBAAssembly *assembly);
  virtual void RemoveActorsFromAssembly(vtkALBAAssembly *assembly);

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

  /** Set/Get Active Scalar */
  void SetActiveScalar(int index){m_ScalarIndex = index;};
  int GetScalarIndex(){return m_ScalarIndex;};

  /** Get Number of Scalars */
  int GetNumberOfArrays(){return m_NumberOfArrays;};

protected:

  vtkDataSetMapper        *m_Mapper;
  vtkActor                *m_Actor;
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
  vtkActor                *m_OutlineActor;
  albaAxes                 *m_Axes;
  vtkColorTransferFunction *m_Table;

  vtkSphereSource        *m_Sphere;
  vtkGlyph3D             *m_Glyph;
	vtkALBATubeFilter          *m_Tube;

  /** Initialize representation, capping, radius and resolution variables.*/
  void InitializeFromTag();

  void CreateFieldDataControlArrays();
  void UpdateProperty(bool fromTag = false);
  /**Update data value to selected scalar */
  void UpdateScalars();
  /** Update the visualization with changed scalar*/
  void UpdatePipeFromScalars();

  wxString                *m_ScalarsName;
  wxString                *m_ScalarsVTKName;

  int                      m_PointCellArraySeparation;
  int                      m_ScalarIndex;
  int                      m_NumberOfArrays;
  int                      m_ActiveScalarType;

  //Branch id stuff
  int m_ShowBranchId;
  vtkActor2D *m_ActorBranchId;
  //////////////////////////////////////////////////////////////////////////

  int m_Capping;
  int m_Representation;
  int m_ScalarDim;
  double m_TubeRadius;
  double m_SphereRadius;
  double m_TubeResolution;
  double m_SphereResolution;


  virtual albaGUI  *CreateGui();
};  
#endif // __albaVisualPipePolylineGraph_H__
