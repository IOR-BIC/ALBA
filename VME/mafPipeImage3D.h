/*=========================================================================

 Program: MAF2
 Module: mafPipeImage3D
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

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
class vtkLookupTable;
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
class MAF_EXPORT mafPipeImage3D : public mafPipe
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
  vtkLookupTable *GetLUT(){return m_ImageLUT;};

  vtkProperty *GetProperty();

protected:
  vtkPlaneSource			      *m_ImagePlane;
  vtkTexture						    *m_ImageTexture;
  vtkLookupTable						*m_ImageLUT;

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
