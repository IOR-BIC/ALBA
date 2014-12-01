/*=========================================================================

 Program: MAF2
 Module: medPipeVolumeDRR
 Authors: Paolo Quadrani - porting Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medPipeVolumeDRR_H__
#define __medPipeVolumeDRR_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafPipe.h"

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
class mafGUIFloatSlider;
/**
className: medPipeVolumeDRR
Pipe which uses BES vtkXRayVolumeMapper.
*/
class MAF_EXPORT medPipeVolumeDRR : public mafPipe
{
public:
  /** RTTI macro*/
  mafTypeMacro(medPipeVolumeDRR,mafPipe);
  /** constructor*/
           medPipeVolumeDRR();
  /** destructor */
  virtual ~medPipeVolumeDRR();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

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
		ID_IMAGE_COLOR,
		ID_IMAGE_OFFSET_X,
		ID_IMAGE_OFFSET_Y,
		ID_IMAGE_ANGLE,
		ID_EXPORT,
		ID_RESAMPLE_FACTOR,
		ID_LAST
  };
  /** create pipe*/
  virtual void Create(mafSceneNode *n);
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
  virtual mafGUI  *CreateGui();

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
	mafGUIFloatSlider    *m_ExposureCorrectionSlider[2];
	double             m_Gamma;
	mafGUIFloatSlider    *m_GammaSlider;
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
#endif // __medPipeVolumeDRR_H__
