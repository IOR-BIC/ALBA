/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewPlot.h,v $
  Language:  C++
  Date:      $Date: 2007-03-15 14:22:00 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafViewPlot_H__
#define __mafViewPlot_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafView.h"
#include "mafSceneGraph.h"
#include "mafSceneNode.h" //used in subclasses
#include <map>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVME;
class mmdMouse;

//----------------------------------------------------------------------------
// mafViewPlot :
//----------------------------------------------------------------------------
/** 
mafViewPlot is a View that got a mafXYPlot and plot curves on a plane.
*/
class mafViewPlot: public mafView
{
public:
  mafViewPlot(const wxString &label = "plot View");
  virtual ~mafViewPlot();

  mafTypeMacro(mafViewPlot, mafView);

  virtual mafView*  Copy(mafObserver *Listener);
  virtual void      Create();
  virtual void			OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
/*  enum VIEW_VTK_WIDGET_ID
  {
    ID_SHOW_GRID = Superclass::ID_LAST,
    ID_GRID_NORMAL,
    ID_LAST
  };*/

  /** Add the vme to the view's scene-graph*/
  virtual void VmeAdd(mafNode *vme);
  
  /** Remove the vme from the view's scene-graph*/
  virtual void VmeRemove(mafNode *vme);

  virtual void VmeSelect(mafNode *vme, bool select);

  /** Called to show/hide vme*/
  virtual void VmeShow(mafNode *vme, bool show);

  /** 
  Called to update visual pipe properties of the vme passed as argument. If the 'fromTag' flag is true,
  the update is done by reading the visual parameters from tags.*/
  virtual void VmeUpdateProperty(mafNode *vme, bool fromTag = false);

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa mafSceneGraph*/
  virtual int GetNodeStatus(mafNode *vme);

  /** 
  Return a pointer to the visual pipe of the node passed as argument. 
  It is used in mafSideBar to plug the visual pipe's GUI in the tabbed vme panel. \sa mafSideBar*/
  virtual mafPipe*  GetNodePipe(mafNode *vme);
  
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(mafNode *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(mafNode *vme);

  virtual mafSceneGraph *GetSceneGraph()	  {return m_Sg;}; 

  /** Return a pointer to the image of the plot.*/
  void GetImage(wxBitmap &bmp, int magnification = 1);

  /** Struct containing information regarding visual pipe plugged into the view. */
  struct mafVisualPipeInfo
  {
    mafString m_PipeName;
    long      m_Visibility;
  };
  typedef std::map<mafString, mafVisualPipeInfo> mafPipeMap;

  /** Plug a visual pipe for a particular vme. It is used also to plug custom pipe.*/
  void PlugVisualPipe(mafString vme_type, mafString pipe_type, long visibility = VISIBLE);

  mafPipeMap m_PipeMap; ///< Map used to store visual pipeline associated with vme types

  /** Set the mouse device to use inside the view */
  void SetMouse(mmdMouse *mouse);

  /** Print this view.*/
  virtual void Print(wxDC *dc, wxRect margins);

protected:
  mafSceneGraph *m_Sg;

  virtual mmgGui *CreateGui();

  /** Return the visual pipe's name.*/
  void GetVisualPipeName(mafNode *node, mafString &pipe_name);
};
#endif
