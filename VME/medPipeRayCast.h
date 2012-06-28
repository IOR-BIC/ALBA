/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeIsosurface.h,v $
Language:  C++
Date:      $Date: 2012-03-08 13:03:50 $
Version:   $Revision: 1.15.2.5 $
Authors:   Alexander Savenko  -  Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeRayCast_H__
#define __medPipeRayCast_H__

#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkVolume;
class mafVME;
class mafGUIFloatSlider;
class mafEventBase;
class vtkPiecewiseFunction;
class vtkColorTransferFunction;
class vtkVolumeRayCastMapper;
class vtkImageCast;
class vtkPolyDataMapper;
class vtkOutlineCornerFilter;
class vtkMEDRayCastCleaner;
class vtkMAFVolumeResample;

//----------------------------------------------------------------------------
// mafPipeIsosurface :
//----------------------------------------------------------------------------
/** This visual pipe create a raycast rendering for volumes.
It define different levels of density for bone-blood-muscle and set different
colors for a realistic visualization 
*/
class MAF_EXPORT medPipeRayCast : public mafPipe 
{
public:
  /** RTTI macro */
	mafTypeMacro(medPipeRayCast, mafPipe);

  /** constructor */
	medPipeRayCast(double muscleOpacity=0.15,double bloodOpacity=0.8,double boneOpacity=0.2);
  /** destructor */
	virtual  ~medPipeRayCast();

	/** process events coming from Gui */
	virtual void OnEvent(mafEventBase *maf_event);

  /** Create the VTK rendering pipeline*/
	virtual void Create(mafSceneNode *n);

  /** Manage the actor selection by showing the corner box around the actor when the corresponding VME is selected.*/
	virtual void Select(bool select);

  /** return the contour value. */
  void  EnableBoundingBoxVisibility(bool enable = true);

  /** Set the visibility of the actor */
  void SetActorVisibility(int visibility);

	/** IDs for the GUI */
	enum PIPE_ISOSURFACE_WIDGET_ID
	{
		ID_OPACITY_SLIDERS = Superclass::ID_LAST,
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

  /** Set The levels of opacity and the colors for the Ray Cast Mapper*/
  void SetRayCastFunctions();

  /** Update pipe internal data */
  void UpdateFromData();

  vtkPiecewiseFunction     *m_OpacityFunction;
  vtkColorTransferFunction *m_ColorFunction;
	vtkVolumeRayCastMapper   *m_RayCastMapper;
  vtkMEDRayCastCleaner     *m_RayCastCleaner;
  vtkImageCast             *m_VolumeCaster;
  vtkMAFVolumeResample		 *m_ResampleFilter;	
	vtkVolume                *m_Volume;
  

  double m_BoneLowerThreshold;
  double m_BloodLowerThreshold;
  double m_BloodUpperThreshold;
  double m_MuscleLowerThreshold;
  double m_MuscleUpperThreshold;
  
  vtkOutlineCornerFilter   *m_OutlineBox;
  vtkPolyDataMapper        *m_OutlineMapper;
  vtkActor                 *m_OutlineActor;

  double m_MuscleOpacity;
  double m_BoneOpacity;
  double m_BloodOpacity;

  int m_OnLoading;

  bool m_BoundingBoxVisibility;

  mafString m_ExtractIsosurfaceName;
};  
#endif
