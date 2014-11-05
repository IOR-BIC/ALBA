/*=========================================================================

 Program: MAF2Medical
 Module: medVisualPipePolylineGraph
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medVisualPipePolylineGraph_H__
#define __medVisualPipePolylineGraph_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medVMEDefines.h"
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkDataSetMapper;
class vtkPolyData;
class vtkActor;
class vtkProperty;
class mafAxes;
class vtkColorTransferFunction;
class vtkSphereSource;
class vtkTubeFilter;
class vtkGlyph3D;
class vtkActor2D;

//----------------------------------------------------------------------------
// medVisualPipePolylineGraph :
//----------------------------------------------------------------------------
class MED_VME_EXPORT medVisualPipePolylineGraph : public mafPipe
{
public:
  mafTypeMacro(medVisualPipePolylineGraph,mafPipe);

  medVisualPipePolylineGraph();
  virtual     ~medVisualPipePolylineGraph();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual
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
  virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};


  /** Core of the pipe */
  virtual void ExecutePipe();

  /** Add/RemoveTo Assembly Front/back */
  virtual void AddActorsToAssembly(vtkMAFAssembly *assembly);
  virtual void RemoveActorsFromAssembly(vtkMAFAssembly *assembly);

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
  mafAxes                 *m_Axes;
  vtkColorTransferFunction *m_Table;

  vtkSphereSource        *m_Sphere;
  vtkGlyph3D             *m_Glyph;
  vtkTubeFilter          *m_Tube;

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


  virtual mafGUI  *CreateGui();
};  
#endif // __medVisualPipePolylineGraph_H__
