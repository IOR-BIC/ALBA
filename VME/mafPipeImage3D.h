/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeImage3D.h,v $
  Language:  C++
  Date:      $Date: 2007-01-23 13:50:13 $
  Version:   $Revision: 1.6 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeImage3D_H__
#define __mafPipeImage3D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkPlaneSource;
class vtkTexture;
class vtkWindowLevelLookupTable;
class vtkPolyDataMapper;
class vtkActor;
class vtkProperty;
class vtkOutlineCornerFilter;
class vtkMAFAssembly;

//----------------------------------------------------------------------------
// mafPipeImage3D :
//----------------------------------------------------------------------------
class mafPipeImage3D : public mafPipe
{
public:
  mafTypeMacro(mafPipeImage3D,mafPipe);

           mafPipeImage3D();
  virtual ~mafPipeImage3D ();

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

  /** Return if the image is gray scale*/
  bool IsGrayImage();

  /** Return the current lookup table*/
  vtkWindowLevelLookupTable *GetLUT(){return m_ImageLUT;};

  vtkProperty *GetProperty();

protected:
  vtkPlaneSource			   *m_ImagePlane;
  vtkTexture						 *m_ImageTexture;
  vtkWindowLevelLookupTable *m_ImageLUT;
  vtkPolyDataMapper			 *m_ImageMapper;
  vtkActor               *m_ImageActor;

  vtkOutlineCornerFilter *m_SelectionFilter;
  vtkPolyDataMapper      *m_SelectionMapper;
  vtkProperty            *m_SelectionProperty;
  vtkActor               *m_SelectionActor;

  vtkActor               *m_GhostActor;
  vtkMAFAssembly         *m_UsedAssembly;
};
#endif // __mafPipeImage3D_H__
