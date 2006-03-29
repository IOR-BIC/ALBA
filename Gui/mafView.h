/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafView.h,v $
  Language:  C++
  Date:      $Date: 2006-03-29 13:44:33 $
  Version:   $Revision: 1.8 $
  Authors:   Silvano Imboden - Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/
#ifndef __mafView_H__
#define __mafView_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafObject.h"
#include "mafDecl.h"
#include "mafNode.h"
#include "mmgGuiHolder.h"
#include "mmgGui.h"
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mmdMouse;
class mafDevice;
class mafInteractor;

class mafRWIBase;
class vtkCellPicker;
class vtkRayCast3DPicker;
class vtkAssemblyPath;
class vtkProp3D;

//----------------------------------------------------------------------------
// const :
//----------------------------------------------------------------------------
/** status of a SceneNode -- do not change, it is used by mafCheckTree also */
enum NODE_STATUS_ID
{
  NODE_NON_VISIBLE =0,
  NODE_VISIBLE_OFF,
  NODE_VISIBLE_ON,
  NODE_MUTEX_OFF,
  NODE_MUTEX_ON,
};
//----------------------------------------------------------------------------
// mafView :
//----------------------------------------------------------------------------
/** 
mafView is the base class for Views in maf.

The member function GetNodeStatus(vme) will be called by the mmgCheckTree 
in order to update it's icons. (Note that mmgCheckTree doesn't know about
the SceneGraph anymore)

mafView doesn't have a Scenegraph, nor knowledge about VTK /sa mafViewVTK.

mafView can be the base class for composed Views.

*/
class mafView: public mafObserver, public mafObject
{
public:
  mafView(wxString label = "View");
  virtual ~mafView(); 

  mafTypeMacro(mafView, mafObject);

  virtual void			SetListener(mafObserver *Listener) {m_Listener = Listener;};
  virtual void			OnEvent(mafEventBase *maf_event);
  virtual mafView*  Copy(mafObserver *Listener) {return NULL;};
  virtual void      Create() {};

  virtual void			VmeAdd(mafNode *vme)																		{};
  virtual void			VmeRemove(mafNode *vme)																	{};
  virtual void			VmeSelect(mafNode *vme, bool select)										{};
  virtual void			VmeShow(mafNode *vme, bool show)												{};
	virtual void      VmeUpdateProperty(mafNode *vme, bool fromTag = false)		{};

  virtual void	    VmeCreatePipe(mafNode *vme)													    {};
  virtual void	    VmeDeletePipe(mafNode *vme)													    {};

  virtual void			CameraReset(mafNode *node = NULL)  											{};
  virtual void			CameraUpdate()																					{};
  virtual void      SetMouse(mmdMouse *mouse)                               {};
  virtual mafRWIBase *GetRWI()                                              {return NULL;};
  virtual void      GetImage(wxBitmap &bmp, int magnification = 1)          {};

  /** 
  Find the pocked VME at button down. As argument the function needs
  the device which performed the action, and provides as result pointers
  to piked prop, vme and its behavior if it exists. */
  virtual bool FindPokedVme(mafDevice *device,mafMatrix &point_pose,vtkProp3D *&picked_prop,mafVME *&picked_vme,mafInteractor *&picked_behavior) {return false;};

  /** return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... */
  //having mafView::GetNodeStatus allow mmgCheckTree to not know about mafSceneGraph
  virtual int 	    GetNodeStatus(mafNode *vme) {return NODE_NON_VISIBLE;};
  
  /** return the current pipe for the specified vme (if any exist at this moment) */
  virtual mafPipe*  GetNodePipe(mafNode *vme) {return NULL;};

  wxString  GetLabel() {return m_Label;};
  wxString  GetName()  {return m_Name;};
  void      SetLabel(wxString label)  {m_Label = label;};
  void      SetName(wxString name)    {m_Name = name;};

  virtual wxWindow*	      GetWindow(){return m_Win;};
  virtual wxFrame*		    GetFrame() {return m_Frame;};
  virtual mmgGui*		      GetGui()   {if(m_Gui == NULL) CreateGui(); assert(m_Gui); return m_Gui;};
//  virtual mmgGuiHolder*		GetGuih()  {return m_Guih;};
  virtual void		  SetFrame(wxFrame* f) {m_Frame = f;};

  //virtual void			HideGui();
  //virtual void			ShowGui();
//  virtual void			ShowSettings()							{};
  virtual void			OnSize(wxSizeEvent &size_event)	{};

  /** 
  Print this view.*/
  virtual void Print(wxDC *dc, wxRect margins) {};
  
  /** 
  Print the bitmap filling the paper size considering margins.*/
  virtual void PrintBitmap(wxDC *dc, wxRect margins, wxBitmap *bmp);

  /** 
  Set the vtk RenderWindow size. Used only for Linux (not necessary for Windows) */
  virtual void SetWindowSize(int w, int h) {};

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
  virtual mafVME *GetPickedVme() {return m_PickedVME;};

  /** 
  Return the corresponding vtkProp3D of the picked VME*/
  virtual vtkProp3D *GetPickedProp() {return m_PickedProp;};

protected:
  mafObserver   *m_Listener;
  wxString       m_Label;
  wxString       m_Name;
  wxWindow			*m_Win;
  wxFrame				*m_Frame;
  mmgGui      	*m_Gui;
  mmgGuiHolder	*m_Guih;

  vtkCellPicker *m_Picker2D;  ///< the picker used to pick the in the render window
  vtkRayCast3DPicker* m_Picker3D; ///< Used to pick in a VTK Render window

  mafVME        *m_PickedVME;   ///< Pointer to the picked vme. It is initialized on picking
  vtkProp3D     *m_PickedProp;  ///< Pointer to the picked vme Prop3D. It is initialized on picking
  double         m_PickedPosition[3];

  wxPrintData   *m_PrintData;

  /** 
  Find the VME picked */
  bool FindPickedVme(vtkAssemblyPath *ap = NULL);

public:
  int            m_Mult;    ///< Used to store the multiplicity of the view type created (e.g. the 3rd view surface created).
  int            m_Id;      ///< Used to store the view type created (e.g. view surface).
  bool           m_Plugged; // forget it - it is used from outside 
  mafView       *m_Next;    // forget it - it is used from outside 

  /** IDs for the GUI */
  enum VIEW_WIDGET_ID
  {
    ID_FIRST = MINID,
    ID_LAST
  };

  /** destroy the Gui */
  void DeleteGui();

protected:
  /**
  Internally used to create a new instance of the GUI. This function should be
  overridden by subclasses to create specialized GUIs. Each subclass should append
  its own widgets and define the enum of IDs for the widgets as an extension of
  the superclass enum. The last id value must be defined as "LAST_ID" to allow the 
  subclass to continue the ID enumeration from it. For appending the widgets in the
  same pannel GUI, each CreateGUI() function should first call the superclass' one.*/
  virtual mmgGui  *CreateGui();
};
#endif
