/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDecl.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-06 10:37:07 $
  Version:   $Revision: 1.15 $
  Authors:   Silvano Imboden
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafDecl.h"
#include "mafIncludeWX.h"
#include "mmuIdFactory.h"
#include <vector>
#include <string>

static bool yelding;
//----------------------------------------------------------------------------
void mafYield()
//----------------------------------------------------------------------------
{
   if(!yelding)
   {
       yelding=true;
       wxYield();
       yelding=false;
   }
}

/* to be removed (Marco)
//----------------------------------------------------------------------------
mafVmeBaseTypes mafGetBaseType(mafVME* vme)
//----------------------------------------------------------------------------
{
  assert(vme);
	     if(vme->GetTagArray()->FindTag("MAF_TOOL_VME") != -1)  return VME_TOOL;
			 if(vme->IsA("mflVMEPointSet")==1)                      return VME_POINTSET;
	else if(vme->IsA("mflVMEWidgetLine") ==1)                       return VME_WIDGET; //SIL. 18-11-2004: 
	else if(vme->IsA("mflVMESurface") ==1)                      return VME_SURFACE;
	else if(vme->IsA("mflVMEImage")   ==1)                      return VME_IMAGE;

  else if(vme->IsA("mflVMEExFieldScalar")     ==1)            return VME_EX_FIELD_SCALAR;//BEZ. 20-7-2004: 
	else if(vme->IsA("mflVMEExFieldVector")     ==1)            return VME_EX_FIELD_VECTOR;//BEZ. 20-7-2004: 
  else if(vme->IsA("mflVMEExFieldProfile")     ==1)           return VME_EX_FIELD_PROFILE;//BEZ. 20-7-2004:
	else if(vme->IsA("mflVMEExField")     ==1)                  return VME_EX_FIELD;       //BEZ. 20-7-2004: 

  else if(vme->IsA("mflVMEGrayVolume")  ==1)                  return VME_GRAY_VOLUME;
	else if(vme->IsA("mflVMEVolume")  ==1)                      return VME_VOLUME;
	else if(vme->IsA("mflVMEGizmo")   ==1)                      return VME_GIZMO;
  else if(vme->IsA("mflVMEExternalData") ==1)                 return VME_EXTERNAL_DATA;
	else if(vme->IsA("mflVMEfem")     ==1)                      return VME_FEM;
	else if(vme->IsA("mflVMEScalar")     ==1)                   return VME_SCALAR;
	else return VME_GENERIC;
}
*/
//----------------------------------------------------------------------------
std::string mafGetOpenFile(const char *initial, const char * wild, const char * title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxSplitPath(initial,&path,&name,&ext);

  if(name != "" && ext != "") name = wxString::Format("%s.%s",name.c_str(),ext.c_str());

  wxString wildcard=wild;
  wildcard+="|All Files (*.*)|*.*";
 
	wxFileDialog dialog(parent, title, path, name, wildcard, wxOPEN|wxFILE_MUST_EXIST|wxHIDE_READONLY|wxMULTIPLE);

  dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  mafYield(); // wait for the dialog to disappear
  return  (result == wxID_OK) ? dialog.GetPath().c_str() : "";
}

//----------------------------------------------------------------------------
void mafGetOpenMultiFiles(const char * initial, const char * wild, std::vector<std::string> &files, const char * title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxSplitPath(initial,&path,&name,&ext);
  if(name != "" && ext != "") name = wxString::Format("%s.%s",name.c_str(),ext.c_str());
  wxString wildcard = wild;
  wildcard += "|All Files (*.*)|*.*";
 
	wxFileDialog dialog(parent, title, path, name, wildcard, wxOPEN|wxFILE_MUST_EXIST|wxHIDE_READONLY|wxMULTIPLE);

  dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  mafYield(); // wait for the dialog to disappear
  wxArrayString wxfiles;
	(result == wxID_OK) ? dialog.GetPaths(wxfiles) : wxfiles.Empty();
	for (int i=0;i<wxfiles.GetCount();i++)
	  files.push_back(wxfiles[i].c_str());
}
//----------------------------------------------------------------------------
std::string mafGetSaveFile(const char * initial, const char * wild, const char * title, wxWindow *parent)
//----------------------------------------------------------------------------
{
  wxString path, name, ext;
  wxSplitPath(initial,&path,&name,&ext);
  if(name != "" && ext != "") name = wxString::Format("%s.%s",name.c_str(),ext.c_str());
  wxString wildcard = wild;
  wildcard += "|All Files (*.*)|*.*";
  wxFileDialog dialog(parent,title, path, name, wildcard, wxSAVE|wxOVERWRITE_PROMPT|wxHIDE_READONLY);
  dialog.SetReturnCode(wxID_OK);
	int result = dialog.ShowModal();
  mafYield(); // wait for the dialog to disappear
  return  (result == wxID_OK) ? dialog.GetPath().c_str() : "";
}
//----------------------------------------------------------------------------
std::string mafGetApplicationDirectory()
//----------------------------------------------------------------------------
{
	static wxString app_dir = "";
	if (app_dir == "")
	{
		wxString cd = wxGetCwd();
		wxSetWorkingDirectory("..");
		app_dir = wxGetCwd();
		wxSetWorkingDirectory(cd);
	}
	return app_dir.c_str();
}
//----------------------------------------------------------------------------
wxColour mafRandomColor()
//----------------------------------------------------------------------------
{
  static long i = 117;
  i = (i*13)%16+1;
  switch( i )
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
 //SIL. 21-3-2005: - perche non ritorna una mafString ?
std::string  mafIdString(int id)
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
     //case MENU_EDIT_UNDO:	      s="MENU_EDIT_UNDO"; break; 
     //case MENU_EDIT_REDO:	      s="MENU_EDIT_REDO"; break; 
     //case MENU_EDIT_DELETE:	    s="MENU_EDIT_DELETE"; break; 
     //case MENU_EDIT_CUT:	      s="MENU_EDIT_CUT"; break; 
     //case MENU_EDIT_COPY:	      s="MENU_EDIT_COPY"; break; 
     //case MENU_EDIT_PASTE:	    s="MENU_EDIT_PASTE"; break; 
     case MENU_EDIT_END:	      s="MENU_EDIT_END"; break; 

     case MENU_VIEW_START:	    s="MENU_VIEW_START"; break; 
     case SASH_START:	          s="SASH_START"; break; 
     case MENU_VIEW_TOOLBAR:	  s="MENU_VIEW_TOOLBAR"; break; 
     case MENU_VIEW_LOGBAR:	    s="MENU_VIEW_LOGBAR"; break; 
     case MENU_VIEW_SIDEBAR:	  s="MENU_VIEW_SIDEBAR"; break; 
     case MENU_VIEW_TIMEBAR:	  s="MENU_VIEW_TIMEBAR"; break; 
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

     case VME_ADD:	            s="VME_ADD"; break; 
     case VME_ADDED:	          s="VME_ADDED"; break; 
     case VME_REMOVE:	          s="VME_REMOVE"; break; 
     case VME_REMOVING:	        s="VME_REMOVING"; break; 
     case VME_SELECT:	          s="VME_SELECT"; break; 
     case VME_SELECTED:	        s="VME_SELECTED"; break; 
     case VME_SHOW:	            s="VME_SHOW"; break; 
     case VME_TRANSFORM:	      s="VME_TRANSFORM"; break; 
     case VME_MODIFIED:	        s="VME_MODIFIED"; break; 
     case VME_CHOOSE:						s="VME_CHOOSE"; break; 
     case VME_REF_SYS_CHOOSED:  s="VME_REF_SYS_CHOOSED"; break;
     case VME_FEM_DATA_CHOOSED: s="VME_FEM_DATA_CHOOSED"; break;
     case VME_PICKED:	          s="VME_PICKED"; break; 
     case VME_PICKING:	        s="VME_PICKING"; break; 

     case VIEW_QUIT:            s="VIEW_QUIT"; break;
     case VIEW_CREATE:          s="VIEW_CREATE"; break; 
     case VIEW_CREATED:         s="VIEW_CREATED"; break; 
     case VIEW_DELETE:          s="VIEW_DELETE"; break; 
     case VIEW_SELECT:          s="VIEW_SELECT"; break; 
     case VIEW_SELECTED:        s="VIEW_SELECTED"; break; 
     case VIEW_CLICKED:         s="VIEW_CLICKED"; break; 
     case VIEW_MAXIMIZE:        s="VIEW_CLICKED"; break; 
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
     case CAMERA_CT:						s="CAMERA_CT"; break; 
     case CAMERA_OS_X:	        s="CAMERA_OS_X"; break; 
     case CAMERA_OS_Y:	        s="CAMERA_OS_Y"; break; 
     case CAMERA_OS_Z:	        s="CAMERA_OS_Z"; break; 
     case CAMERA_OS_P:	        s="CAMERA_OS_P"; break; 
     case CAMERA_UPDATE:	      s="CAMERA_UPDATE"; break; 
     case CAMERA_RESET:	        s="CAMERA_RESET"; break; 
		 case CAMERA_FIT:						s="CAMERA_FIT"; break; 
		 case CAMERA_FLYTO:					s="CAMERA_FLYTO"; break; 

     case OP_RUN:	              s="OP_RUN"; break; 
     case OP_RUN_OK:	          s="OP_RUN_OK"; break; 
     case OP_RUN_CANCEL:	      s="OP_RUN_CANCEL"; break; 
     case OP_RUN_STARTING:	    s="OP_RUN_STARTING"; break; 
     case OP_RUN_TERMINATED:	  s="OP_RUN_TERMINATED"; break; 
     case OP_SHOW_GUI:	        s="OP_SHOW_GUI"; break; 
     case OP_HIDE_GUI:	        s="OP_HIDE_GUI"; break; 
     case OP_FORCE_STOP:	      s="OP_FORCE_STOP"; break; 
     case REMOTE_PARAMETER:     s="REMOTE_PARAMETER"; break; 

     case EVT_END:	            s="EVT_END"; break; 
     case MENU_END:	            s="MENU_END"; break; 

		 case SHOW_CONTEXTUAL_MENU: s="SHOW_CONTEXTUAL_MENU"; break;

     case OP_START:							s="OP_START"; break; 
     case OP_UNDO:							s="OP_UNDO"; break; 
     case OP_REDO:							s="OP_REDO"; break; 
     case OP_DELETE:						s="OP_DELETE"; break; 
     case OP_CUT:								s="OP_CUT"; break; 
     case OP_COPY:							s="OP_COPY"; break; 
     case OP_PASTE:							s="OP_PASTE"; break; 
     case OP_SELECT:						s="OP_SELECT"; break; 
     case OP_TRANSFORM:					s="OP_TRANSFORM"; break; 
     case OP_END:								s="OP_END"; break; 

     case WIDGETS_START:	      s="WIDGETS_START"; break; 
     case ID_LAYOUT:	          s="ID_LAYOUT"; break; 
     case ID_CLOSE_SASH:	      s="ID_CLOSE_SASH"; break; 
     case ID_LABEL_CLICK:	      s="ID_LABEL_CLICK"; break; 
     case ID_DISPLAY_LIST:	    s="ID_DISPLAY_LIST"; break; 
     case ID_SETTINGS:					s="ID_SETTINGS"; break; 

     case WIDGETS_END:	        s="WIDGETS_END"; break; 
     case PIC_START:	          s="PIC_START"; break; 
     case TIME_SET:	            s="TIME_SET"; break; 
     case TIME_PLAY:	          s="TIME_PLAY"; break; 
     case TIME_STOP:	          s="TIME_STOP"; break; 
     case TIME_PREV:	          s="TIME_PREV"; break; 
     case TIME_NEXT:	          s="TIME_NEXT"; break; 
     case TIME_BEGIN:	          s="TIME_BEGIN"; break; 
     case TIME_END:	            s="TIME_END"; break; 
        
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

// const used by mmgGui and mmgGuiValidator
     case MINID:	              s="MINID"; break; 
     case MAXID:	              s="MAXID"; break; 
     case ID_GUI_UPDATE:	      s="ID_GUI_UPDATE"; break; 
     default:
      s << id;
     break;
    }
    return s.c_str(); 
}
