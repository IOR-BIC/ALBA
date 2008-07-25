/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medVisualPipePolylineGraph.h,v $
Language:  C++
Date:      $Date: 2008-07-25 11:19:42 $
Version:   $Revision: 1.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medVisualPipePolylineGraph_H__
#define __medVisualPipePolylineGraph_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
// medVisualPipePolylineGraph :
//----------------------------------------------------------------------------
class medVisualPipePolylineGraph : public mafPipe
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
