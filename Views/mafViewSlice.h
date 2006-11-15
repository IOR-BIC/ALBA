/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewSlice.h,v $
  Language:  C++
  Date:      $Date: 2006-11-15 18:17:33 $
  Version:   $Revision: 1.18 $
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
  mafViewSlice(wxString label = "Slice", int camera_position = CAMERA_CT, bool show_axes = false, bool show_grid = false, bool show_ruler = false, int stereo = 0);
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

  /** 
  Set the slice coordinates.*/
  virtual void SetSliceLocalOrigin(double origin[3]);

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
  
  /** Update list of surfaces vme */
  void UpdateSurfacesList(mafNode *node);

  void VmeShow(mafNode *node, bool show);

  void VmeRemove(mafNode *vme);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);// const;

protected:
  virtual mmgGui *CreateGui();

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

};
#endif
