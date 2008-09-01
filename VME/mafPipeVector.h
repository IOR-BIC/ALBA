/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVector.h,v $
  Language:  C++
  Date:      $Date: 2008-09-01 08:33:13 $
  Version:   $Revision: 1.8 $
  Authors:   Roberto Mucci
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeVector_H__
#define __mafPipeVector_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkAppendPolyData;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkProperty;
class mafGUIMaterialButton;
class mmaMaterial;
class vtkSphereSource;
class vtkConeSource;
class vtkActor;
class mafVMEVector;
class mafMatrixVector;
  
//----------------------------------------------------------------------------
// mafPipeVector :
//----------------------------------------------------------------------------
/** Visual pipe for mafVMEVector: creates two platforms and visualize data stored 
in mafVMEVector as two arrows.*/
class mafPipeVector : public mafPipe
{
public:
  mafTypeMacro(mafPipeVector,mafPipe);

               mafPipeVector();
  virtual     ~mafPipeVector();

  /** process events coming from Gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

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
  mafVMEVector             *m_Vector;
  vtkAppendPolyData        *m_Bunch;

  std::vector<mafTimeStamp> m_TimeVector;
  mafMatrixVector *m_MatrixVector;

  int                       m_UseArrow;
  int                       m_UseSphere;
  int                       m_UseVTKProperty;
  int                       m_Interval;
  int                       m_Step;
  int                       m_AllVector;
  int                       m_UseBunch;
  int                       m_AllBunch;
  mmaMaterial              *m_SurfaceMaterial;
  mafGUIMaterialButton        *m_MaterialButton;

  /** Update the position of the sphere and the position/orientation of the arrow */
  void UpdateProperty(bool fromTag = false);

  /** Draw a bunch of vectors in a frame interval */
  void AllVector(bool fromTag = false);
  
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
  virtual mafGUI  *CreateGui();

  /** Enable widgets */
  void EnableWidget();

};  
#endif // __mafPipeSurface_H__
