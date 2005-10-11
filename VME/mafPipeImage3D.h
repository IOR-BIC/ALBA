/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeImage3D.h,v $
  Language:  C++
  Date:      $Date: 2005-10-11 17:50:25 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeImage3D_H_
#define _mafPipeImage3D_H_

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

  vtkPlaneSource			   *m_ImagePlane;
  vtkTexture						 *m_ImageTexture;
  vtkWindowLevelLookupTable *m_ImageLUT;
  vtkPolyDataMapper			 *m_ImageMapper;
  vtkActor               *m_ImageActor;

  vtkOutlineCornerFilter *m_SelectionFilter;
  vtkPolyDataMapper      *m_SelectionMapper;
  vtkProperty            *m_SelectionProperty;
  vtkActor               *m_SelectionActor;

protected:
  vtkActor               *m_GhostActor;
};
#endif // _mafPipeImage3D_H_
