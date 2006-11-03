/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeDRR.h,v $
  Language:  C++
  Date:      $Date: 2006-11-03 14:17:47 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani - porting Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeVolumeDRR_H__
#define __medPipeVolumeDRR_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
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
class mmgFloatSlider;

//----------------------------------------------------------------------------
// medPipeVolumeDRR :
//----------------------------------------------------------------------------
class medPipeVolumeDRR : public mafPipe
{
public:
  mafTypeMacro(medPipeVolumeDRR,mafPipe);

           medPipeVolumeDRR();
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
		ID_LAST
  };

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

protected:
  virtual mmgGui  *CreateGui();

  vtkLookupTable              *m_ColorLUT;
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkXRayVolumeMapper         *m_VolumeMapper;
	vtkVolume                    *m_Volume;
	
  vtkActor  *m_SelectionActor;
  double  m_VolumeBounds[6];
  double  m_VolumeOrientation[3];
  double  m_VolumePosition[3];

	//----------------------------- volume settings
	double             m_ExposureCorrection[2];
	mmgFloatSlider    *m_ExposureCorrectionSlider[2];
	double             m_Gamma;
	mmgFloatSlider    *m_GammaSlider;
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
