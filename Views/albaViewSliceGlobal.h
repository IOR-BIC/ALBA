/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewSliceGlobal
 Authors: Eleonora Mambrini
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaViewSliceGlobal_H__
#define __albaViewSliceGlobal_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewSlice.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaGUIFloatSlider;

//----------------------------------------------------------------------------
// albaViewSliceGlobal :
//----------------------------------------------------------------------------
/** 
albaViewSliceGlobal is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor.
This is an experimental component with rotated volumes interaction
and visualization enabled.
*/

class ALBA_EXPORT albaViewSliceGlobal: public albaViewSlice
{
public:
  albaViewSliceGlobal(wxString label = "Slice", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, int stereo = 0,bool showTICKs=false,bool textureInterpolate=true);
  virtual ~albaViewSliceGlobal(); 

  albaTypeMacro(albaViewSliceGlobal, albaViewSlice);

  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  virtual void			OnEvent(albaEventBase *alba_event);

  //void Create();

  /** 
  Set the visualization status for the vme (visible, not visible, mutex, ...) \sa albaSceneGraph albaView*/
  virtual int GetNodeStatus(albaVME *vme);

  /** IDs for the view GUI */
  enum VIEW_NEW_GLOBAL_SLICE_WIDGET_ID
  {
    ID_POS_SLIDER = Superclass::ID_LAST,
    ID_CHANGE_VIEW,
    ID_OPACITY_SLIDER,
    ID_SHOW_TEXT,
    ID_TEXT_MODE,
    ID_LAST
  };

  /** 
  Create the visual pipe for the vme passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(albaVME *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(albaVME *vme);

  /** Set slicer parameter to generate the slice. */
  //virtual void SetSlice(double origin[3], float xVect[3], float yVect[3]);
	 
  /** Calculate for every actor the correct slice. */
  virtual void UpdateSlice();

  /** Update slice position along current axis. */
  double UpdateSlicePos(double pos = -1);

  /** Refresh text in CT View**/
  virtual void UpdateText(int ID = 1);

  /** Public method to be called to change slice axis. */
  void ChangeView(int viewIndex);

  /** Show on/off slice and frame numbers. */
  virtual void ShowText(bool show);

  /**  Change text mode. */
  virtual void SetTextMode(int textMode);

  /** Function called when select a vme different from selected one.*/
  virtual void VmeSelect(albaVME *vme, bool select);

  /** Override superclass. */
  virtual void VmeShow(albaVME *vme, bool show);

  /** pPrint a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** Override superclass. */
  virtual void CameraUpdate();

protected:

  /** Override superclass. */
  virtual albaGUI *CreateGui();

  /** Internally called to update information about current slice number.*/
  void UpdateSliceIndex();

  /** Force the updating of several slice parameters like origin, vectors and gui components. */
  virtual void UpdateSliceParameters();

  /** Internally called to update information about current volume data bounds..*/
  virtual void UpdateBounds();

  double		m_GlobalBounds[6];
  double		m_SliderOldOrigin;
  double		m_SliderOrigin;
  double		m_Opacity;
  //int				m_SliceMode;
  int				m_ViewIndex;
  double		m_Dn;

  albaGUIFloatSlider	*m_GlobalSlider;
  albaGUIFloatSlider	*m_OpacitySlider;

  bool			m_GlobalBoundsInitialized;
  bool      m_GlobalBoundsValid;

  int m_NumberOfSlices[3];
  int m_SliceIndex;
  albaTimeStamp m_CurrentTimeStamp;
  int m_NumberOfFrames;
  int m_FrameIndex;
  int m_ShowText;
  int m_TextMode;

  bool m_TextureInterpolate;

};
#endif
