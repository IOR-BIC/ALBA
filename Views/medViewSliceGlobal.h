/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medViewSliceGlobal.h,v $
Language:  C++
Date:      $Date: 2010-11-22 11:04:58 $
Version:   $Revision: 1.1.2.4 $
Authors:   Eleonora Mambrini
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __medViewSliceGlobal_H__
#define __medViewSliceGlobal_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewSlice.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;

//----------------------------------------------------------------------------
// medViewSliceGlobal :
//----------------------------------------------------------------------------
/** 
medViewSliceGlobal is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor.
This is an experimental component with rotated volumes interaction
and visualization enabled.
*/

class medViewSliceGlobal: public mafViewSlice
{
public:
  medViewSliceGlobal(wxString label = "Slice", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0,bool showTICKs=false,bool textureInterpolate=true);
  virtual ~medViewSliceGlobal(); 

  mafTypeMacro(medViewSliceGlobal, mafViewSlice);

  virtual mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);
  virtual void			OnEvent(mafEventBase *maf_event);

  //void Create();

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
  virtual int GetNodeStatus(mafNode *vme);

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
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

  /** Set slicer parameter to generate the slice. */
  virtual void SetSlice(double origin[3], float xVect[3], float yVect[3]);

  /** Set the origin and normal of the slice 
  Both, Origin and Normal may be NULL, if the current value is to be preserved */
  virtual void SetSlice(double* Origin, double* Normal = NULL);

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
  virtual void VmeSelect(mafNode *node, bool select);

  /** Override superclass. */
  virtual void VmeShow(mafNode *node, bool show);

  /** pPrint a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** Override superclass. */
  virtual void CameraUpdate();

protected:

  /** Override superclass. */
  virtual mafGUI *CreateGui();

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

  mafGUIFloatSlider	*m_GlobalSlider;
  mafGUIFloatSlider	*m_OpacitySlider;

  bool			m_GlobalBoundsInitialized;
  bool      m_GlobalBoundsValid;

  int m_NumberOfSlices[3];
  int m_SliceIndex;
  mafTimeStamp m_CurrentTimeStamp;
  int m_NumberOfFrames;
  int m_FrameIndex;
  int m_ShowText;
  int m_TextMode;

  bool m_TextureInterpolate;

};
#endif
