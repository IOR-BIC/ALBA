/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeIsosurface.h,v $
Language:  C++
Date:      $Date: 2008-12-18 08:57:31 $
Version:   $Revision: 1.15.2.2 $
Authors:   Alexander Savenko  -  Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafPipeIsosurface_H__
#define __mafPipeIsosurface_H__

#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
class vtkMAFContourVolumeMapper;
class vtkPolyDataMapper;
class vtkOutlineCornerFilter;
class mafVME;
class mafGUIFloatSlider;
class mafEventBase;
class mafVMESurface;

//----------------------------------------------------------------------------
// mafPipeIsosurface :
//----------------------------------------------------------------------------
/** This visual pipe allow to extract a surface from a volume data given a 
threshold value. The value is extracted according to the scalar values present 
into the volume data. The iso-surface is extracted in real time and can be changed 
also the opacity value of the surface extracted.*/
class mafPipeIsosurface : public mafPipe 
{
public:
	mafTypeMacro(mafPipeIsosurface, mafPipe);

	mafPipeIsosurface();
	virtual  ~mafPipeIsosurface();

	/** process events coming from Gui */
	virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
	virtual void Create(mafSceneNode *n);

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

  void SetActorVisibility(int visibility);

  void SetExtractIsosurfaceName(const char *name){m_ExtractIsosurfaceName = mafString(name);}

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
	virtual mafGUI  *CreateGui();

	mafVMESurface            *m_IsosurfaceVme;

	vtkMAFContourVolumeMapper   *m_ContourMapper; 
	vtkVolume                *m_Volume;

	vtkOutlineCornerFilter   *m_OutlineBox;
	vtkPolyDataMapper        *m_OutlineMapper;
	vtkActor                 *m_OutlineActor;

	mafGUIFloatSlider  *m_ContourSlider;
	mafGUIFloatSlider  *m_AlphaSlider;
	double m_ContourValue;
	double m_AlphaValue;

  bool m_BoundingBoxVisibility;

  mafString m_ExtractIsosurfaceName;
};  
#endif // __mafPipeIsosurface_H__
