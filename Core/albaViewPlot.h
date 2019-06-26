/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaViewPlot
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaViewPlot_H__
#define __albaViewPlot_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaView.h"
#include "albaSceneGraph.h"
#include "albaSceneNode.h" //used in subclasses
#include <map>

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaDeviceButtonsPadMouse;

/** Struct containing information regarding visual pipe plugged into the view. */
struct albaVisualPipeInfo
{
  albaString m_PipeName;
  long      m_Visibility;
};

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_STL_MAP(ALBA_EXPORT,albaString,albaVisualPipeInfo);
#endif

//----------------------------------------------------------------------------
// albaViewPlot :
//----------------------------------------------------------------------------
/** 
albaViewPlot is a View that got a albaXYPlot and plot curves on a plane.
*/
class ALBA_EXPORT albaViewPlot: public albaView
{
public:
  albaViewPlot(const wxString &label = "plot View");
  virtual ~albaViewPlot();

  albaTypeMacro(albaViewPlot, albaView);

  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false);
  virtual void      Create();
  virtual void			OnEvent(albaEventBase *alba_event);

  /** IDs for the GUI */
/*  enum VIEW_VTK_WIDGET_ID
  {
    ID_SHOW_GRID = Superclass::ID_LAST,
    ID_GRID_NORMAL,
    ID_LAST
  };*/

  /** Add the vme to the view's scene-graph*/
  virtual void VmeAdd(albaVME *vme);
  
  /** Remove the vme from the view's scene-graph*/
  virtual void VmeRemove(albaVME *vme);

  virtual void VmeSelect(albaVME *vme, bool select);

  /** Called to show/hide vme*/
  virtual void VmeShow(albaVME *vme, bool show);

  /** 
  Called to update visual pipe properties of the vme passed as argument. If the 'fromTag' flag is true,
  the update is done by reading the visual parameters from tags.*/
  virtual void VmeUpdateProperty(albaVME *vme, bool fromTag = false);

  /** 
  Set the visualization status for the node (visible, not visible, mutex, ...) \sa albaSceneGraph*/
  virtual int GetNodeStatus(albaVME *vme);

  /** 
  Return a pointer to the visual pipe of the node passed as argument. 
  It is used in albaSideBar to plug the visual pipe's GUI in the tabbed vme panel. \sa albaSideBar*/
  virtual albaPipe*  GetNodePipe(albaVME *vme);
  
  /** 
  Create the visual pipe for the node passed as argument. 
  To create visual pipe first check in m_PipeMap if custom visual pipe is defined, 
  otherwise ask to vme which is its visual pipe. */
  virtual void VmeCreatePipe(albaVME *vme);

  /** 
  Delete vme's visual pipe. It is called when vme is removed from visualization.*/
  virtual void VmeDeletePipe(albaVME *vme);

  virtual albaSceneGraph *GetSceneGraph()	  {return m_Sg;}; 

	virtual void SetBackgroundColor(wxColor color);

	typedef std::map<albaString, albaVisualPipeInfo> albaPipeMap;

  /** Plug a visual pipe for a particular vme. It is used also to plug custom pipe.*/
  void PlugVisualPipe(albaString vme_type, albaString pipe_type, long visibility = VISIBLE);

  albaPipeMap m_PipeMap; ///< Map used to store visual pipeline associated with vme types

  /** Set the mouse device to use inside the view */
  void SetMouse(albaDeviceButtonsPadMouse *mouse);

protected:
  albaSceneGraph *m_Sg;

  virtual albaGUI *CreateGui();

  /** Return the visual pipe's name.*/
  void GetVisualPipeName(albaVME *vme, albaString &pipe_name);
};
#endif
