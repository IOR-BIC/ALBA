/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medGUIDynamicVP.h,v $ 
  Language: C++ 
  Date: $Date: 2010-01-08 13:55:59 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================  
*/

#ifndef __medGUIDynamicVP_H__
#define __medGUIDynamicVP_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafEvent.h"
#include "mafObserver.h"
#include "mafGUIPanel.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafPipe;
class mafSceneNode;

class mafView;
class mafGUI;
class mafGUIScrolledPanel;
class wxStaticLine;

//----------------------------------------------------------------------------
/**
 medGUIDynamicVP is a wxPanel with a set of widget to handle dynamic 
construction/destruction of visual pipe according to the user choice. In
its default mode, it displays a GUI with a combobox containing a list of
supported visual pipes (passed by the caller) and a close button. At the
beginning, no visual pipe is selected. When the user changes the selection,
the corresponding visual pipe is constructed and its GUI is placed bellow
the widgets of this panel. Similarly, when the user clicks on the close
button, the visual pipe is destroyed. After both actions are completed, the
listener is notified by mafEvent with ID equal to the specified one (given
in ctor) and with ID_CREATE_VP or ID_CLOSE_VP argument (see
mafEvent::GetArg). medGUIDynamicVP can be configured so it does not contain
neither close button nor create combo nor any other widget. In such a
special mode, the construction of visual pipe is governed by the caller.*/
//----------------------------------------------------------------------------
class medGUIDynamicVP: public mafGUIPanel, public mafObserver
{
public:  
  enum PLAYER_WIDGET_ID
  {
    ID_NAME = MINID, //must be MINID otherwise validators do not work
    ID_CREATE_VP,
    ID_CLOSE_VP,
  };

  enum GUI_STYLE
  {
    GS_DEFAULT        = 0,
    GS_READONLY_NAME  = 1,
    GS_NO_NAME        = 2,
    GS_NO_CREATE_VP   = 4,
    GS_NO_CLOSE_VP    = 8,    
  };

  //structure to store information about visual pipe
  typedef struct SUPPORTED_VP_ENTRY
  {
    const char* szClassName;            ///<visual pipe class name
    const char* szUserFriendlyName;     ///<user friendly name to be displayed in GUI
  } SUPPORTED_VP_ENTRY;

protected:
#pragma region Register of Pipes
  /**
    class name: medPipeRegister
    This class registe the pipes. It handle a pipe vector container.
  */
  class medPipeRegister
  {
  protected:    
    typedef struct VIEW_ITEM
    {
      mafView* m_View;
      std::vector< mafPipe* > m_Pipes;      
    } VIEW_ITEM;
      
    static std::vector< VIEW_ITEM > m_RegViews; ///<array of registered views-pipes
  public:
    /** Registers the specified pipe with the view.
    Returns number of pipes registered for this view.
    N.B. duplicity check of pipes is not performed!*/
    static int RegisterPipe(mafPipe* pipe, mafView* view);

    /** Unregisters the specified pipe with the view (if specified).
    Returns number of pipes registered for this view (after unregister). */
    static int UnregisterPipe(mafPipe* pipe, mafView* view = NULL);
  protected:
    /** returns index of given view in m_RegViews, or -1, if not found */
    static int FindView(mafView* view);
  };
#pragma endregion Register of Pipes

protected:  
  long m_GuiStyle;                ///<style/mode of the GUI
  wxWindowID  m_NotifyId;         ///<Id used for when listener is notified
  mafObserver* m_Listener;        ///<object that is notified when something changes (notified by ID)

  const SUPPORTED_VP_ENTRY* m_VPipes; ///<list of currently supported visual pipes, terminated by (NULL, NULL)

  int m_VPipeIndex;             ///<index of the selected visual pipe
  mafPipe* m_VPipe;             ///<this is the current visual pipe  
  mafSceneNode* m_SceneNode;    ///<reference to the scene node associated with this pipe
  mafString m_Name;             ///<user name associated with this panel

#pragma region GUI
  wxComboBox* m_ComboVP;

  wxPanel* m_GUI_This;
  mafGUIScrolledPanel* m_GUI_VP;  
#pragma endregion GUI

  bool m_BGUIThisShown;      ///<true, if the GUIThis is shown

protected:
  /** declare event table macro */
  DECLARE_EVENT_TABLE();

public:
  /** constructor */
  medGUIDynamicVP(wxWindow* parent, wxWindowID id, 
    long GUIstyle =  GS_DEFAULT, 
    const wxPoint& pos = wxDefaultPosition, 
    const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
  /** destructor */
  virtual ~medGUIDynamicVP();    

  /** Gets the listener that will be notified when any control changes. */
  inline mafObserver* GetListener(){
    return m_Listener;
  };

  /** Specifies the listener that will be notified when any control changes. */
  inline void SetListener(mafObserver *Listener){
    m_Listener = Listener;
  };

  /** Gets the currently associated name with the GUI. */
  inline const char* GetName() {
    return m_Name;
  }

  /** Sets a new name associated with the GUI. */
  void SetName(const char* szNewName);    

  /** Gets the current list of visual pipes */
  inline const SUPPORTED_VP_ENTRY* GetVPipesList() {
    return m_VPipes;
  }

  /** Sets a new list of visual pipes. */
  void SetVPipesList(const SUPPORTED_VP_ENTRY* pList);

  /** Gets the currently selected visual pipe */
  inline int GetVPipeIndex() {
    return m_VPipeIndex;
  }

  /** Sets a new visual pipe */
  void SetVPipeIndex(int nNewIndex);  

  /** Gets the current style */
  inline long GetGUIStyle() {
    return m_GuiStyle;
  }

  /** Sets a new style = combination from GUI_STYLE */
  void SetGUIStyle(long newtyle);

  /** Gets the current scene node. */
  inline mafSceneNode* GetSceneNode() {
    return m_SceneNode;
  }

  /** Sets a new scene node for visual pipes. 
  N.B. currently constructed visual pipe is recreated, if needed. */
  void SetSceneNode(mafSceneNode* node);  

  /** Gets the currently constructed visual pipe. */
  inline mafPipe* GetCurrentVisualPipe() {
    return m_VPipe;
  }

  /** process the events sent by subjects */
  /*virtual*/ void OnEvent(mafEventBase *e);

protected:  
  /** Constructs geometry visual pipe. */
  virtual void CreateVisualPipe(const char* classname);

  /** Destroys geometry visual pipe. */
  virtual void DestroyVisualPipe();

  /** Handles the construction of VP */
  virtual void OnCreateVP();  

  /** Handles the closing of VP */
  virtual void OnCloseVP();  

  /** Notifies the listener, sending the specified notify id 
  and nData as and argument.  */
  virtual void NotifyListener(long nData);

  /**
	Creates GUI (to be added into m_GUI_This) according to the given style. 
  This routine is called from SetStyle and from the ctor. 
  N.B. when called from ctor the overriden function is not 
  called but this one (C++ feature). */
  virtual wxSizer* CreateGUI(long style);

  /** Handles destroying of controls. Controls cannot be destroyed 
  immediately as they are referenced by wxWidgets event handing core.
  Its immediate destruction would cause crash. */
  void OnDeferedDelete(wxEvent& event);
};
#endif
