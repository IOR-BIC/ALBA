/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaView
 Authors: Silvano Imboden - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaView_H__
#define __albaView_H__
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaObserver.h"
#include "albaServiceClient.h"
#include "albaAbsLogicManager.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaPipe;
class albaSceneGraph;
class albaVME;
class albaGUI;
class albaGUIHolder;
class albaDeviceButtonsPadMouse;
class albaDevice;
class albaInteractor;

class albaRWIBase;
class vtkCellPicker;
class vtkALBARayCast3DPicker;
class vtkAssemblyPath;
class vtkProp3D;

//----------------------------------------------------------------------------
// const :
//----------------------------------------------------------------------------
/** status of a SceneNode -- do not change, it is used by albaCheckTree also */
enum NODE_STATUS_ID
{
  NODE_NON_VISIBLE =0,
  NODE_VISIBLE_OFF,
  NODE_VISIBLE_ON,
  NODE_MUTEX_OFF,
  NODE_MUTEX_ON,
};
/** VME visibility attribute when plugging visual pipe.*/
enum VME_VISIBILITY_ID
{
  NON_VISIBLE = 0,
  VISIBLE,
  MUTEX,
};

//----------------------------------------------------------------------------
// albaView :
//----------------------------------------------------------------------------
/** 
albaView is the base class for Views in alba.

The member function GetNodeStatus(vme) will be called by the albaGUICheckTree 
in order to update it's icons. (Note that albaGUICheckTree doesn't know about
the SceneGraph anymore)

albaView doesn't have a Scenegraph, nor knowledge about VTK /sa albaViewVTK.

albaView can be the base class for composed Views.

*/
class ALBA_EXPORT albaView: public albaObserver, public albaServiceClient, public albaObject
{
public:
  albaView(const wxString &label = "View");
  virtual ~albaView(); 

	albaAbstractTypeMacro(albaView, albaObject);

  virtual void			SetListener(albaObserver *Listener) {m_Listener = Listener;};
  virtual void			OnEvent(albaEventBase *alba_event);
  virtual albaView*  Copy(albaObserver *Listener, bool lightCopyEnabled = false) {m_LightCopyEnabled = lightCopyEnabled; return NULL;};
  virtual void      Create() {};

  virtual void			VmeAdd(albaVME *vme)															{};
  virtual void			VmeRemove(albaVME *vme)														{};
  virtual void			VmeSelect(albaVME *vme, bool select)							{};
  virtual void			VmeShow(albaVME *vme, bool show)									{};
	virtual void      VmeUpdateProperty(albaVME *vme, bool fromTag = false)		{};
	virtual bool			IsVmeShowed(albaVME *vme) { return GetNodePipe(vme) != NULL; };

  virtual void	    VmeCreatePipe(albaVME *vme)										    {};
  virtual void	    VmeDeletePipe(albaVME *vme)										    {};

  virtual void			CameraReset(albaVME *node = NULL)  											{};
  virtual void			CameraUpdate()																					{};
  virtual void      SetMouse(albaDeviceButtonsPadMouse *mouse)                               {};
  virtual albaRWIBase *GetRWI()                                              {return NULL;};
  virtual albaSceneGraph *GetSceneGraph()                                    {return NULL;};
  virtual void      GetImage(wxBitmap &bmp, int magnification = 1)          {};

  /** Called to update all components that depends on Application Options.*/
  virtual void      OptionsUpdate()                                         {};

  /** 
  Find the pocked VME at button down. As argument the function needs
  the device which performed the action, and provides as result pointers
  to piked prop, vme and its behavior if it exists. */
  virtual bool FindPokedVme(albaDevice *device,albaMatrix &point_pose,vtkProp3D *&picked_prop,albaVME *&picked_vme,albaInteractor *&picked_behavior) {return false;};

  /** return the status of the node within this view. es: NON_VISIBLE,VISIBLE_ON, ... */
  //having albaView::GetNodeStatus allow albaGUICheckTree to not know about albaSceneGraph
  virtual int GetNodeStatus(albaVME *vme) {return NODE_NON_VISIBLE;};
  
  /** return the current pipe for the specified vme (if any exist at this moment) */
  virtual albaPipe* GetNodePipe(albaVME *vme) {return NULL;};

  wxString  GetLabel() {return m_Label;};
  wxString  GetName()  {return m_Name;};
  void      SetLabel(wxString label)  {m_Label = label;};
  void      SetName(wxString name)    {m_Name = name;};

  virtual wxWindow*	GetWindow(){return m_Win;};
  virtual wxFrame*	GetFrame() {return m_Frame;};
  virtual albaGUI*		GetGui()   {if(m_Gui == NULL) CreateGui(); assert(m_Gui); return m_Gui;};

	/** destroy the Gui */
	void DeleteGui();

  virtual void		  SetFrame(wxFrame* f) {m_Frame = f;};

  virtual void			OnSize(wxSizeEvent &size_event)	{};
	    
  /** Set the vtk RenderWindow size. Used only for Linux (not necessary for Windows) */
  virtual void SetWindowSize(int w, int h) {};

  /** Perform a picking according to the screen position (x,y) and return true on success*/
  virtual bool Pick(int x, int y);

  /** Perform a picking according to the absolute matrix given and return true on success*/
  virtual bool Pick(albaMatrix &m);

  /** Write into the double array the position picked during Pick method.*/
  virtual void GetPickedPosition(double pos[3]);

  /** Return the picked VME during the Pick method. Return NULL if VME is not found*/
  virtual albaVME *GetPickedVme() {return m_PickedVME;};

  /** Return the corresponding vtkProp3D of the picked VME*/
  virtual vtkProp3D *GetPickedProp() {return m_PickedProp;};

  /** Return the number of visible VMEs.*/ 
  virtual int GetNumberOfVisibleVME() {return m_NumberOfVisibleVme;};

  /** Print a dump of this object into the Log area.*/
  virtual void Print(std::ostream& os, const int tabs=0);// const;

  /** IDs for the GUI */
  enum VIEW_WIDGET_ID
  {
    ID_FIRST = MINID,
    ID_PRINT_INFO,
	ID_HELP,
    ID_LAST
  };

  /** Retrieve html information */
  virtual const char *GetHTMLText(){return m_HTMLText.GetCStr();};

  /** Return the picker used to pick the in the render window */
  vtkCellPicker *GetPicker2D() {return m_Picker2D;};

	/** return an xpm-icon that can be used to represent this view */
	virtual char ** GetIcon();

	virtual wxColor GetBackgroundColor() { return m_BackgroundColor; };
	virtual void SetBackgroundColor(wxColor color) = 0;


	/** Returns CanSpin */
	bool GetCanSpin() const { return m_CanSpin; }

	/** Sets CanSpin */
	void SetCanSpin(bool canSpin) { m_CanSpin = canSpin; }

protected:
  albaObserver   *m_Listener;
  wxString       m_Label;
  wxString       m_Name;
  wxWindow			*m_Win;
  wxFrame				*m_Frame;
  albaGUI      	*m_Gui;
  albaGUIHolder	*m_Guih;

  vtkCellPicker *m_Picker2D;  ///< the picker used to pick the in the render window
  vtkALBARayCast3DPicker* m_Picker3D; ///< Used to pick in a VTK Render window

  albaVME        *m_PickedVME;   ///< Pointer to the picked vme. It is initialized on picking
  vtkProp3D     *m_PickedProp;  ///< Pointer to the picked vme Prop3D. It is initialized on picking
  double         m_PickedPosition[3];
  int            m_NumberOfVisibleVme; ///< perform ResetCamera only for the first vme shown into the view

  wxPrintData   *m_PrintData;
  albaString      m_HTMLText;

	wxColor				m_BackgroundColor;

	bool m_LightCopyEnabled;
	bool m_CanSpin;

  /** Find the VME picked */
  bool FindPickedVme(vtkAssemblyPath *ap = NULL);

	/**
	Internally used to create a new instance of the GUI. This function should be
	overridden by subclasses to create specialized GUIs. Each subclass should append
	its own widgets and define the enum of IDs for the widgets as an extension of
	the superclass enum. The last id value must be defined as "LAST_ID" to allow the
	subclass to continue the ID enumeration from it. For appending the widgets in the
	same panel GUI, each CreateGUI() function should first call the superclass' one.*/
	virtual albaGUI  *CreateGui();



public:
  int            m_Mult;    ///< Used to store the multiplicity of the view type created (e.g. the 3rd view surface created).
  int            m_Id;      ///< Used to store the view type created (e.g. view surface).
  bool           m_Plugged; // forget it - it is used from outside 
  albaView       *m_Next;    // forget it - it is used from outside 

};
#endif
