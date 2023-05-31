/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewSlice
 Authors: Gianluigi Crimi, Paolo Quadrani,Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaViewSlice_H__
#define __albaViewSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaViewVTK.h"
#include "albaMatrix.h"
#include <vector>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaSceneNode;

class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// albaViewSlice :
//----------------------------------------------------------------------------
/** 
albaViewSlice is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor.
Rotated volumes interaction and visualizationo is supported by this component.
*/

class ALBA_EXPORT albaViewSlice: public albaViewVTK
{
public:
  albaViewSlice(wxString label = "Slice", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, int stereo = 0,bool showTICKs=false,bool textureInterpolate=true);
  virtual ~albaViewSlice(); 

  albaTypeMacro(albaViewSlice, albaViewVTK);

  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  virtual void			OnEvent(albaEventBase *alba_event);

    /** Set the origin of the slice */
  inline void SetSliceOrigin(double *Origin) {
    SetSlice(Origin, NULL);
  }

  /** gets the slice origin coordinates */
  inline double* GetSliceOrigin() {
    return m_Slice;
  }

  /** Set the normal of the slice*/
  void SetSliceNormal(double *Normal) {
    SetSlice(NULL, Normal);
  }

  /** gets the slice normal */
  inline double* GetSliceNormal() {
    return m_SliceNormal;
  }

  /** Set the origin and normal of the slice 
  Both, Origin and Normal may be NULL, if the current value is to be preserved */
	virtual void SetSlice(double* Origin = NULL, double* Normal = NULL);

  /** Get the slice origin coordinates and normal.*/
  void GetSlice(double* Origin, double* Normal = NULL);

  void Create();

  /** 
  Set the visualization status for the vme (visible, not visible, mutex, ...) \sa albaSceneGraph albaView*/
  virtual int GetNodeStatus(albaVME *vme);

  /** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_NONE = Superclass::ID_LAST,
    ID_TRILINEAR_INTERPOLATION,
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

  /** 
  Change the range of the WindowLevel Lookup table.*/
  void SetLutRange(double low_val, double high_val);

  /** 
  Return true if a there is a volume inside the view.*/
  bool VolumeIsVisible() {return m_CurrentVolume != NULL;};

  /** Create the background for the slice. */
  void BorderCreate(double col[3]);

  /** Delete the background of the slice. */
  void BorderDelete();
	  
  /** Set color of the text in CT View**/
  void SetTextColor(double color[3]);

  /** Refresh text in CT View**/
  virtual void UpdateText(int ID = 1);
  
  /** Update list of surfaces vme */
  void RemoveFromSlicingList(albaVME *vme);

  virtual void VmeShow(albaVME *vme, bool show);

  void VmeRemove(albaVME *vme);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /* Update the visualization of the border */
  void BorderUpdate();
  
  /* Set the opacity of the border if exist */
  void SetBorderOpacity(double value);

  virtual void CameraUpdate();

  void SetTrilinearInterpolation(int interpolation){m_TrilinearInterpolationOn = interpolation;};

  void SetTextureInterpolate(bool interpolate) {m_TextureInterpolate = interpolate;};

	virtual void SetSliceAxis(int sliceAxis);

	/**
	Give an initial origin and normal (optionally) for the slice.*/
	void InitializeSlice(double* Origin);

protected:
	
  /* Correction of slice pose when current volume pose matrix is different from identity: 
  the multiplication is performed inplace */
  void MultiplyPointByInputVolumeABSMatrix(double *point);

  virtual albaGUI *CreateGui();

  void CameraUpdateForRotatedVolumes();
  void SetCameraParallelToDataSetLocalAxis( int axis );


	//----------------------------------------------------------------------------
	virtual void GetVisualPipeName(albaVME *node, albaString &pipe_name);
 
  albaSceneNode    *m_CurrentVolume; ///< Current visualized volume
  vtkActor2D      *m_Border;
  double           m_Slice[3];        //<Slice origin
  double           m_SliceNormal[3];  //<Slice normal
  bool             m_SliceInitialized; 

	bool						 m_ShowVolumeTICKs;
  double           m_BorderColor[3];

  vtkActor2D    *m_TextActor;
  vtkTextMapper	*m_TextMapper;
  wxString m_Text;
  double	 m_TextColor[3];
  std::vector<albaSceneNode*> m_SlicingVector;
  
  albaMatrix m_OldABSPose;
  albaMatrix m_NewABSPose;

	bool m_TextureInterpolate;
  int m_TrilinearInterpolationOn;
};
#endif
