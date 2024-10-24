/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVector
 Authors: Roberto Mucci
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVector_H__
#define __albaPipeVector_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkAppendPolyData;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkProperty;
class albaGUIMaterialButton;
class mmaMaterial;
class vtkSphereSource;
class vtkConeSource;
class vtkActor;
class albaVMEVector;
class albaMatrixVector;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_VECTOR(ALBA_EXPORT,albaTimeStamp);
#endif
  
//----------------------------------------------------------------------------
// albaPipeVector :
//----------------------------------------------------------------------------
/** Visual pipe for albaVMEVector: creates two platforms and visualize data stored 
in albaVMEVector as two arrows.*/
class ALBA_EXPORT albaPipeVector : public albaPipe
{
public:
  albaTypeMacro(albaPipeVector,albaPipe);

               albaPipeVector();
  virtual     ~albaPipeVector();

  /** process events coming from Gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** IDs for the GUI */
  enum PIPE_SURFACE_WIDGET_ID
  {
    ID_USE_VTK_PROPERTY = Superclass::ID_LAST,
    ID_USE_LOOKUP_TABLE,
    ID_USE_ARROW,
    ID_USE_SPHERE,
    ID_USE_BUNCH,
    ID_INTERVAL,
    ID_STEP,
    ID_ALL_BUNCH,
    ID_LAST
  };

protected:
  vtkPolyDataMapper	      *m_Mapper;
  vtkPolyDataMapper	      *m_MapperBunch;
  vtkActor                *m_Actor;
  vtkActor                *m_ActorBunch;
  vtkActor                *m_OutlineActor;
  mmaMaterial             *m_Material;

  vtkAppendPolyData        *m_Apd;
  vtkConeSource            *m_ArrowTip;
  vtkSphereSource          *m_Sphere;
  vtkPolyData              *m_Data;
  albaVMEVector             *m_Vector;
  vtkAppendPolyData        *m_Bunch;

  std::vector<albaTimeStamp> m_TimeVector;
  albaMatrixVector *m_MatrixVector;

  int                       m_UseArrow;
  int                       m_UseSphere;
  int                       m_UseVTKProperty;
  int                       m_Interval;
  int                       m_Step;
  int                       m_AllVector;
  int                       m_UseBunch;
  int                       m_AllBunch;
  mmaMaterial              *m_SurfaceMaterial;
  albaGUIMaterialButton        *m_MaterialButton;

  /** Update the position of the sphere and the position/orientation of the arrow */
  void UpdateProperty(bool fromTag = false);

  /** Draw a bunch of vectors in a frame interval */
  void AllVector(bool fromTag = false);
  
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual albaGUI  *CreateGui();

  /** Enable widgets */
  void EnableWidget();

};  
#endif // __albaPipeSurface_H__
