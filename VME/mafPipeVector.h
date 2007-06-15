/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVector.h,v $
  Language:  C++
  Date:      $Date: 2007-06-15 14:17:29 $
  Version:   $Revision: 1.3 $
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
#include "mafVMEImage.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkAppendPolyData;
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkPolyData;
class vtkProperty;
class mmgMaterialButton;
class mmaMaterial;
class vtkSphereSource;
class vtkConeSource;
class vtkActor;
class mafVMEVector;
class mafMatrixVector;
  

//----------------------------------------------------------------------------
// mafPipeVector :
//----------------------------------------------------------------------------
class mafPipeVector : public mafPipe
{
public:
  mafTypeMacro(mafPipeVector,mafPipe);

               mafPipeVector();
  virtual     ~mafPipeVector();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  virtual void Create(mafSceneNode *n /*,bool use_axes = true*/ ); //Can't add parameters - is Virtual


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
  vtkOutlineCornerFilter  *m_OutlineBox;
  vtkPolyDataMapper       *m_OutlineMapper;
  vtkProperty             *m_OutlineProperty;
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
  mmgMaterialButton        *m_MaterialButton;

  /** Update the position of the sphere and the position/orientation of the arrow */
  void UpdateProperty(bool fromTag = false);

  /** Draw a bunch of vectors in a frame interval */
  void AllVector(bool fromTag = false);
  
  virtual mmgGui  *CreateGui();
};  
#endif // __mafPipeSurface_H__
