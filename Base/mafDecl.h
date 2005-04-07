/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDecl.h,v $
  Language:  C++
  Date:      $Date: 2005-04-07 08:47:09 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

/** Base declarations of the application core */

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// be careful in changing this file
// because force most of the MAF to be rebuilt
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef __mafDecl_H__
#define __mafDecl_H__

#include "mafDefines.h"
#include <string>
#include <vector>
// To be removed: no more including WX by default! (Marco)
/*
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif
*/

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVME;
class wxColour;
class wxWindow;

//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
// MAX number of widgets that mmgGUI can hold - used also by mmgGuiValidator
const int MAXWIDGET = 200; 
const int MAXOP = 200; 
const int MAXVIEW = 200; 

/** @ingroup Events */
/** @{ */
/**
  Most of the constants used in MAF are declared here.
  These constants must be non-overlapping.
  Constants are grouped by bracket xx_START, xx_END as needed by mmgFrame
*/
enum
{
MENU_START = 10000,
MENU_FILE_START,
  MENU_FILE_NEW,
  MENU_FILE_OPEN,
  MENU_FILE_SAVE,
  MENU_FILE_SAVEAS,
  MENU_FILE_MERGE,
  MENU_FILE_QUIT,
MENU_FILE_END,
MENU_EDIT_START,
  MENU_OP,
MENU_EDIT_END,
MENU_VIEW_START,
  MENU_VIEW_LAYOUT,        // call the Load/Save Layout Dialog
  MENU_VIEW_LAYOUT_MSF,    // call the Load/Save Layout_Msf Dialog 

SASH_START,            // ids of mmgSashPanel - must be Menu events too
	MENU_VIEW_TOOLBAR,
	MENU_VIEW_LOGBAR,
  MENU_VIEW_SIDEBAR,
  MENU_VIEW_TIMEBAR,
SASH_END,
MENU_LAYOUT_START,     // used by mmgCrossSplitter
  LAYOUT_ONE,          
  LAYOUT_TWO_VERT,
  LAYOUT_TWO_HORZ,
  LAYOUT_THREE_UP,
  LAYOUT_THREE_DOWN,
  LAYOUT_THREE_LEFT,
  LAYOUT_THREE_RIGHT,
  LAYOUT_FOUR,
  LAYOUT_LOAD,
  LAYOUT_SAVE,
MENU_LAYOUT_END,
MENU_VIEW_END,
MENU_SETTINGS_START,
  MENU_OPTION_DEVICE_SETTINGS,
  PRINT_TREE,
	ID_APP_SETTINGS,     // calls the Application-Settings-Panel - not implemented yet
	ID_TIMEBAR_SETTINGS, // calls the TimeBar-Settings-Panel
	ID_VIEW_SETTINGS,    // calls the Selected-View-Settings-Panel
MENU_SETTINGS_END,
MENU_HELP_START,
  ABOUT_APPLICATION,
MENU_HELP_END,

EVT_START,
  UPDATE_UI,           // update menu and toolbar widgets
	UPDATE_PROPERTY,     // update vme property widgets

  // connect a vtkObject to the ProgressBar
  BIND_TO_PROGRESSBAR, 
  // manually set the ProgressBar status
  PROGRESSBAR_SHOW, 
  PROGRESSBAR_HIDE, 
  PROGRESSBAR_SET_VALUE, 
  PROGRESSBAR_SET_TEXT, 

  // events about vme
  VME_ADD,       // from a generic mafOp to mafLogic  (to be removed Marco)
  VME_ADDED,     // from mafVmeManager to mafLogic (to be removed Marco)
  VME_REMOVE,    // from a generic mafOp to mafLogic (to be removed Marco)
  VME_REMOVING,  // from mafVmeManager to mafLogic 
  VME_SELECT,    // from mmgTree or mafISV - run mafOpSelect
  VME_SELECTED,  // from mafOpSelect
  VME_SHOW,      // from a vme property panel
  VME_TRANSFORM, // from mafISV to mafLogic
  VME_MODIFIED,  // from a generic mafOp to mafLogic (to be removed Marco)
  VME_CHOOSE,    // from a mafOp to receive more operands
  VME_PICKED,    // used by mafISALandmark to speak with mmoEditLandmarkCloud
  VME_CHOOSE_MATERIAL,
  VME_CHOOSE_TRANSFER_FUNCTION,
	VME_CHOOSE_FEM_PROPERTY,
	VME_REF_SYS_CHOOSED,  //from mmgVMERefSysChooser to a mafOp (to be moved elsewhere Marco)
  VME_FEM_DATA_CHOOSED, //from mmgVMEFEMDataChooser to a mafOp (to be moved elsewhere Marco)
  VME_CREATE_CLIENT_DATA, // (to be removed Marco)

  // events about devices
  DEVICE_ADD, // (to be removed Marco)
  DEVICE_REMOVE, // (to be removed Marco)
  DEVICE_ENABLE, // (to be removed Marco)

  // events about views
  VIEW_QUIT,		 // from keyboard, used specially to close external views.
	VIEW_CREATE,   // from menu to mafViewManager
	VIEW_CREATED,  // from mafViewManager to Logic
  VIEW_DELETE,   // from mmgMDIChild to mafViewManager
  VIEW_SELECT,   // from mmgMDIChild and mmgViewPanel to mafViewManager
  VIEW_CLICKED,  // from mafRWIBase to mmgMDIChild and mmgViewPanel to mafViewManager
	VIEW_MAXIMIZE, // from mmgGui to mmgCrossSplitter
	VIEW_SAVE_IMAGE,//from mafMDIChild  to mafViewManager
  ISV_PUSH,      // from an Operation to mafViewManager // (to be changed Marco)
  ISV_POP,       // from an Operation to mafViewManager // (to be changed Marco)

  CAMERA_FRONT,  // constant used in mafRWIxxx to initialize Camera pos
  CAMERA_BACK,
  CAMERA_LEFT,
  CAMERA_RIGHT,
  CAMERA_TOP,
  CAMERA_BOTTOM,
  CAMERA_PERSPECTIVE,
  CAMERA_RX_FRONT,
  CAMERA_RX_LEFT,
  CAMERA_RX_RIGHT,
	CAMERA_DRR_FRONT,
  CAMERA_DRR_LEFT,
  CAMERA_DRR_RIGHT,
  CAMERA_CT,
  CAMERA_OS_X,
  CAMERA_OS_Y,
  CAMERA_OS_Z,
  CAMERA_OS_P,
  CAMERA_PERSPECTIVE_FRONT,
  CAMERA_PERSPECTIVE_BACK,
  CAMERA_PERSPECTIVE_LEFT,
  CAMERA_PERSPECTIVE_RIGHT, 
  CAMERA_PERSPECTIVE_TOP,
  CAMERA_PERSPECTIVE_BOTTOM, 

  CAMERA_SYNCHRONOUS_UPDATE, // from Interaction Manager to perform a view update
  CAMERA_UPDATE, // events from the toolbar 
  CAMERA_RESET,
	CAMERA_FIT,
	CAMERA_FLYTO,
  CAMERA_PRE_RESET,
  CAMERA_POST_RESET,

  // modified by Stefano 21-9-2004 (begin)
  CAMERA_RXFEM_XNEG, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_XPOS, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_YNEG, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_YPOS, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_ZNEG, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_ZPOS, //(to be moved elsewhere Marco)
  //  //modified by Stefano 21-9-2004 (end)

	OP_ADD,       // events used by mafOpManager, mafOp and mafLogic
  OP_REMOVE,        
  OP_ENABLE,        
  OP_EXEC,        
  OP_RUN,        
  OP_RUN_OK,
  OP_RUN_CANCEL,
  OP_RUN_STARTING,
  OP_RUN_TERMINATED,
  OP_SHOW_GUI,
  OP_HIDE_GUI,
  OP_FORCE_STOP,

  ITEM_SELECTED, //used by mmgListCtrl and Dictionary

  LOG_ALL_EVENT,
	LOG_TO_FILE,

  //used by mafISATransform to talk with mmoTransform 
	MOUSE_DOWN, //(to be removed Marco)
	MOUSE_MOVE, //(to be removed Marco)
	MOUSE_UP,   //(to be removed Marco)
   
  ID_TRANSFORM, //added by Stefano. 30-11-2003 
  SHOW_VIEW_CONTEXTUAL_MENU,	//Added by Paolo 16-9-2003

  INTERACTOR_ADD,     //modified by Marco. 25-8-2004 bind an interactor to an action 
  INTERACTOR_REMOVE,  //modified by Marco. 25-8-2004 unbind an interactor from an action

EVT_USER_START, // to be used from vertical applications
EVT_USER_END = EVT_USER_START+1000,

EVT_END,

MENU_USER_START,// to be used from vertical applications
MENU_USER_END = MENU_USER_START+1000,

MENU_END,

// ids of operations - used by mafOpManager and mmgFrame
// must be after MENU_END
OP_START,
  OP_UNDO,
  OP_REDO,
  OP_DELETE,
  OP_CUT,
  OP_COPY,
  OP_PASTE,
  OP_SELECT,
  OP_TRANSFORM,
	OP_USER,
OP_END = OP_START + MAXOP,

// ids of views - used by mafViewManager and mmgFrame
// must be after MENU_END
VIEW_START,
VIEW_END = VIEW_START + MAXVIEW,

// events sent by widget

WIDGETS_START,
  ID_LAYOUT,         //sent from a mmgSashPanel to mmgFrame
  ID_CLOSE_SASH,     //sent from a mmgButton		to mmgSashPanel 
  ID_LABEL_CLICK,    //sent from a mmgLabel			to mmgViewPanel
  ID_DISPLAY_LIST,   //sent from a mmgButton		to mmgMDIChild  - ask for display list menu
  ID_SETTINGS,       //sent from a mmgButton		to mmgMDIChild  - ask to display a view-settings-panel
  ID_RANGE_MODIFIED, //sent from a mmgLutSlider	to mmgMDIChild  - ask to Views
WIDGETS_END,

// id of pictures - used in mmgBitmaps
PIC_START,  
  TIME_SET,				 //also sent by mmgTimeBar to mafLogic
  TIME_PLAY,       //pictures used by the timebar
  TIME_STOP,
  TIME_PREV,
  TIME_NEXT,
  TIME_BEGIN,
  TIME_END,

  APP_ICON, 
  PIC_NODE_BLUE,   //picture used by the tree
  PIC_NODE_YELLOW,
  PIC_NODE_RED,
  PIC_NODE_GRAY,

	PIC_ZOOM_ALL,    //picture used by the toolbar
	PIC_ZOOM_SEL,

  PIC_OPACITY_0,   //pictures not currently used
  PIC_OPACITY_05,
  PIC_OPACITY_1,
  MENU_FILE_OPEN_D,  
  MENU_FILE_SAVE_D,  
  MENU_EDIT_UNDO_D,  
  MENU_EDIT_REDO_D,  
  MENU_EDIT_CUT_D,  
  MENU_EDIT_COPY_D,  
  MENU_EDIT_PASTE_D,  

  CAMERA_RESET_D,
  CAMERA_ZOOM1,
  CAMERA_ZOOM2,
  CAMERA_ZOOM3,
  CAMERA_ZOOM4,
  PIC_END,

  PIC_USER_START = PIC_START+500 ,// to be used from vertical applications
	PIC_USER_END = PIC_USER_START +100,// to be used from vertical applications

PARSE_STRING,	//Added by Paolo 16-9-2003 execute the operation passed as a string
APPSTAMP,

// ids for event channels
MCH_START,
  MCH_UP,     ///< channel used to send events up in the tree
  MCH_DOWN,   ///< channel used to send events down in the tree
  MCH_INPUT,  ///< channel used for events coming from an input device
  MCH_OUTPUT, ///< channel used for events sent to an output device
  MCH_NODE,   ///< channel used by nodes for sending events to observers
MCH_END,

// ids issued/observed by mafNode and mafVME
NODE_START,
  NODE_DETACHED_FROM_TREE,///< issued when the node is detachment from the tree
  NODE_ATTACHED_TO_TREE,  ///< issued when the node is attached to the tree
  NODE_DESTROYED,         ///< issued when the node is destroyed
  NODE_GET_STORAGE,       ///< issued by tree elements to retrieve the storage (served by mafNodeRoot)
  NODE_GET_ROOT,          ///< issued by subnode components to extract ROOT node
  VME_TIME_SET,           ///< used either to set or to advise of time changes
  VME_MATRIX_CHANGED,     ///< issued when the matrix in the output has changed (e.g. when matrix pipe is changed at runtime)
  VME_MATRIX_PREUPDATE,  ///< issued by mafMatrixPipe before updating the matrix 
  VME_MATRIX_UPDATE,      ///< issued when pose matrix is updated
  VME_ABSMATRIX_UPDATE,   ///< issued when absolute pose matrix is updated
  VME_OUTPUT_DATA_PREUPDATE, ///< issued by mafDataPipe before the output data is actually updated
  VME_OUTPUT_DATA_UPDATE, ///< issued when the output data is updated
  VME_OUTPUT_DATA_CHANGED,///< issued when the output data object has changed (i.e. object pointer changed)
  VME_GET_NEWITEM_ID,     ///< issued by VMEItems or DataVector to retrieve a new item-ID (served by mafVMERoot)
NODE_END,

MATRIX_UPDATED, ///< issue by a matrix pipe to advise consumers of output matrix updating

// const used by mmgGui and mmgGuiValidator
MINID,      
MAXID = MINID + MAXWIDGET,

ID_GUI_UPDATE,

MAF_BASE_ID ///< base numeric ID used by mafIdFactory. This must be the LAST ONE!!!
};
/** @} */

//----------------------------------------------------------------------------
// global functions
//----------------------------------------------------------------------------

/** 
  return the application frame window.
  to be used as parent of panels to be shown later. */    
extern wxWindow* mafGetFrame();

/** global function to retrieve the vme base type. */
//mafVmeBaseTypes mafGetBaseType(mflVME* vme); //(to be removed Marco)

/** to be used by mmgFrame and derived class only */    
void      mafSetFrame(wxWindow* frame);

/** process pending events. be careful not to call it recursively */    
void      mafYield();

/** show the File Open Dialog Box */
std::string  mafGetOpenFile(const char * initial, const char * wildcard, const char * title = "Open File", wxWindow *parent = NULL);

/** show the File Open Dialog Box for multiple file selection */
void mafGetOpenMultiFiles(const char * initial, const char * wildcard, std::vector<std::string> &files, const char * title = "Open Files", wxWindow *parent = NULL);

/** show the File Save Dialog Box */
std::string  mafGetSaveFile(const char * initial, const char * wildcard, const char * title = "Save File", wxWindow *parent = NULL);

/** return a random wxColour from a palette of 16 */
wxColour  mafRandomColor();

/** return the application start-up directory */
std::string  mafGetApplicationDirectory();

/** translate an event-id into a readable string */
std::string  mafIdString(int id);

#endif









