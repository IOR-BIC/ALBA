/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewCompound.h,v $
  Language:  C++
  Date:      $Date: 2008-02-19 09:52:57 $
  Version:   $Revision: 1.25 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafViewCompound_H__
#define __mafViewCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmdMouse;
class mafSceneGraph;
class mmgGui;
class mafRWIBase;

//----------------------------------------------------------------------------
// mafViewCompound :
//----------------------------------------------------------------------------
/** 
mafViewCompound is the base class for Compound Views in MAF.

The member function GetNodeStatus(vme) will be called by the mmgCheckTree 
in order to update it's icons. (Note that mmgCheckTree doesn't know about
the SceneGraph anymore)

mafViewCompound doesn't have a Scenegraph, nor knowledge about VTK /sa mafViewVTK.
*/
class mafViewCompound: public mafView
{
public:
  mafViewCompound(const wxString &label = "View Compound", int num_row = 1, int num_col = 2);
  virtual ~mafViewCompound(); 

  mafTypeMacro(mafViewCompound, mafView);

  enum LAYOUT_CONFIGURATION_ID
  {
    GRID_LAYOUT = 0,
    LAYOUT_1,
    LAYOUT_2,
    LAYOUT_3,
    LAYOUT_CUSTOM
  };

  /** 
  Plug the child view into the compound view. If the number of child view is less then row x col, 
  the last plugged view will fill the remaining holes.*/
  virtual void PlugChildView(mafView *child);

  /** 
  Redefine this method to package the compounded view */
  virtual void PackageView() {};

  virtual mafView *Copy(mafObserver *Listener);
  
  /** 
  Create the plugged sub-view and call virtual method CreateGuiView() */
  virtual void Create();
  
  /** 
  Create the GUI on the bottom of the compounded view. */
  virtual void CreateGuiView();
  
  virtual void OnEvent(mafEventBase *maf_event);

  /** 
  Add VME into plugged sub-views*/
  virtual void VmeAdd(mafNode *node);

  /** 
  Remove VME into plugged sub-views*/
  virtual void VmeRemove(mafNode *node);
  virtual void VmeSelect(mafNode *node, bool select);
  /** 
  Show/Hide VMEs into plugged sub-views*/
  virtual void VmeShow(mafNode *node, bool show);
	virtual void VmeUpdateProperty(mafNode *node, bool fromTag = false)		{};

  virtual void CameraReset(mafNode *node = NULL);
  virtual void CameraUpdate();

  /** 
  return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... 
  having mafViewCompound::GetNodeStatus allow mmgCheckTree to not know about mafSceneGraph */
  virtual int  GetNodeStatus(mafNode *vme);

  virtual void OnSize(wxSizeEvent &event);

  /** IDs for the GUI */
  enum VIEWCOMPOUND_WIDGET_ID
  {
    ID_DEFAULT_CHILD_VIEW = Superclass::ID_LAST,
    ID_LAYOUT_CHOOSER,
    ID_LINK_SUBVIEW,
    ID_LAST
  };

  /** 
  Set the size of the windowing double slider according to the size of the view. */
  virtual void OnLayout();

  virtual void SetMouse(mmdMouse *mouse);

  virtual mafSceneGraph *GetSceneGraph();
  virtual mafRWIBase    *GetRWI();

  /** 
  Return a pointer to the image of all sub-views.*/
  void GetImage(wxBitmap &bmp, int magnification = 1);

  /** 
  Find the pocked VME at button down. As argument the function needs
  the device which performed the action, and provides as result pointers
  to piked prop, vme and its behavior if it exists. */
  virtual bool FindPokedVme(mafDevice *device,mafMatrix &point_pose,vtkProp3D *&picked_prop,mafVME *&picked_vme,mafInteractor *&picked_behavior);

  /** 
  Perform a picking according to the screen position (x,y) and return true on success*/
  virtual bool Pick(int x, int y);

  /** 
  Perform a picking according to the absolute matrix given and return true on success*/
  virtual bool Pick(mafMatrix &m);

  /** 
  Write into the double array the position picked during Pick method.*/
  virtual void GetPickedPosition(double pos[3]);

  /** 
  Return the picked VME during the Pick method. Return NULL if VME is not found*/
  virtual mafVME *GetPickedVme();

  /** 
  Return the current pipe for the specified vme (if any exist at this moment) */
//  virtual mafPipe*  GetNodePipe(mafNode *vme);

  /** 
  Return the GUI for the visual pipe corresponding to the default child view. */
//  virtual mmgGui *GetNodePipeGUI(mafNode *vme);

  /** 
  Return the gui for the visual pipe corresponding to the n-th child view. */
//  virtual mmgGui *GetNodePipeGUI(mafNode *vme, int view_idx);

  /** 
  Return the sub-view in which the mouse is interacting with.*/
  mafView *GetSubView();

  /** 
  Return the sub-view with index idx.*/
  mafView *GetSubView(int idx);

  /** 
  Return the sub-view num index which the mouse is interacting with.*/
  int GetSubViewIndex();

  /** 
  Return the number of child view.*/
  int GetNumberOfSubView() {return m_NumOfChildView;};

  /** 
  Let the sub-view to be maximized inside the compounded view. */
  void MaximizeSubView(int subview_id = 0, bool maximize = true);

  /** 
  Return true is one of the plugged view is maximized.*/
  bool IsSubViewMaximized() {return m_SubViewMaximized != -1;};

  /** 
  Print the sub-view.*/
  virtual void Print(wxDC *dc, wxRect margins);

  /** Allow to link the camera of the sub-views.*/
  virtual void LinkView(bool link_camera = true);

  /** Called to update all components that depends on Application Options.*/
  virtual void OptionsUpdate();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same panel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mmgGui  *CreateGui();

  /** Arrange sub-view into the defined layout configuration.*/
  void LayoutSubView(int width, int height);

  /** 
  Redefine this method to define a custom layout.*/
  virtual void LayoutSubViewCustom(int width, int height) {};

  int m_ViewRowNum; ///< number of rows to divide the compound view
  int m_ViewColNum; ///< number of cols to divide the compound view
  int m_NumOfPluggedChildren; ///< number of plugged children view
  int m_NumOfChildView; ///< number of child view (is equal or greater then m_NumOfPluggedChildren)

  int m_LayoutConfiguration; ///< Arrange the subviews on different layout configuration.
  int m_DefauldChildView;
  
  int m_SubViewMaximized;  ///< contain the id of the sub-view to be maximized. -1 means that all sub-views are in normal size
  int m_LinkSubView;

  wxSize  m_Size; ///< size of the compound view
  mmgGui *m_GuiView;
  wxWindow *m_GuiViewWindow;
  mmdMouse *m_Mouse;


  std::vector<mafView *> m_ChildViewList; ///< Child views vector
  std::vector<mafView *> m_PluggedChildViewList; ///< Plugged Child views vector
};
#endif
