/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardOperaiontionBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"
#include "albaWizardBlockOperation.h"
#include "albaVME.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
albaWizardBlockOperation::albaWizardBlockOperation(const char *name):albaWizardBlock(name)
//----------------------------------------------------------------------------
{
  //Default constructor
  m_AutoShowSelectedVME=true;
  m_windowhastoberesized = false;
  m_windowhastobetiled = false;
  m_tile_windows = "";
  m_x = 0.;
  m_y = 0.;
  m_width = 0.85;
  m_height = 0.35;
  m_viewtodelete = "";
  m_viewhastobedeleted = false;
}

//----------------------------------------------------------------------------
albaWizardBlockOperation::~albaWizardBlockOperation()
//----------------------------------------------------------------------------
{
  //Clearing the list of to show and to hide VME
  m_VmeShow.clear();
  m_VmeHide.clear();
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::SetRequiredView( const char *View )
//----------------------------------------------------------------------------
{
  //setting required view name 
  m_RequiredView = View;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::SetViewToDelete( const char *View )
	//----------------------------------------------------------------------------
{
  m_viewhastobedeleted = true;
  m_viewtodelete = View;
}

//----------------------------------------------------------------------------
wxString albaWizardBlockOperation::GetRequiredView()
//----------------------------------------------------------------------------
{
  //return the required view
  return m_RequiredView;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::ResizeView(double x, double y, double width, double height)
{
	m_windowhastoberesized = true;
	
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::TileWindows( const char *tile_windows )
{
   m_windowhastobetiled = true;
   m_tile_windows = tile_windows;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::VmeSelect( const char *path )
//----------------------------------------------------------------------------
{
  //Set the path of the vme which was selected before operation start
  m_VmeSelect=path;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::VmeShowAdd( const char *path )
//----------------------------------------------------------------------------
{
  //push back the path of the vme showed before operation start
  wxString wxPath;
  wxPath=path;
  m_VmeShow.push_back(wxPath);
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::VmeHideAdd( const char *path )
//----------------------------------------------------------------------------
{
  //push back the path of the vme hided after operation end
  wxString wxPath;
  wxPath=path;
  m_VmeHide.push_back(wxPath);
}


//----------------------------------------------------------------------------
void albaWizardBlockOperation::ExcutionBegin()
//----------------------------------------------------------------------------
{
  albaString tmpStr;

  albaWizardBlock::ExcutionBegin();

  ///////////////////////
  //Ask Wizard for View
  if (m_RequiredView!="")
  {
    //send up the event in order to open/select the required view
    tmpStr=m_RequiredView;
    albaEventMacro(albaEvent(this,WIZARD_REQUIRED_VIEW,&tmpStr));
	if(m_windowhastoberesized) 
	{
		wxSize albaframesize = albaGetFrame()->GetSize();
		int x = m_x*albaframesize.GetWidth();
		int y = m_y*albaframesize.GetHeight();
		int width = m_width*albaframesize.GetWidth();
		int height = m_height*albaframesize.GetHeight();

	    albaEventMacro(albaEvent(this,VIEW_RESIZE,&tmpStr,x,y,width,height));	
	}
	if(m_windowhastobetiled)
	{
		if(!m_tile_windows.Compare("Tile_window_horizontally")) 
		{
	        albaEventMacro(albaEvent(this,TILE_WINDOW_HORIZONTALLY));
		}
		else if (!m_tile_windows.Compare("Tile_window_vertically")) 
		{
			albaEventMacro(albaEvent(this,TILE_WINDOW_VERTICALLY));
		}
		else if (!m_tile_windows.Compare("Tile_window_cascade"))
		{
			albaEventMacro(albaEvent(this,TILE_WINDOW_CASCADE));
		}
		else 
		{
	      albaLogMessage("The selected tile window modality does not exist!");
		}
	}
  }

  ///////////////////////
  //Select the input VME for the operation
  if (m_SelectedVME)
    m_SelectedVME=m_SelectedVME->GetByPath(m_VmeSelect.ToAscii());
  
  if (m_SelectedVME)
  {
    //forward up vme selection event 
    //for view/logic/operation update
		GetLogicManager()->VmeSelect(m_SelectedVME);
  }
  else 
  {
    //If we cannot select the correct vme we need to abort the wizard
    albaLogMessage("Wizard Error: unable to select VME, path:\"%s\" base:\"%s\"",m_VmeSelect.ToAscii(),m_SelectedVME->GetName());
    Abort();
    //we stop execution now
    return;
  }

  //////////////////////////  
  //Show the required VMEs

  //If there is no view required we don't show any vme 
  if (m_RequiredView != "")
  {
    //Showing input vme to ensure visualization in the operation
    if (m_AutoShowSelectedVME)
			GetLogicManager()->VmeShow(m_SelectedVME, true);

    for(int i=0;i<m_VmeShow.size();i++)
    {
      //detecting all other vme starting on selected vme and show it
      albaVME *toShow=m_SelectedVME->GetByPath(m_VmeShow[i].ToAscii());
      if (toShow != NULL)
        GetLogicManager()->VmeShow(toShow, true);
    }
  }
  
  //////////////////////////  
  //Run Operation
  if (m_Operation!="")
  {
    tmpStr=m_Operation;
    //ask logic for operation run the flow will continue after operation stop
    albaEventMacro(albaEvent(this,WIZARD_RUN_OP,&tmpStr));
  }
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::ExcutionEnd()
//----------------------------------------------------------------------------
{
  albaWizardBlock::ExcutionEnd();
  //////////////////////////  
  //Hide the required VMEs
  for(int i=0;i<m_VmeHide.size();i++)
  {
    albaVME *toHide=m_SelectedVME->GetByPath(m_VmeHide[i]);
    GetLogicManager()->VmeShow(toHide, false);
  }

  if(m_viewhastobedeleted) 
  {
      albaEventMacro(albaEvent(this,WIZARD_DELETE_VIEW,&m_viewtodelete));
  }
 
}

//----------------------------------------------------------------------------
wxString albaWizardBlockOperation::GetRequiredOperation()
//----------------------------------------------------------------------------
{
  //return the name of required operation
  return m_Operation;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::SetRequiredOperation( const char *name )
//----------------------------------------------------------------------------
{
  //set the name of required operation
  m_Operation=name;
}

//----------------------------------------------------------------------------
void albaWizardBlockOperation::SetAutoShowSelectedVME( bool autoShow )
//----------------------------------------------------------------------------
{
  m_AutoShowSelectedVME=autoShow;
}

//----------------------------------------------------------------------------
bool albaWizardBlockOperation::GetAutoShowSelectedVME()
//----------------------------------------------------------------------------
{
  return m_AutoShowSelectedVME;
}




