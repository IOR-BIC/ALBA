/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSlice.h,v $
  Language:  C++
  Date:      $Date: 2009-12-02 09:18:45 $
  Version:   $Revision: 1.26.2.4 $
  Authors:   Paolo Quadrani,Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafViewSlice_H__
#define __mafViewSlice_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafViewVTK.h"
#include <vector>

#if defined(DEPRECATE_SUPPORTED) && _MSC_VER >= 1400
#define DECLSPEC_DEPRECATED_S(x) __declspec(deprecated(x))
#else
#define DECLSPEC_DEPRECATED_S(x)
#pragma deprecated(SetSliceLocalOrigin)
#pragma deprecated(SetNormal)
#endif

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mafSceneNode;
class vtkActor2D;
class vtkTextMapper;

//----------------------------------------------------------------------------
// mafViewSlice :
//----------------------------------------------------------------------------
/** 
mafViewSlice is a View that visualize volume as slices and 
initialize the visual pipe according to the camera position that is passed through constructor
\sa mafViewVTK
*/
class mafViewSlice: public mafViewVTK
{
public:
  mafViewSlice(wxString label = "Slice", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0,bool showTICKs=false);
  virtual ~mafViewSlice(); 

  mafTypeMacro(mafViewSlice, mafViewVTK);

  virtual mafView*  Copy(mafObserver *Listener);
  virtual void			OnEvent(mafEventBase *maf_event);

  void Create();

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph mafView*/
  virtual int GetNodeStatus(mafNode *vme);

  /** IDs for the view GUI */
  enum VIEW_SLICE_WIDGET_ID
  {
    ID_NONE = Superclass::ID_LAST,
    ID_ENABLE_GPU,
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

  /** 
  Change the range of the WindowLevel Lookup table.*/
  void SetLutRange(double low_val, double high_val);


  /** Set the slice coordinates. 
  DEPRECATED - use SetOrigin or SetSlice */
  inline virtual DECLSPEC_DEPRECATED_S("SetSliceLocalOrigin(double origin[3]) is deprecated, use SetSlice or SetSliceOrigin") 
    void SetSliceLocalOrigin(double origin[3]) {
      return SetSlice(origin, NULL);
  }

  /** Set the origin of the slice */
  inline void SetSliceOrigin(double *Origin) {
    SetSlice(Origin, NULL);
  }

  /** gets the slice origin coordinates */
  inline double* GetSliceOrigin() {
    return m_Slice;
  }

  /** Set the slice coordinates. 
  DEPRECATED - use SetOrigin or SetSlice */
  inline virtual DECLSPEC_DEPRECATED_S("SetNormal(double normal[3]) is deprecated, use SetSlice or SetSliceNormal") 
    void SetNormal(double normal[3]) {
      return SetSlice(NULL, normal);
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
  virtual void SetSlice(double* Origin, double* Normal = NULL);

  /** Get the Slice coordinates.
  DEPRECATED - use GetOrigin or GetSlice*/
  inline DECLSPEC_DEPRECATED_S("double *GetSlice() is deprecated, use GetSliceOrigin") 
    double *GetSlice() {
    return GetSliceOrigin();
  }

  /** Get the slice origin coordinates and normal.*/
  void GetSlice(double* Origin, double* Normal = NULL);


  /** 
  Return true if a there is a volume inside the view.*/
  bool VolumeIsVisible() {return m_CurrentVolume != NULL;};

  /** Create the background for the slice. */
  void BorderCreate(double col[3]);

  /** Delete the background of the slice. */
  void BorderDelete();

  /** 
  Give an initial origin and normal (optionally) for the slice.*/
  void InitializeSlice(double* Origin, double* Normal = NULL);

  /** Set color of the text in CT View**/
  void SetTextColor(double color[3]);

  /** Refresh text in CT View**/
  void UpdateText(int ID = 1);
  
  /** Update list of surfaces vme */
  void UpdateSurfacesList(mafNode *node);

  void VmeShow(mafNode *node, bool show);

  void VmeRemove(mafNode *vme);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /* Function for the correction of slice pose in case of currentvolume pose matrix is different from identity */
  void VolumePositionCorrection(double *point);

  /* Update the visualization of the border */
  void BorderUpdate();
  
  /* Set the opacity of the border if exist */
  void SetBorderOpacity(double value);

protected:
  virtual mafGUI *CreateGui();

  mafSceneNode    *m_CurrentVolume; ///< Current visualized volume
  vtkActor2D      *m_Border;
  double           m_Slice[3];        //<Slice origin
  double           m_SliceNormal[3];  //<Slice normal
  bool             m_SliceInitialized; 

	bool						 m_ShowVolumeTICKs;
  double           m_BorderColor[3];
  // text stuff
  vtkActor2D    *m_TextActor;
  vtkTextMapper	*m_TextMapper;
  wxString m_Text;
  double	 m_TextColor[3];
  std::vector<mafSceneNode*> m_CurrentSurface;
	std::vector<mafSceneNode*> m_CurrentPolyline;
	std::vector<mafSceneNode*> m_CurrentPolylineGraphEditor;
  std::vector<mafSceneNode*> m_CurrentMesh;

  // Added by Losi 11.25.2009
  int m_EnableGPU; ///<Non-zero, if the GPU support for slicing is used (default)
};
#endif
