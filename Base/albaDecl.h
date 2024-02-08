/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDecl
 Authors: Silvano Imboden, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDecl_H__
#define __albaDecl_H__
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// be careful in changing this file
// because force most of the ALBA to be rebuilt
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include "albaDefines.h"
#include <wx/bitmap.h>

#include "albaString.h"

#include <vector>


/**
@mainpage
ALBA: Agile Library for Biomedical Applications

ALBA is a versatile library designed to facilitate the management, visualization, and manipulation of biomedical data. Specifically tailored for biomedical applications, ALBA smoothly handles various data types, including medical images (CT scans, X-rays, MRIs), surfaces, and meshes.

Key Features:

Diverse Data Support:
ALBA offers reliable support for a range of biomedical data formats, making it a practical solution for researchers and healthcare professionals dealing with different types of data. From intricate 3D meshes to detailed medical images, ALBA streamlines the management of diverse data types.

Efficient Data Handling:
ALBA simplifies the manipulation of biomedical data, allowing users to perform tasks like segmentation, registration, and transformation with ease, enhancing their ability to derive insights.

Interactive Visualization:
The library provides interactive visualization tools for exploring 3D structures, making it easier for users to analyze and interpret their data visually.

Scalability and Performance:
Engineered for efficiency, ALBA meets the demands of both small-scale research projects and larger biomedical applications, ensuring swift data processing and analysis.

Open Source Collaboration:
ALBA is an open-source initiative that encourages collaboration within the biomedical community. Researchers and developers can contribute to its development, ensuring continuous improvement and adaptation to evolving biomedical challenges.

Whether you are involved in medical research, diagnostic imaging, or clinical applications, ALBA serves as a flexible and practical solution for advancing biomedical data management and analysis.

Explore the capabilities of ALBA today to enhance your biomedical applications effectively.
*/

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class wxColour;
class wxWindow;

//----------------------------------------------------------------------------
// constant
//----------------------------------------------------------------------------
// MAX number of widgets that albaGUI can hold - used also by albaGUIValidator
//const int MAXWIDGET = 200; 
const int MAXWIDGET = 4096;
const int MAXOP = 200; 
const int MAXVIEW = 200;
const int MAXWIZARD = 50;  

// extern ALBA_EXPORT int ALBAExpertMode; ///< Flag to turn On/Off detailed debug information or GUI widgets

/** Data Type enum that identify data stored in WidgetDataType struct */
enum ALBA_EXPORT WIDGET_DATA_TYPE_ID
{
  NULL_DATA,
  INT_DATA,
  FLOAT_DATA,
  DOUBLE_DATA,
  STRING_DATA,
};

struct ALBA_EXPORT WidgetDataType
{
  int    iValue;
  float  fValue;
  double dValue;
  const char *sValue;
  int    dType;
};

/** conventional device button values */
enum ALBA_EXPORT DEVICE_BUTTONS
{
  ALBA_ANY_BUTTON = -1,
  ALBA_LEFT_BUTTON = 0,
  ALBA_MIDDLE_BUTTON,    
  ALBA_RIGHT_BUTTON
};

/** conventional key modifiers values */
enum ALBA_EXPORT DEVICE_MODIFIERS
{
  ALBA_SHIFT_KEY = 0,
  ALBA_CTRL_KEY,
  ALBA_ALT_KEY
};

/** @ingroup Events */
/** @{ */
/**
  Most of the constants used in ALBA are declared here.
  These constants must be non-overlapping.
  Constants are grouped by bracket xx_START, xx_END as needed by albaGUIFrame
*/
enum ALBA_EXPORT MAIN_EVENT_ID
{
MENU_START = 10000,
MENU_FILE_START,
  MENU_FILE_NEW,
  MENU_FILE_OPEN,
  MENU_FILE_SAVE,
  MENU_FILE_SAVEAS,
  MENU_FILE_MERGE,
  MENU_FILE_PRINT,
  MENU_FILE_PRINT_PREVIEW,
  MENU_FILE_PRINT_SETUP,
  MENU_FILE_PRINT_PAGE_SETUP,
  MENU_FILE_QUIT,
MENU_FILE_END,
MENU_EDIT_START,
  MENU_EDIT_FIND_VME,
  MENU_OP,
MENU_EDIT_END,
MENU_VIEW_START,
  MENU_VIEW_LAYOUT,        // call the Load/Save Layout Dialog
  MENU_VIEW_LAYOUT_MSF,    // call the Load/Save Layout_Msf Dialog 
SASH_START,                // ids of albaGUISashPanel/ ids of Dockable Panes - must be Menu events too
	MENU_VIEW_TOOLBAR,
	MENU_VIEW_LOGBAR,
  MENU_VIEW_SIDEBAR,
  MENU_VIEW_TIMEBAR,
  MENU_VIEW_USER_FIRST,
  MENU_VIEW_USER_LAST = MENU_VIEW_USER_FIRST+20,
SASH_END,
MENU_LAYOUT_START,     // used by albaGUICrossSplitter
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
  MENU_OPTION_APPLICATION_SETTINGS,
  MENU_OPTION_DEVICE_SETTINGS,
  MENU_OPTION_LOCALE_SETTINGS,
  MENU_OPTION_MEASURE_UNIT_SETTINGS,
  PRINT_TREE,
	ID_APP_SETTINGS,     // calls the Application-Settings-Panel - not implemented yet
	ID_TIMEBAR_SETTINGS, // calls the TimeBar-Settings-Panel
	ID_VIEW_SETTINGS,    // calls the Selected-View-Settings-Panel
MENU_SETTINGS_END,
MENU_HELP_START,
  ABOUT_APPLICATION,
	REGISTER_PRODUCT,
	GENERATE_LICENCE,
  HELP_HOME,
MENU_HELP_END,
GUI_HELP_START,
  GET_BUILD_HELP_GUI, // sent from op, views or vme's to know if we are building the help gui in the application
  OPEN_HELP_PAGE, // sent from an op, a view or a vme to open the corresponding help page
GUI_HELP_END,

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
  VME_ADDED,     // from albaVmeManager to albaLogic (to be removed Marco)
  VME_REMOVING,  // from albaVmeManager to albaLogic 
  VME_SELECT,    // from albaGUITree or albaISV - run albaOpSelect
  VME_DCLICKED,  // from albaInteractorPER issued to infor logic that a double click has been done on a VME.
  VME_SELECTED,  // from albaOpSelect
  VME_TRANSFORM, // from albaISV to albaLogic
  VME_CHOOSE,    // from a albaOp to receive more operands
  VME_PICKED,    // used sent to the listener a vtkPoint positioned at the picked position
  VME_PICKING,   // same as above, but send point position in continuous configuration (at each mouse motion)
  VME_CHOOSE_MATERIAL,
  VME_CHOOSE_TRANSFER_FUNCTION,
  VME_CHOOSE_FEM_PROPERTY,
  VME_REF_SYS_CHOOSED,  //from albaGUIVMERefSysChooser to a albaOp (to be moved elsewhere Marco)
  VME_FEM_DATA_CHOOSED, //from albaGUIVMEFEMDataChooser to a albaOp (to be moved elsewhere Marco)
  CREATE_STORAGE,

  // events about views
  VIEW_QUIT,		 // from keyboard, used specially to close external views.
  VIEW_CREATE,   // from menu to albaViewManager
  VIEW_CREATED,  // from albaViewManager to Logic
  VIEW_DELETE,   // from albaGUIMDIChild to albaViewManager
  VIEW_SELECT,   // from albaGUIMDIChild and albaGUIViewPanel to albaViewManager
  VIEW_SELECTED, // used to ask to logic if there is a view selected and get a pointer to it
  VIEW_CLICKED,  // from albaRWIBase to albaGUIMDIChild and albaGUIViewPanel to albaViewManager
  VIEW_MAXIMIZE, // from albaGUI to albaGUICrossSplitter
  VIEW_RESIZE,   // from anyone that want to block the view's size.
  VIEW_SAVE_IMAGE,//from albaMDIChild  to albaViewManager
  PER_PUSH,       // from an Operation to albaInteractionManager
  PER_POP,        // from an Operation to albaInteractionManager
	GET_CURRENT_PER,

  CAMERA_FRONT,  // constant used in albaRWIxxx to initialize Camera pos
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
	CAMERA_ARB, //Camera with arbitrary look vector

  CAMERA_SYNCHRONOUS_UPDATE, // from Interaction Manager to perform a view update
  CAMERA_UPDATE, // events from the toolbar 
  CAMERA_RESET,
	CAMERA_FIT,
	CAMERA_FLYTO,
  CAMERA_PRE_RESET,
  CAMERA_POST_RESET,
	CAMERA_MOVED, //Event invoked by albaInteractorCameraMove

  // modified by Stefano 21-9-2004 (begin)
  CAMERA_RXFEM_XNEG, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_XPOS, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_YNEG, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_YPOS, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_ZNEG, //(to be moved elsewhere Marco)
  CAMERA_RXFEM_ZPOS, //(to be moved elsewhere Marco)
  //  //modified by Stefano 21-9-2004 (end)

	CAMERA_BLEND,

	OP_ADD,       // events used by albaOpManager, albaOp and albaLogic
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

  ITEM_SELECTED, //used by albaGUIListCtrl and Dictionary

  IMPORT_FILE, // used when drag a non msf or zmsf file into the application as a VTK file.

  CLEAR_UNDO_STACK, // used by albaGUIMDIFrame to clear UnDo stack when the application is running in low memory

  LOG_ALL_EVENT,
	LOG_TO_FILE,

  //used by albaISATransform to talk with albaOpTransform 
	MOUSE_DOWN, //(to be removed Marco)
	MOUSE_MOVE, //(to be removed Marco)
	MOUSE_UP,   //(to be removed Marco)
  MOUSE_WHEEL, //(Used by albaGUI to send mouse wheel event to the listener) added by Losi 06/28/2012
   
  ID_TRANSFORM, //added by Stefano. 30-11-2003 
	SHOW_VIEW_CONTEXTUAL_MENU,	
	SHOW_TREE_CONTEXTUAL_MENU,	


  INTERACTOR_ADD,     //modified by Marco. 25-8-2004 bind an interactor to an action 
  INTERACTOR_REMOVE,  //modified by Marco. 25-8-2004 unbind an interactor from an action

  DEVICE_ADD,
  DEVICE_REMOVE,
  DEVICE_GET,
  // resize window
  TILE_WINDOW_CASCADE,
  TILE_WINDOW_HORIZONTALLY,
  TILE_WINDOW_VERTICALLY,

EVT_USER_START, // to be used from vertical applications
EVT_USER_END = EVT_USER_START+1000,

EVT_END,

MENU_USER_START,// to be used from vertical applications
MENU_USER_END = MENU_USER_START+1000,

MENU_END,

// ids of operations - used by albaOpManager and albaGUIFrame
// must be after MENU_END
OP_START,
  OP_UNDO,
  OP_REDO,
  OP_DELETE,
  OP_CUT,
  OP_COPY,
  OP_PASTE,
  OP_RENAME,
  OP_REPARENT,
  OP_ADD_GROUP,
  OP_SELECT,
  OP_TRANSFORM,
	OP_USER,
OP_END = OP_START + MAXOP,

// ids of views - used by albaViewManager and albaGUIFrame
// must be after MENU_END
VIEW_START,
VIEW_END = VIEW_START + MAXVIEW,

// events sent by widget

WIDGETS_START,
  ID_LAYOUT,         //sent from a albaGUISashPanel to albaGUIFrame
  ID_CLOSE_SASH,     //sent from a albaGUIButton		to albaGUISashPanel 
  ID_LABEL_CLICK,    //sent from a albaGUILabel			to albaGUIViewPanel
  ID_DISPLAY_LIST,   //sent from a albaGUIButton		to albaGUIMDIChild  - ask for display list menu
  ID_SETTINGS,       //sent from a albaGUIButton		to albaGUIMDIChild  - ask to display a view-settings-panel
  ID_RANGE_MODIFIED, //sent from a albaGUILutSlider	to albaGUIMDIChild  - ask to Views
WIDGETS_END,

// id of pictures - used in albaGUIBitmaps
PIC_START,  
  TIME_SET,				 //also sent by albaGUITimeBar to albaLogic
  TIME_PLAY,       //pictures used by the timebar
  TIME_STOP,
  TIME_PREV,
  TIME_NEXT,
  TIME_BEGIN,
  TIME_END,
  MOVIE_RECORD,
  /*
  APP_ICON, 
  PIC_NODE_BLUE,   //picture used by the tree
  PIC_NODE_YELLOW,
  PIC_NODE_RED,
  PIC_NODE_GRAY,
	PIC_ZOOM_ALL,    //picture used by the toolbar
	PIC_ZOOM_SEL,
  */ 
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
  MCH_ANY,    ///< Id used by albaAgent to identify any channel connected to it
  MCH_VTK,    ///< channel used to as a bridge from VTK objects
MCH_END,

// ids issued/observed by albaVME and albaVME
NODE_START,
  NODE_DETACHED_FROM_TREE,///< issued when the node is detachment from the tree
  NODE_ATTACHED_TO_TREE,  ///< issued when the node is attached to the tree
  NODE_DESTROYED,         ///< issued when the node is destroyed
  NODE_GET_STORAGE,       ///< issued by tree elements to retrieve the storage (served by albaVMERoot)
  NODE_GET_ROOT,          ///< issued by subnode components to extract ROOT node
  VME_TIME_SET,           ///< used either to set or to advise of time changes
  VME_MATRIX_CHANGED,     ///< issued when the matrix in the output has changed (e.g. when matrix pipe is changed at runtime)
  VME_MATRIX_PREUPDATE,  ///< issued by albaMatrixPipe before updating the matrix 
  VME_MATRIX_UPDATE,      ///< issued when pose matrix is updated
  VME_ABSMATRIX_UPDATE,   ///< issued when absolute pose matrix is updated
  VME_OUTPUT_DATA_PREUPDATE, ///< issued by albaDataPipe before the output data is actually updated
  VME_OUTPUT_DATA_UPDATE, ///< issued when the output data is updated
  VME_OUTPUT_DATA_CHANGED,///< issued when the output data object has changed (i.e. object pointer changed)
  VME_GET_NEWITEM_ID,     ///< issued by VMEItems or DataVector to retrieve a new item-ID (served by albaVMERoot)
NODE_END,

AVATAR_ADDED,     ///< used to advise the InteractionManager an avatar has been added (issued by mmdTracker's)
AVATAR_REMOVED,   ///< used to advise the InteractionManager an avatar has been removed (issued by mmdTracker's)

MATRIX_UPDATED, ///< issue by a matrix pipe to advise consumers of output matrix updating

// const used by albaGUI and albaGUIValidator
MINID,      
MAXID = MINID + MAXWIDGET,

ID_GUI_UPDATE,

ALBA_BASE_ID ///< base numeric ID used by albaIdFactory. This must be the LAST ONE!!!
};

enum MED_MAIN_EVENT_ID
{
  ID_VME_BEHAVIOR_UPDATE = EVT_USER_START,
  MENU_FILE_SNAPSHOT,
	MENU_FILE_MANAGE_SNAPSHOT,
  ID_GET_FILENAME,
  WIZARD_RUN_STARTING,
  WIZARD_RUN_CONTINUE,
  WIZARD_RUN_TERMINATED,
  WIZARD_REQUIRED_VIEW,
  WIZARD_DELETE_VIEW,
  WIZARD_RUN_OP,
  WIZARD_UPDATE_WINDOW_TITLE,
  WIZARD_PAUSE,
  WIZARD_OP_DELETE,
  WIZARD_OP_NEW,
  WIZARD_SWITCH,
  WIZARD_RELOAD_MSF,
  WIZARD_INFORMATION_BOX_SHOW_SET,
  WIZARD_INFORMATION_BOX_SHOW_GET,
  WIZARD_START,
  WIZARD_END=WIZARD_START+MAXWIZARD,
  MED_EVT_USER_START,
};

enum MED_MENU_EVENT_ID
{
   MENU_WIZARD = MENU_USER_START,
   MED_MENU_USER_START,
};

/** @} */

//----------------------------------------------------------------------------
// global functions
//----------------------------------------------------------------------------

/** 
  return the application frame window.
  to be used as parent of panels to be shown later. */    
ALBA_EXPORT extern wxWindow* albaGetFrame();

/** global function to retrieve the vme base type. */
//albaVmeBaseTypes albaGetBaseType(albaVME* vme); //(to be removed Marco)

/** to be used by albaGUIFrame and derived class only */    
ALBA_EXPORT void albaSetFrame(wxWindow* frame);

/** process pending events. be careful not to call it recursively */    
ALBA_EXPORT void albaYield();

/** process pending events. be careful not to call it recursively */
ALBA_EXPORT void albaEnableYielding(bool enable=true);

/** show the Dir Selection Dialog Box */
ALBA_EXPORT wxString  albaGetDirName(const char * initial, const char * title = "Select Directory", wxWindow *parent = NULL);

/** show the File Open Dialog Box */
ALBA_EXPORT wxString  albaGetOpenFile(const char * initial, const char * wildcard, const char * title = "Open File", wxWindow *parent = NULL);

/** show the File Open Dialog Box for multiple file selection */
ALBA_EXPORT void albaGetOpenMultiFiles(const char * path, const char * wildcard, std::vector<wxString> &files, const char * title = "Open Files", wxWindow *parent = NULL);

/** show the File Save Dialog Box */
ALBA_EXPORT wxString  albaGetSaveFile(const char * initial, const char * wildcard, const char * title = "Save File", wxWindow *parent = NULL, bool warnOverWrite=true);

/** Resets the Random Color Selection used for test repeatability */
ALBA_EXPORT void albaResetRandomColor();

/** return a random wxColour from a palette of 16 */
ALBA_EXPORT wxColour  albaRandomColor();

/** Used to generate gray scale bitmap given a colored one.*/
ALBA_EXPORT wxBitmap albaWhiteFade(wxBitmap bmp,double level=0.5);

/** Used to generate red scale bitmap to represent a VME with data not present.*/
ALBA_EXPORT wxBitmap albaBlueScale(wxBitmap bmp);

/** return the application start-up directory */
ALBA_EXPORT wxString  albaGetApplicationDirectory();

/** return the WorkingData directory */
ALBA_EXPORT wxString  albaGetAppDataDirectory();

/** return the Documents directory */
ALBA_EXPORT wxString albaGetDocumentsDirectory();

/** return the Last User directory */
ALBA_EXPORT wxString albaGetLastUserFolder();

/** return the Last User directory */
ALBA_EXPORT void albaSetAppDebugDir(wxString ddir);

/** return the config directory */
ALBA_EXPORT wxString albaGetConfigDirectory();

/** Set the Last User directory */
ALBA_EXPORT void albaSetLastUserFolder(albaString folder);

/** translate an event-id into a readable string */
ALBA_EXPORT wxString  albaIdString(int id);

ALBA_EXPORT float RoundValue(float f_in, int decimal_digits = 2);
ALBA_EXPORT double RoundValue(double d_in, int decimal_digits = 2);

/** Formats the specified size to B, KB, MB or GB */
ALBA_EXPORT void albaFormatDataSize(long long size, albaString& szOut);

ALBA_EXPORT int* GetALBAExpertMode();

#endif
