/*=========================================================================

 Program: MAF2
 Module: mafViewSingleSlice
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafViewSingleSlice_H__
#define __mafViewSingleSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafViewVTK.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class vtkActor2D;
class vtkTextMapper;
class mafGUIFloatSlider;

//----------------------------------------------------------------------------
// mafViewSingleSlice :
//----------------------------------------------------------------------------
/** 
mafViewSingleSlice is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class MAF_EXPORT mafViewSingleSlice: public mafViewVTK
{
public:
  mafViewSingleSlice(wxString label = "Slice", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, int stereo = 0, bool show_orientation = false);
  virtual ~mafViewSingleSlice(); 

  mafTypeMacro(mafViewSingleSlice, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener, bool lightCopyEnabled = false);
  virtual void			OnEvent(mafEventBase *maf_event);

  void Create();

  /** 
  Set the visualization status for the vme (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
  virtual int GetNodeStatus(mafVME *vme);

  /** 
  Set tolerance of the Picker*/
  void SetPickerTolerance(double tolerance);


  /** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_NONE = Superclass::ID_LAST,
		ID_POSITION,
		ID_PLANE_SELECT,
    ID_LAST
  };

	enum VIEW_SELECT_PLANE
	{
		XY,
		YZ,
		ZX,
	};

  /** 
  Create the visual pipe for the vme passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafVME *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafVME *vme);

  /** 
  Change the range of the WindowLevel Lookup table.*/
  void SetLutRange(double low_val, double high_val);

  /** 
  Set the slice coordinates.*/
  virtual void SetSlice(double origin[3]);

  /** Get the Slice coordinates.*/
  void GetSlice(double slice[3]);

  /** Get the Slice coordinates.*/
  double *GetSlice();

  /** 
  Return true if a there is a volume inside the view.*/
  bool VolumeIsVisible() {return m_CurrentVolume != NULL;};

  /** Create the background for the slice. */
  void BorderCreate(double col[3]);

  /** Delete the background of the slice. */
  void BorderDelete();

  /** 
  Give an initial origin for the slice.*/
  void InitializeSlice(double slice[3]);

  /** Set color of the text in CT View**/
  void SetTextColor(double color[3]);

  /** Refresh text in CT View**/
  void UpdateText(int ID = 1);

  /** Set Size of the text*/
  void SetTextSize(int size);
  
  /** Update list of surfaces vme */
  void UpdateSurfacesList(mafVME *vme);

  void VmeShow(mafVME *vme, bool show);

protected:
  virtual mafGUI *CreateGui();
  
  //mafGUIFloatSlider  *m_Slider;
  mafSceneNode    *m_CurrentVolume; ///< Current visualized volume
  vtkActor2D      *m_Border;
  double           m_Slice[3];
  bool             m_SliceInitialized; 

  // text stuff
  vtkActor2D    *m_TextActor;
  vtkTextMapper	*m_TextMapper;
  wxString m_Text;
  double	 m_TextColor[3];
  std::vector<mafSceneNode*> m_CurrentSurface;
	std::vector<mafSceneNode*> m_CurrentPolyline;

	double m_Position;
	double m_OriginVolume[3];
	int    m_PlaneSelect;
	

};
#endif
