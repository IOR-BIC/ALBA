/*=========================================================================

 Program: MAF2
 Module: mafPipeRayCast
 Authors: Alexander Savenko  -  Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeRayCast_H__
#define __mafPipeRayCast_H__

#include "mafPipe.h"
#include "mafDefines.h"

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
class vtkMAFRayCastCleaner;

//----------------------------------------------------------------------------
// mafPipeIsosurface :
//----------------------------------------------------------------------------
/** This visual pipe create a raycast rendering for volumes.
It define different levels of density for bone-blood-muscle and set different
colors for a realistic visualization 
*/
class MAF_EXPORT mafPipeRayCast : public mafPipe 
{
public:
  /** RTTI macro */
	mafTypeMacro(mafPipeRayCast, mafPipe);

  /** constructor */
	mafPipeRayCast(double skinOpacity=0.2,double fatMassOpacity=0.2,double muscleOpacity=0.2,double bloodOpacity=0.8,double boneOpacity=0.2);
  /** destructor */
	virtual  ~mafPipeRayCast();

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

  /** Set Test Mode On */
  void TestModeOn(){ m_TestMode=true; };

  
	/** IDs for the GUI */
	enum PIPE_ISOSURFACE_WIDGET_ID
	{
		ID_OPACITY_SLIDERS = Superclass::ID_LAST,
    ID_CHANGE_MODALITY,
    ID_CHANGE_OPACITY,
		ID_CAMERA_FRONT,
		ID_CAMERA_BACK,
		ID_CAMERA_LEFT,
		ID_CAMERA_RIGHT,
		ID_CAMERA_TOP,
		ID_CAMERA_BOTTOM,
    ID_LAST
	};

  enum RAY_CAST_MODALITY
  {
    CT_MODALITY,
    MR_MODALITY,
  };

  enum PRESETS
  {
    DEFAULT_PRESET,
    MUSCULAR_PRESET,
    CIRCULATORY_PRESET,
    SKELETON_PRESET,
  };

protected:
  /** Create the Gui for the visual pipe that allow the user to change the pipe's parameters.*/
	virtual mafGUI  *CreateGui();

  /** Set The levels of opacity and the colors for the Ray Cast Mapper*/
  void SetRayCastFunctions();

  /** Update pipe internal data */
  void UpdateFromData();

  /** Setting Threshold Ranges by modality*/
  void SetThresholding();

  /** attempt to auto detect modality by VME tags or scalar range*/
  void DetectModality();

  /** Set default opacity values on preset selection */
  void OnPreset();

  /** Show and Hides sliders by modality */
  void ShowHideSliders();

  /** Change view modality */
  void OnChangeModality();

  vtkPiecewiseFunction     *m_OpacityFunction;
  vtkColorTransferFunction *m_ColorFunction;
	vtkVolumeRayCastMapper   *m_RayCastMapper;
  vtkMAFRayCastCleaner     *m_RayCastCleaner;
  vtkVolume                *m_Volume;
  

  double m_SkinLowerThreshold;
  double m_SkinUpperThreshold;
  double m_FatMassLowerThreshold;
  double m_FatMassUpperThreshold;
  double m_BoneLowerThreshold;
  double m_BoneUpperThreshold;
  double m_BloodLowerThreshold;
  double m_BloodUpperThreshold;
  double m_MuscleLowerThreshold;
  double m_MuscleUpperThreshold;
  
  vtkOutlineCornerFilter   *m_OutlineBox;
  vtkPolyDataMapper        *m_OutlineMapper;
  vtkActor                 *m_OutlineActor;

  double m_FatMassOpacity;
  double m_SkinOpacity;
  double m_MuscleOpacity;
  double m_BoneOpacity;
  double m_BloodOpacity;
  double m_BloodFocus;

  int m_OnLoading;
  int m_Preset;

  double m_ScalarRange[2];

  mafGUI *m_CTSliders;
  mafGUI *m_MRSliders;
  
  bool m_BoundingBoxVisibility;
  bool m_TestMode;

  mafString m_ExtractIsosurfaceName;
  int m_Modality;
};  
#endif
