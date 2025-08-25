/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeImage3D
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeImage3D_H__
#define __albaPipeImage3D_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaPipe.h"

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
class vtkALBAAssembly;


//----------------------------------------------------------------------------
// albaPipeImage3D :
//----------------------------------------------------------------------------
/**
Class Name: albaPipeImage3D.

*/
class ALBA_EXPORT albaPipeImage3D : public albaPipe
{
public:
  /** RTTI macro.*/
  albaTypeMacro(albaPipeImage3D,albaPipe);

	/** IDs for the GUI */
	enum PIPE_IMAGE3D_WIDGET_ID
	{
		ID_INTERPOLATION = Superclass::ID_LAST,
	};

  /** constructor. */
  albaPipeImage3D();

  /** destructor. */
  virtual ~albaPipeImage3D ();

  /** Create the VTK rendering pipeline*/
  virtual void Create(albaSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
  virtual void Select(bool select); 

  /** Return if the image is gray scale*/
  bool IsGrayImage();

  /** Return the current lookup table*/
  vtkLookupTable *GetLUT(){return m_ImageLUT;};

  vtkProperty *GetProperty();

	/** process events coming from Gui */
	virtual void OnEvent(albaEventBase* alba_event);

  /** Return current interpolation status */
  int GetInterpolation() const { return m_Interpolation; }
  /** Set interpolation and updates the pipe */
  void SetInterpolation(int val);
protected:

	/** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	virtual albaGUI* CreateGui();

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
  vtkALBAAssembly         *m_UsedAssembly;

  int                    m_Interpolation;
};
#endif // __albaPipeImage3D_H__
