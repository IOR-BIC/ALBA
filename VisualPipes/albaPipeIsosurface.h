/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeIsosurface
 Authors: Alexander Savenko  -  Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeIsosurface_H__
#define __albaPipeIsosurface_H__

#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
class vtkSmartVolumeMapper;
class vtkPolyDataMapper;
class vtkVolumeProperty;
class vtkPiecewiseFunction;
class vtkOutlineCornerFilter;
class albaVME;
class albaGUIFloatSlider;
class albaEventBase;
class albaVMESurface;


//----------------------------------------------------------------------------
// albaPipeIsosurface :
//----------------------------------------------------------------------------
/** This visual pipe allow to extract a surface from a volume data given a 
threshold value. The value is extracted according to the scalar values present 
into the volume data. The iso-surface is extracted in real time and can be changed 
also the opacity value of the surface extracted.*/
class ALBA_EXPORT albaPipeIsosurface : public albaPipe 
{
public:
  /** RTTI macro */
	albaTypeMacro(albaPipeIsosurface, albaPipe);

  /** constructor */
	albaPipeIsosurface();
  /** destructor */
	virtual  ~albaPipeIsosurface();

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

	void ExctractIsosurface(albaVMESurface *isoSurface = NULL);

  void UpdateFromData();

  void SetActorVisibility(int visibility);

  void SetExtractIsosurfaceName(const char *name){m_ExtractIsosurfaceName = albaString(name);}

  void SetEnableContourAnalysis(bool clean);

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
		ID_LAST
	};

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	virtual albaGUI  *CreateGui();

	vtkSmartVolumeMapper     *m_ContourMapper;
	vtkVolume                *m_Volume;
	vtkVolumeProperty				 *m_VolumeProp;
	vtkPiecewiseFunction		 *m_OpacityFunc;

	vtkOutlineCornerFilter   *m_OutlineBox;
	vtkPolyDataMapper        *m_OutlineMapper;
	vtkActor                 *m_OutlineActor;

	albaGUIFloatSlider  *m_ContourSlider;
	albaGUIFloatSlider  *m_AlphaSlider;
	double m_ContourValue;
	double m_AlphaValue;
	double m_VolumeRange[2];

  bool m_BoundingBoxVisibility;

  albaString m_ExtractIsosurfaceName;
};  
#endif // __albaPipeIsosurface_H__
