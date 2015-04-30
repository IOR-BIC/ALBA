/*=========================================================================

 Program: MAF2
 Module: mafView3D
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafView3D_H__
#define __mafView3D_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewVTK.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class mafGUI;
class mafVMEVolumeGray;
class mafVMESurface;
class mafGUIFloatSlider;

//----------------------------------------------------------------------------
// mafViewRX :
//----------------------------------------------------------------------------
/** 
mafViewRX is a View that visualize volume as projection along x or y axis and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class MAF_EXPORT mafView3D: public mafViewVTK
{
public:
  mafView3D(wxString label = "3D", int camera_position = CAMERA_PERSPECTIVE, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
  virtual ~mafView3D(); 

  mafTypeMacro(mafView3D, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);
	virtual void			OnEvent(mafEventBase *maf_event);

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
  virtual void VmeCreatePipe(mafNode *vme);

	/** Show/Hide VMEs into plugged sub-views */
	virtual void VmeShow(mafNode *node, bool show);

	/** 
	Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
	virtual int GetNodeStatus(mafNode *vme);

  double GetContourValue(){return m_ContourValueIso;};
  void SetContourValue(double value){m_ContourValueIso = value; OnEvent(&mafEvent(this,ID_CONTOUR_VALUE_ISO));};

  double GetAlphaValue(){return m_AlphaValueIso;};
  void SetAlphaValue(double value){m_AlphaValueIso = value;OnEvent(&mafEvent(this,ID_ALPHA_VALUE_ISO));};

protected:

	virtual mafGUI *CreateGui();

	void EnableSubGui(int idSubPipe,bool enable=true);

	void InizializeSubGui();

	int m_Choose;

	mafVME          	*m_CurrentVolume;
	mafVMESurface			*m_CurrentSurface;
	mafVMESurface			*m_CurrentInvisibleSurface;

	double m_ResampleFactor;

	//----------------------------- ISO settings
	double m_ContourValueIso;
	double m_AlphaValueIso;
	mafGUIFloatSlider *m_SliderContourIso;
	mafGUIFloatSlider *m_SliderAlphaIso;

	//----------------------------- DRR settings
	double             m_ExposureCorrection[2];
	mafGUIFloatSlider    *m_ExposureCorrectionSlider[2];
	double             m_Gamma;
	mafGUIFloatSlider    *m_GammaSlider;
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
