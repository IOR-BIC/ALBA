/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaView3D
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaView3D_H__
#define __albaView3D_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewVTK.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaSceneNode;
class albaGUI;
class albaVMEVolumeGray;
class albaVMESurface;
class albaGUIFloatSlider;

//----------------------------------------------------------------------------
// albaViewRX :
//----------------------------------------------------------------------------
/** 
albaViewRX is a View that visualize volume as projection along x or y axis and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa albaViewVTK
*/
class ALBA_EXPORT albaView3D: public albaViewVTK
{
public:
  albaView3D(wxString label = "3D", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = false, bool show_grid = false, int stereo = 0);
  virtual ~albaView3D(); 

  albaTypeMacro(albaView3D, albaViewVTK);

  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false);
	virtual void			OnEvent(albaEventBase *alba_event);

	void Create();


  /** IDs for the GUI */
  enum VIEW_RX_WIDGET_ID
  { 
    ID_NONE = Superclass::ID_LAST,
		ID_COMBO_PIPE,
		ID_RESAMPLE_FACTOR,
		ID_CONTOUR_VALUE_ISO,
		ID_ALPHA_VALUE_ISO,
		ID_EXTRACT_ISO,
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
    ID_LAST
  };

	/** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(albaVME *vme);

	/** Show/Hide VMEs into plugged sub-views */
	virtual void VmeShow(albaVME *vme, bool show);

	/** 
	Set the visualization status for the node (visible, not visible, mutex, ...) \sa albaSceneGraph albaView*/
	virtual int GetNodeStatus(albaVME *vme);

  double GetContourValue(){return m_ContourValueIso;};
  void SetContourValue(double value){m_ContourValueIso = value; OnEvent(&albaEvent(this,ID_CONTOUR_VALUE_ISO));};

  double GetAlphaValue(){return m_AlphaValueIso;};
  void SetAlphaValue(double value){m_AlphaValueIso = value;OnEvent(&albaEvent(this,ID_ALPHA_VALUE_ISO));};

protected:

	virtual albaGUI *CreateGui();

	void EnableSubGui(int idSubPipe,bool enable=true);

	void InizializeSubGui();

	int m_Choose;

	albaVME          	*m_CurrentVolume;
	albaVMESurface			*m_CurrentSurface;
	albaVMESurface			*m_CurrentInvisibleSurface;

	double m_ResampleFactor;

	//----------------------------- ISO settings
	double m_ContourValueIso;
	double m_AlphaValueIso;
	albaGUIFloatSlider *m_SliderContourIso;
	albaGUIFloatSlider *m_SliderAlphaIso;

	//----------------------------- DRR settings
	double             m_ExposureCorrection[2];
	albaGUIFloatSlider    *m_ExposureCorrectionSlider[2];
	double             m_Gamma;
	albaGUIFloatSlider    *m_GammaSlider;
	wxColor            m_VolumeColor;
	wxColor            m_ImageColor;
	double             m_ImageAngle;
	double             m_Offset[2];
	double             m_CameraAngle;
	double             m_CameraPositionDRR[3];
	double             m_CameraFocus[3];
	double             m_CameraRoll;



};
#endif
