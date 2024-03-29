/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeDRR
 Authors: Paolo Quadrani - porting Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVolumeDRR_H__
#define __albaPipeVolumeDRR_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkImageCast;
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkXRayVolumeMapper;
class vtkActor;
class vtkVolume;
class vtkLookupTable;
class vtkImageResample;
class albaGUIFloatSlider;
/**
className: albaPipeVolumeDRR
Pipe which uses BES vtkXRayVolumeMapper.
*/
class ALBA_EXPORT albaPipeVolumeDRR : public albaPipe
{
public:
  /** RTTI macro*/
  albaTypeMacro(albaPipeVolumeDRR,albaPipe);
  /** constructor*/
           albaPipeVolumeDRR();
  /** destructor */
  virtual ~albaPipeVolumeDRR();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_DRR_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_VOLUME_COLOR,
		ID_EXPOSURE_CORRECTION_L,
		ID_EXPOSURE_CORRECTION_H,
		ID_GAMMA,
		ID_CAMERA_ANGLE,
		ID_CAMERA_POSITION,
		ID_CAMERA_FOCUS,
		ID_CAMERA_ROLL,
		ID_EXPORT,
		ID_RESAMPLE_FACTOR,
		ID_LAST
  };
  /** create pipe*/
  virtual void Create(albaSceneNode *n);
  /** called when the vme has been selected*/
  virtual void Select(bool select); 
        /** set color used by the mapper*/
	void SetColor(wxColor color);
        /** set exposure correction which is a parameter for simulate RX*/
	void SetExposureCorrection(double value[2]);
        /** set gamma correction*/
	void SetGamma(double value);
        /** set view angle */
	void SetCameraAngle(double value);
        /** set position of the camera*/
	void SetCameraPosition(double value[3]);
        /** set focus point of the camera*/
	void SetCameraFocus(double value[3]);
        /** set roll angle of the camera */
	void SetCameraRoll(double value);
        /** set resample factor of the rendered volume*/
	void SetResampleFactor(double value);
        /** get the resample factor of the rendered volume*/
	double GetResampleFactor();

protected:
  /** create the gui widget for the pipe*/
  virtual albaGUI  *CreateGui();

  vtkLookupTable              *m_ColorLUT;
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkXRayVolumeMapper         *m_VolumeMapper;
	vtkVolume                   *m_Volume;
	vtkImageResample						*m_ResampleFilter;
	
  vtkActor  *m_SelectionActor;
  double  m_VolumeBounds[6];
  double  m_VolumeOrientation[3];
  double  m_VolumePosition[3];
	double  m_ResampleFactor;

	//----------------------------- volume settings
	double             m_ExposureCorrection[2];
	albaGUIFloatSlider    *m_ExposureCorrectionSlider[2];
	double             m_Gamma;
	albaGUIFloatSlider    *m_GammaSlider;
	wxColor            m_VolumeColor;

	//----------------------------- image settings
	wxColor            m_ImageColor;
	double             m_ImageAngle;
	double             m_Offset[2];

	//----------------------------- camera settings
	double             m_CameraAngle;
	double             m_CameraPosition[3];
	double             m_CameraFocus[3];
	double             m_CameraRoll;

};
#endif // __albaPipeVolumeDRR_H__
