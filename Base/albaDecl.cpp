/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDecl
 Authors: Silvano Imboden
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "albaDefines.h"

#include "albaDecl.h"
#include <wx/uri.h>
#include "wx/filename.h"

#include "mmuIdFactory.h"
#include <math.h>
#include "wx\stdpaths.h"
#include "wx\msw\registry.h"
#include "albaLogicWithManagers.h"
#include "albaColor.h"
#include "wx/confbase.h"
#include "wx/config.h"


// int ALBAExpertMode = TRUE;

static long GLO_randCol = 117;
static bool GLO_yelding;
static bool GLO_yieldEnabled;
static wxString GLO_appDebugDir;

//----------------------------------------------------------------------------
void albaYield()
//----------------------------------------------------------------------------
{
   if(GLO_yieldEnabled && !GLO_yelding)
   {
       GLO_yelding=true;
       wxYield();
       GLO_yelding=false;
   }
}

//----------------------------------------------------------------------------
ALBA_EXPORT void albaEnableYielding(bool enable/*=true*/)
{

}

//----------------------------------------------------------------------------
wxString  albaGetDirName(const char * initial, const char * title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxDirDialog dialog(parent, title, initial, wxDD_DEFAULT_STYLE | wxDD_NEW_DIR_BUTTON);

  dialog.SetReturnCode(wxID_OK);
  int result = dialog.ShowModal();
  albaYield(); // wait for the dialog to disappear

  albaString newPath = (result == wxID_OK) ? dialog.GetPath() : "";
  albaSetLastUserFolder(newPath);
  return newPath;
}

//----------------------------------------------------------------------------
wxString albaGetOpenFile(const char *initial, const char * wild, const char * title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
	if (wxDirExists(initial))
		path = initial;
	else
		wxFileName::SplitPath(initial,&path,&name,&ext);

	if (name != "" && ext != "") 
		name = name + "." + ext;

  wxString wildcard=wild;
  wildcard+="|All Files (*.*)|*.*";
 
	wxFileDialog dialog(parent, title, path, "", wildcard, wxFD_OPEN|wxFD_FILE_MUST_EXIST);

  dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  albaYield(); // wait for the dialog to disappear

  albaString newPath = (result == wxID_OK) ? dialog.GetPath() : "";
  albaSetLastUserFolder(newPath);
  return newPath;
}

//----------------------------------------------------------------------------
void albaGetOpenMultiFiles(const char * path, const char * wild, std::vector<wxString> &files, const char * title, wxWindow *parent)
//----------------------------------------------------------------------------
{  
  wxString wildcard = wild;
  wildcard += "|All Files (*.*)|*.*";
 
	wxFileDialog dialog(parent, title, path, "", wildcard, wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_MULTIPLE);

  dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  albaYield(); // wait for the dialog to disappear
  wxArrayString wxfiles;
	(result == wxID_OK) ? dialog.GetPaths(wxfiles) : wxfiles.Empty();
	for (int i=0;i<wxfiles.GetCount();i++)
	  files.push_back((char *)wxfiles[i].ToAscii());
	if (wxfiles.GetCount() > 0)
	{
		wxString newPath, name, ext;
		wxFileName::SplitPath(wxfiles[0], &newPath, &name, &ext);
		albaSetLastUserFolder(newPath);
	}
}
//----------------------------------------------------------------------------
wxString albaGetSaveFile(const char * initial, const char * wild, const char * title, wxWindow *parent, bool warnOverWrite)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
	wxString defaultname = "NewFile";
	wxFileName::SplitPath(initial,&path,&name,&ext);
	if (name != "" && ext != "")
	{
		name = name + "." + ext;
		defaultname = name;
	}
  
	wxString wildcard = wild;

  wildcard += "|All Files (*.*)|*.*";
  
	long style = wxFD_SAVE;

	if (warnOverWrite)
		style = style | wxFD_OVERWRITE_PROMPT;
	
	wxFileDialog dialog(parent,title, initial, defaultname, wildcard, style);
	
	dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  albaYield(); // wait for the dialog to disappear

  albaString newPath = (result == wxID_OK) ? dialog.GetPath() : "";
  albaSetLastUserFolder(newPath);
  return newPath;
}
//----------------------------------------------------------------------------
wxString albaGetApplicationDirectory()
//----------------------------------------------------------------------------
{

	if (GLO_appDebugDir.empty())
	{
		wxStandardPaths &std= wxStandardPaths::Get();
		wxString exePath = std.GetDataDir();

		//get current working directory
		wxString cd = wxGetCwd();

		static wxString app_dir;

		wxSetWorkingDirectory(exePath);
		wxSetWorkingDirectory("..");
		app_dir = wxGetCwd();

		//Restore working directory
		wxSetWorkingDirectory(cd);

		return app_dir;
	}
	else
	{
		return GLO_appDebugDir;
	}
}
//----------------------------------------------------------------------------
wxString albaGetAppDataDirectory()
//----------------------------------------------------------------------------
{
  //getting user app directory
	wxStandardPaths &std = wxStandardPaths::Get();
	wxString appData_dir = std.GetUserDataDir();

  return appData_dir.ToAscii();
}
//----------------------------------------------------------------------------
wxString albaGetDocumentsDirectory()
//----------------------------------------------------------------------------
{
  //getting the Documents directory
  wxString home_dir =  wxGetHomeDir();
  home_dir=+"\\Documents";

  return home_dir;
}
//----------------------------------------------------------------------------
wxString albaGetLastUserFolder()
//----------------------------------------------------------------------------
{  
  wxString lastUserFolder;

	wxConfigBase* config = new wxConfig(wxEmptyString);

  // Return last User Folder if exists, else Documents folder
  if(!config->Read("LastUserFolder", &lastUserFolder))
    lastUserFolder = albaGetDocumentsDirectory().ToAscii();

  return lastUserFolder.ToAscii();

	cppDEL(config);
}


//----------------------------------------------------------------------------
ALBA_EXPORT void albaSetAppDebugDir(wxString ddir)
{
	GLO_appDebugDir = ddir;
}

//----------------------------------------------------------------------------
ALBA_EXPORT wxString albaGetConfigDirectory()
{
	//getting the Config directory
	wxString config_dir = albaGetApplicationDirectory().ToAscii();

	//if the debug directory is set we need to add "Installer" to the PATH
	if(!GLO_appDebugDir.empty())
		config_dir += "\\Installer";

	config_dir += "\\Config";

	for (unsigned int i = 0; i < config_dir.Length(); i++)
	{
		if (config_dir[i] == '/')
			config_dir[i] = '\\';
	}

	return config_dir;
}

//----------------------------------------------------------------------------
void albaSetLastUserFolder(albaString folder)
//----------------------------------------------------------------------------
{
  if(folder!="")
  {
    wxString appName = wxApp::GetInstance()->GetAppName();
		wxConfigBase* config = new wxConfig(wxEmptyString);
		

		if (!wxDirExists(folder.GetCStr()))
		{
			wxString path, name, ext;
			wxFileName::SplitPath(folder.GetCStr(), &path, &name, &ext);
			if (wxDirExists(path))
				folder = path;
			else
				return;
		}

    config->Write("LastUserFolder", folder.GetCStr());
    
		cppDEL(config);
  }
}

//----------------------------------------------------------------------------
float RoundValue(float f_in, int decimal_digits)
//----------------------------------------------------------------------------
{
  float f_tmp = f_in * pow((double)10,(double)decimal_digits);
  int b = ( f_tmp >= 0 ) ? static_cast<int>( f_tmp + .5):static_cast<int>( f_tmp - .5);
  return b / pow((double)10,(double)decimal_digits);
}
//----------------------------------------------------------------------------
double RoundValue(double d_in, int decimal_digits)
//----------------------------------------------------------------------------
{
  double d_tmp = d_in * pow((double)10,(double)decimal_digits);
  int b = ( d_tmp >= 0 ) ? static_cast<int>( d_tmp + .5):static_cast<int>( d_tmp - .5);
  return b / pow((double)10,(double)decimal_digits);
}
//----------------------------------------------------------------------------
void albaFormatDataSize( long long size, albaString& szOut )
//----------------------------------------------------------------------------
{
  const char* SZUN[] = {"B", "KB", "MB", "GB", NULL};
  const int LIMITS[] = { 16384, 4096, 1024, INT_MAX};

  int idx = 0;
  double nsize = (double)size;
  while (SZUN[idx] != NULL)
  {
    if (nsize < LIMITS[idx])
      break;

    nsize /= 1024;
    idx++;
  }

  szOut = albaString::Format("%g %s", RoundValue(nsize), SZUN[idx]);
}
//----------------------------------------------------------------------------
wxBitmap albaWhiteFade(wxBitmap bmp,double level)
//----------------------------------------------------------------------------
{
  wxImage img = bmp.ConvertToImage();
  unsigned char *p = img.GetData();
  unsigned char *max = p + img.GetWidth() * img.GetHeight() * 3;
  unsigned char *r, *g, *b;
  unsigned int gray ;
  while( p < max )
  {
    r = p++;
    g = p++;
    b = p++;
		*r = (255-(255-*r)*level);
		*g = (255-(255-*g)*level);
		*b = (255-(255-*b)*level);
  }
  return wxBitmap(img);
}
//----------------------------------------------------------------------------
wxBitmap albaBlueScale(wxBitmap bmp)
//----------------------------------------------------------------------------
{
  wxImage img = bmp.ConvertToImage();
  unsigned char *p = img.GetData();
  unsigned char *max = p + img.GetWidth() * img.GetHeight() * 3;
  unsigned char *r, *g, *b;
	int h,s,l;
	int ir,ig,ib; 
  while( p < max )
  {
    r = p++;
    g = p++;
    b = p++;

		if(*r!=255 && *b!= 255 && *g!=255)
		{
			albaColor::RGBToHSL(*r, *g, *b, &h, &s, &l);
			h=210;
			s=255/2;
			albaColor::HSLToRGB(h, s, l, &ir, &ig, &ib);

			*r=ir;
			*g=ig;
			*b=ib;
		}
  }
  return wxBitmap(img);
}

void albaResetRandomColor()
{
	GLO_randCol = 117;
}
//----------------------------------------------------------------------------
wxColour albaRandomColor()
//----------------------------------------------------------------------------
{
	GLO_randCol = (GLO_randCol*13)%16+1;
  switch( GLO_randCol )
  {
    case 1:  return wxColour(229,166,215); break;
    case 2:  return wxColour(224,86 ,86 ); break;
    case 3:  return wxColour(224,143,87 ); break;
    case 4:  return wxColour(224,198,87 ); break;
    case 5:  return wxColour(198,224,87 ); break;
    case 6:  return wxColour(143,224,87 ); break;
    case 7:  return wxColour(87 ,224,87 ); break;
    case 8:  return wxColour(87 ,224,143); break;
    case 9:  return wxColour(87 ,224,198); break;
    case 10: return wxColour(88 ,199,195); break;
    case 11: return wxColour(87 ,224,198); break;
    case 12: return wxColour(88 ,88 ,225); break;
    case 13: return wxColour(140,88 ,225); break;
    case 14: return wxColour(196,88 ,225); break;
    case 15: return wxColour(225,188,199); break;
    default: return wxColour(224,87 ,143); break;
  };
}
//----------------------------------------------------------------------------
wxString  albaIdString(int id)
//----------------------------------------------------------------------------
{
    wxString s;
    switch(id)
    {
     case MENU_START:           s="MENU_START"; break; 
     case MENU_FILE_START:      s="MENU_FILE_START"; break; 
     case MENU_FILE_NEW:        s="MENU_FILE_NEW"; break; 
     case MENU_FILE_OPEN:       s="MENU_FILE_OPEN"; break; 
     case MENU_FILE_SAVE:       s="MENU_FILE_SAVE"; break; 
     case MENU_FILE_SAVEAS:     s="MENU_FILE_SAVEAS"; break;
     case MENU_FILE_MERGE:      s="MENU_FILE_MERGE"; break; 
     case MENU_FILE_PRINT:      s="MENU_FILE_PRINT"; break;
     case MENU_FILE_PRINT_PREVIEW:s="MENU_FILE_PRINT_PREVIEW"; break;
     case MENU_FILE_PRINT_SETUP:s="MENU_FILE_PRINT_SETUP"; break;
     case MENU_FILE_PRINT_PAGE_SETUP:s="MENU_FILE_PRINT_PAGE_SETUP"; break;
     case MENU_FILE_QUIT:       s="MENU_FILE_QUIT"; break; 
     case MENU_FILE_END:        s="MENU_FILE_END"; break; 
     case MENU_EDIT_START:      s="MENU_EDIT_START"; break; 
     case MENU_OP:              s="MENU_OP"; break; 
     case MENU_OPTION_DEVICE_SETTINGS: s="MENU_OPTION_DEVICE_SETTINGS"; break;
     case MENU_OPTION_LOCALE_SETTINGS: s="MENU_OPTION_LOCALE_SETTINGS"; break;
     //case MENU_EDIT_UNDO:	      s="MENU_EDIT_UNDO"; break; 
     //case MENU_EDIT_REDO:	      s="MENU_EDIT_REDO"; break; 
     //case MENU_EDIT_DELETE:	    s="MENU_EDIT_DELETE"; break; 
     //case MENU_EDIT_CUT:	      s="MENU_EDIT_CUT"; break; 
     //case MENU_EDIT_COPY:	      s="MENU_EDIT_COPY"; break; 
     //case MENU_EDIT_PASTE:	    s="MENU_EDIT_PASTE"; break; 
     case MENU_EDIT_FIND_VME:   s="MENU_EDIT_FIND_VME"; break; 
     case MENU_EDIT_END:	      s="MENU_EDIT_END"; break; 

     case MENU_VIEW_START:	    s="MENU_VIEW_START"; break; 
     case SASH_START:	          s="SASH_START"; break; 
     case MENU_VIEW_TOOLBAR:	  s="MENU_VIEW_TOOLBAR"; break; 
     case MENU_VIEW_LOGBAR:	    s="MENU_VIEW_LOGBAR"; break; 
     case MENU_VIEW_SIDEBAR:	  s="MENU_VIEW_SIDEBAR"; break; 
     case MENU_VIEW_TIMEBAR:	  s="MENU_VIEW_TIMEBAR"; break; 
     case MENU_OPTION_APPLICATION_SETTINGS: s = "MENU_OPTION_APPLICATION_SETTINGS"; break;
     case MENU_OPTION_MEASURE_UNIT_SETTINGS: s = "MENU_OPTION_MEASURE_UNIT_SETTINGS"; break;
     case SASH_END:	            s="SASH_END"; break; 
     case MENU_LAYOUT_START:	  s="MENU_LAYOUT_START"; break; 
     case LAYOUT_ONE:	          s="LAYOUT_ONE"; break; 
     case LAYOUT_TWO_VERT:	    s="LAYOUT_TWO_VERT"; break; 
     case LAYOUT_TWO_HORZ:	    s="LAYOUT_TWO_HORZ"; break; 
     case LAYOUT_THREE_UP:	    s="LAYOUT_THREE_UP"; break; 
     case LAYOUT_THREE_DOWN:	  s="LAYOUT_THREE_DOWN"; break; 
     case LAYOUT_THREE_LEFT:	  s="LAYOUT_THREE_LEFT"; break; 
     case LAYOUT_THREE_RIGHT:	  s="LAYOUT_THREE_RIGHT"; break; 
     case LAYOUT_FOUR:	        s="LAYOUT_FOUR"; break; 
     case LAYOUT_LOAD:	        s="LAYOUT_LOAD"; break; 
     case LAYOUT_SAVE:	        s="LAYOUT_SAVE"; break; 
     case MENU_LAYOUT_END:	    s="MENU_LAYOUT_END"; break; 
     case MENU_VIEW_END:	      s="MENU_VIEW_END"; break; 

     case MENU_SETTINGS_START:	s="MENU_SETTINGS_START"; break; 
     case ID_APP_SETTINGS:	    s="ID_APP_SETTINGS"; break; 
     case ID_TIMEBAR_SETTINGS:	s="ID_TIMEBAR_SETTINGS"; break; 
     case ID_VIEW_SETTINGS:	    s="ID_VIEW_SETTINGS"; break; 
     case MENU_SETTINGS_END:	  s="MENU_SETTINGS_END"; break; 

     case EVT_START:	          s="EVT_START"; break; 
     case UPDATE_UI:	          s="UPDATE_UI"; break; 
     case UPDATE_PROPERTY:      s="UPDATE_PROPERTY"; break; 

     case VME_ADDED:	          s="VME_ADDED"; break; 
     case VME_REMOVING:	        s="VME_REMOVING"; break; 
     case VME_SELECT:	          s="VME_SELECT"; break; 
     case VME_SELECTED:	        s="VME_SELECTED"; break;
     case VME_DCLICKED:         s="VME_DCLICKED"; break;
     case VME_TRANSFORM:	      s="VME_TRANSFORM"; break; 
     case VME_CHOOSE:						s="VME_CHOOSE"; break; 
     case VME_REF_SYS_CHOOSED:  s="VME_REF_SYS_CHOOSED"; break;
     case VME_FEM_DATA_CHOOSED: s="VME_FEM_DATA_CHOOSED"; break;
     case VME_PICKED:	          s="VME_PICKED"; break; 
     case VME_PICKING:	        s="VME_PICKING"; break; 

     case CREATE_STORAGE:       s="CREATE_STORAGE"; break; 

     case VIEW_QUIT:            s="VIEW_QUIT"; break;
     case VIEW_CREATE:          s="VIEW_CREATE"; break; 
     case VIEW_CREATED:         s="VIEW_CREATED"; break; 
     case VIEW_DELETE:          s="VIEW_DELETE"; break; 
     case VIEW_SELECT:          s="VIEW_SELECT"; break; 
     case VIEW_SELECTED:        s="VIEW_SELECTED"; break; 
     case VIEW_CLICKED:         s="VIEW_CLICKED"; break; 
     case VIEW_MAXIMIZE:        s="VIEW_CLICKED"; break; 
     case VIEW_RESIZE:          s="VIEW_RESIZE"; break; 
     case VIEW_SAVE_IMAGE:      s="VIEW_SAVE_IMAGE"; break; 

     case PER_PUSH:             s="PER_PUSH"; break; 
     case PER_POP:              s="PER_POP"; break; 
     case CAMERA_FRONT:	        s="CAMERA_FRONT"; break; 
     case CAMERA_BACK:	        s="CAMERA_BACK"; break; 
     case CAMERA_LEFT:	        s="CAMERA_LEFT"; break; 
     case CAMERA_RIGHT:	        s="CAMERA_RIGHT"; break; 
     case CAMERA_TOP:	          s="CAMERA_TOP"; break; 
     case CAMERA_BOTTOM:	      s="CAMERA_BOTTOM"; break; 
     case CAMERA_PERSPECTIVE:	  s="CAMERA_PERSPECTIVE"; break; 
     case CAMERA_RX_FRONT:	    s="CAMERA_RX_FRONT"; break; 
     case CAMERA_RX_LEFT:	      s="CAMERA_RX_LEFT"; break; 
     case CAMERA_RX_RIGHT:	    s="CAMERA_RX_RIGHT"; break; 
    // modified by Stefano 21-9-2004 (begin)
    case CAMERA_RXFEM_XNEG:     s="CAMERA_RXFEM_XNEG";break;
    case CAMERA_RXFEM_XPOS:     s="CAMERA_RXFEM_XPOS";break;
    case CAMERA_RXFEM_YNEG:     s="CAMERA_RXFEM_YNEG";break;
    case CAMERA_RXFEM_YPOS:     s="CAMERA_RXFEM_YPOS";break;
    case CAMERA_RXFEM_ZNEG:     s="CAMERA_RXFEM_ZNEG";break;
    case CAMERA_RXFEM_ZPOS:     s="CAMERA_RXFEM_ZPOS";break;
    // modified by Stefano 21-9-2004 (end)
		 case CAMERA_BLEND:					s="CAMERA_BLEND";break;
     case CAMERA_CT:						s="CAMERA_CT"; break; 
     case CAMERA_OS_X:	        s="CAMERA_OS_X"; break; 
     case CAMERA_OS_Y:	        s="CAMERA_OS_Y"; break; 
     case CAMERA_OS_Z:	        s="CAMERA_OS_Z"; break; 
     case CAMERA_OS_P:	        s="CAMERA_OS_P"; break; 
     case CAMERA_UPDATE:	      s="CAMERA_UPDATE"; break; 
     case CAMERA_RESET:	        s="CAMERA_RESET"; break; 
		 case CAMERA_FIT:						s="CAMERA_FIT"; break; 
		 case CAMERA_FLYTO:					s="CAMERA_FLYTO"; break; 
     case DEVICE_ADD:           s="DEVICE_ADD"; break; 
     case DEVICE_REMOVE:        s="DEVICE_REMOVE"; break; 
     case DEVICE_GET:           s="DEVICE_GET"; break; 
     case INTERACTOR_ADD:       s="INTERACTOR_ADD"; break; 
     case INTERACTOR_REMOVE:    s="INTERACTOR_REMOVE"; break; 

     case OP_RUN:	              s="OP_RUN"; break; 
     case OP_RUN_OK:	          s="OP_RUN_OK"; break; 
     case OP_RUN_CANCEL:	      s="OP_RUN_CANCEL"; break; 
     case OP_RUN_STARTING:	    s="OP_RUN_STARTING"; break; 
     case OP_RUN_TERMINATED:	  s="OP_RUN_TERMINATED"; break; 
     case OP_SHOW_GUI:	        s="OP_SHOW_GUI"; break; 
     case OP_HIDE_GUI:	        s="OP_HIDE_GUI"; break; 
     case OP_FORCE_STOP:	      s="OP_FORCE_STOP"; break; 

     case EVT_END:	            s="EVT_END"; break; 
     case MENU_END:	            s="MENU_END"; break; 

		 case SHOW_VIEW_CONTEXTUAL_MENU: s = "SHOW_VIEW_CONTEXTUAL_MENU"; break;
		 case SHOW_TREE_CONTEXTUAL_MENU: s = "SHOW_TREE_CONTEXTUAL_MENU"; break;

     case OP_START:							s="OP_START"; break; 
     case OP_UNDO:							s="OP_UNDO"; break; 
     case OP_REDO:							s="OP_REDO"; break; 
     case OP_DELETE:						s="OP_DELETE"; break; 
     case OP_CUT:								s="OP_CUT"; break; 
     case OP_COPY:							s="OP_COPY"; break; 
     case OP_PASTE:							s="OP_PASTE"; break; 
	 case OP_RENAME:						s = "OP_RENAME"; break;
     case OP_SELECT:						s="OP_SELECT"; break; 
     case OP_TRANSFORM:					s="OP_TRANSFORM"; break; 
     case OP_END:								s="OP_END"; break; 

     case WIDGETS_START:	      s="WIDGETS_START"; break; 
     case ID_LAYOUT:	          s="ID_LAYOUT"; break; 
     case ID_CLOSE_SASH:	      s="ID_CLOSE_SASH"; break; 
     case ID_LABEL_CLICK:	      s="ID_LABEL_CLICK"; break; 
     case ID_DISPLAY_LIST:	    s="ID_DISPLAY_LIST"; break; 
     case ID_SETTINGS:					s="ID_SETTINGS"; break; 

     case IMPORT_FILE:          s="IMPORT_FILE"; break;
     case CLEAR_UNDO_STACK:     s="CLEAR_UNDO_STACK"; break;

     case WIDGETS_END:	        s="WIDGETS_END"; break; 
     case PIC_START:	          s="PIC_START"; break; 
     case TIME_SET:	            s="TIME_SET"; break; 
     case TIME_PLAY:	          s="TIME_PLAY"; break; 
     case TIME_STOP:	          s="TIME_STOP"; break; 
     case TIME_PREV:	          s="TIME_PREV"; break; 
     case TIME_NEXT:	          s="TIME_NEXT"; break; 
     case TIME_BEGIN:	          s="TIME_BEGIN"; break; 
     case TIME_END:	            s="TIME_END"; break; 
     case MOVIE_RECORD:         s="MOVIE_RECORD"; break; 
        
     case MCH_START:            s="MCH_START"; break;
     case MCH_UP:               s="MCH_UP"; break;
     case MCH_DOWN:             s="MCH_DOWN"; break;
     case MCH_INPUT:            s="MCH_INPUT"; break;
     case MCH_OUTPUT:           s="MCH_OUTPUT"; break;
     case MCH_NODE:             s="MCH_NODE"; break;
     case MCH_ANY:              s="MCH_ANY"; break;
     case MCH_VTK:              s="MCH_VTK"; break;
     case MCH_END:              s="MCH_END"; break;

     case NODE_START:               s="NODE_START"; break;
     case NODE_DETACHED_FROM_TREE:  s="NODE_DETACHED_FROM_TREE"; break;
     case NODE_ATTACHED_TO_TREE:    s="NODE_ATTACHED_TO_TREE"; break;
     case NODE_DESTROYED:           s="NODE_DESTROYED"; break;
     case NODE_GET_STORAGE:         s="NODE_GET_STORAGE"; break;
     case NODE_GET_ROOT:            s="NODE_GET_ROOT"; break;
     case VME_TIME_SET:             s="VME_TIME_SET"; break;
     case VME_MATRIX_CHANGED:       s="VME_MATRIX_CHANGED"; break;
     case VME_MATRIX_PREUPDATE:     s="VME_MATRIX_PREUPDATE"; break;
     case VME_MATRIX_UPDATE:        s="VME_MATRIX_UPDATE"; break;
     case VME_ABSMATRIX_UPDATE:     s="VME_ABSMATRIX_UPDATE"; break;
     case VME_OUTPUT_DATA_PREUPDATE:s="VME_OUTPUT_DATA_PREUPDATE"; break;
     case VME_OUTPUT_DATA_UPDATE:   s="VME_OUTPUT_DATA_UPDATE"; break;
     case VME_OUTPUT_DATA_CHANGED:  s="VME_OUTPUT_DATA_CHANGED"; break;
     case VME_GET_NEWITEM_ID:       s="VME_GET_NEWITEM_ID"; break;
     case NODE_END:                 s="NODE_END"; break;

     case AVATAR_ADDED:         s="AVATAR_ADDED"; break;
     case AVATAR_REMOVED:       s="AVATAR_REMOVED"; break;

     case MATRIX_UPDATED:       s="MATRIX_UPDATED"; break;

// const used by albaGUI and albaGUIValidator
     case MINID:	              s="MINID"; break; 
     case MAXID:	              s="MAXID"; break; 
     case ID_GUI_UPDATE:	      s="ID_GUI_UPDATE"; break; 
     default:
      s << id;
     break;
    }
    return s.ToAscii();
}
//----------------------------------------------------------------------------
int* GetALBAExpertMode()
//----------------------------------------------------------------------------
{
  static int ALBAExpertMode = TRUE;
  return &ALBAExpertMode;
}
