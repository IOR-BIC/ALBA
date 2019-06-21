/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurfaceGPU
 Authors: Alexander Savenko  -  Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeIsosurfaceGPU_H__
#define __albaPipeIsosurfaceGPU_H__

#include "albaPipe.h"
#include "vtkALBAContourVolumeMapperGPU.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
//class vtkALBAContourVolumeMapperGPU;
class vtkPolyDataMapper;
class vtkOutlineCornerFilter;
class albaVME;
class albaGUIFloatSlider;
class albaEventBase;
class albaVMESurface;

//----------------------------------------------------------------------------
// albaPipeIsosurfaceGPU :
//----------------------------------------------------------------------------
/** This visual pipe allow to extract a surface from a volume data given a 
threshold value. The value is extracted according to the scalar values present 
into the volume data. The iso-surface is extracted in real time and can be changed 
also the opacity value of the surface extracted.*/
class ALBA_EXPORT albaPipeIsosurfaceGPU : public albaPipe 
{
public:
	albaTypeMacro(albaPipeIsosurfaceGPU, albaPipe);

	albaPipeIsosurfaceGPU();
	virtual  ~albaPipeIsosurfaceGPU();

	/** process events coming from Gui */
	virtual void OnEvent(albaEventBase *alba_event);

  /** Create the VTK rendering pipeline*/
	virtual void Create(albaSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
	virtual void Select(bool select);

	/** Set the contour value for contour mapper, return false if contour mapper is NULL, otherwise return true. */
	bool   SetContourValue(float value);

	/** return the contour value. */
	float  GetContourValue();

	void SetAlphaValue(double value);
	double GetAlphaValue(){return m_AlphaValue;};

  /** return the contour value. */
  void  EnableBoundingBoxVisibility(bool enable = true);

	void ExctractIsosurface();

  void UpdateFromData();

	/** IDs for the GUI */
	enum PIPE_ISOSURFACE_WIDGET_ID
	{
		ID_CONTOUR_VALUE = Superclass::ID_LAST,
		ID_GENERATE_ISOSURFACE,
		ID_ALPHA_VALUE,
		ID_CAMERA_FRONT,
		ID_CAMERA_BACK,
		ID_CAMERA_LEFT,
		ID_CAMERA_RIGHT,
		ID_CAMERA_TOP,
		ID_CAMERA_BOTTOM,
    ID_ENABLE_GPU,
		ID_LAST
	};

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	virtual albaGUI  *CreateGui();

	albaVMESurface            *m_IsosurfaceVme;

  vtkALBAContourVolumeMapperGPU   *m_ContourMapper; 
	vtkVolume                *m_Volume;

	vtkOutlineCornerFilter   *m_OutlineBox;
	vtkPolyDataMapper        *m_OutlineMapper;
	vtkActor                 *m_OutlineActor;

	albaGUIFloatSlider  *m_ContourSlider;
	albaGUIFloatSlider  *m_AlphaSlider;
	double m_ContourValue;
	double m_AlphaValue;

  int m_EnableGPU;  ///<Non-zero, if the GPU support for visualization is used (default)
  bool m_BoundingBoxVisibility;
};  
#endif // __albaPipeIsosurfaceGPU_H__
