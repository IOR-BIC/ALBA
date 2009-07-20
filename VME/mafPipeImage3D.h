/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeImage3D.h,v $
  Language:  C++
  Date:      $Date: 2009-07-20 14:38:40 $
  Version:   $Revision: 1.6.6.1 $
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
/**
Class Name: mafPipeImage3D.

*/
class mafPipeImage3D : public mafPipe
{
public:
  /** RTTI macro.*/
  mafTypeMacro(mafPipeImage3D,mafPipe);

   /** constructor. */
           mafPipeImage3D();

  /** destructor. */
  virtual ~mafPipeImage3D ();

  /** Create the VTK rendering pipeline*/
  virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Return if the image is gray scale*/
  bool IsGrayImage();

  /** Return the current lookup table*/
  vtkWindowLevelLookupTable *GetLUT(){return m_ImageLUT;};

  vtkProperty *GetProperty();

protected:
  vtkPlaneSource			      *m_ImagePlane;
  vtkTexture						    *m_ImageTexture;
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
