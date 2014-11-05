/*=========================================================================

 Program: MAF2Medical
 Module: medWizardOperaiontionBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
#include "medWizardBlockOperation.h"
#include "mafNode.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
medWizardBlockOperation::medWizardBlockOperation(const char *name):medWizardBlock(name)
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
medWizardBlockOperation::~medWizardBlockOperation()
//----------------------------------------------------------------------------
{
  //Clearing the list of to show and to hide VME
  m_VmeShow.clear();
  m_VmeHide.clear();
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::SetRequiredView( const char *View )
//----------------------------------------------------------------------------
{
  //setting required view name 
  m_RequiredView = View;
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::SetViewToDelete( const char *View )
	//----------------------------------------------------------------------------
{
  m_viewhastobedeleted = true;
  m_viewtodelete = View;
}

//----------------------------------------------------------------------------
wxString medWizardBlockOperation::GetRequiredView()
//----------------------------------------------------------------------------
{
  //return the required view
  return m_RequiredView;
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::ResizeView(double x, double y, double width, double height)
{
	m_windowhastoberesized = true;
	
	m_x = x;
	m_y = y;
	m_width = width;
	m_height = height;
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::TileWindows( const char *tile_windows )
{
   m_windowhastobetiled = true;
   m_tile_windows = tile_windows;
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::VmeSelect( const char *path )
//----------------------------------------------------------------------------
{
  //Set the path of the vme which was selected before operation start
  m_VmeSelect=path;
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::VmeShowAdd( const char *path )
//----------------------------------------------------------------------------
{
  //push back the path of the vme showed before operation start
  wxString wxPath;
  wxPath=path;
  m_VmeShow.push_back(wxPath);
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::VmeHideAdd( const char *path )
//----------------------------------------------------------------------------
{
  //push back the path of the vme hided after operation end
  wxString wxPath;
  wxPath=path;
  m_VmeHide.push_back(wxPath);
}


//----------------------------------------------------------------------------
void medWizardBlockOperation::ExcutionBegin()
//----------------------------------------------------------------------------
{
  mafString tmpStr;

  medWizardBlock::ExcutionBegin();

  ///////////////////////
  //Ask Wizard for View
  if (m_RequiredView!="")
  {
    //send up the event in order to open/select the required view
    tmpStr=m_RequiredView;
    mafEventMacro(mafEvent(this,WIZARD_REQUIRED_VIEW,&tmpStr));
	if(m_windowhastoberesized) 
	{
		wxSize mafframesize = mafGetFrame()->GetSize();
		int x = m_x*mafframesize.GetWidth();
		int y = m_y*mafframesize.GetHeight();
		int width = m_width*mafframesize.GetWidth();
		int height = m_height*mafframesize.GetHeight();

	    mafEventMacro(mafEvent(this,VIEW_RESIZE,&tmpStr,x,y,width,height));	
	}
	if(m_windowhastobetiled)
	{
		if(!m_tile_windows.Compare("Tile_window_horizontally")) 
		{
	        mafEventMacro(mafEvent(this,TILE_WINDOW_HORIZONTALLY));
		}
		else if (!m_tile_windows.Compare("Tile_window_vertically")) 
		{
			mafEventMacro(mafEvent(this,TILE_WINDOW_VERTICALLY));
		}
		else if (!m_tile_windows.Compare("Tile_window_cascade"))
		{
			mafEventMacro(mafEvent(this,TILE_WINDOW_CASCADE));
		}
		else 
		{
	      mafLogMessage("The selected tile window modality does not exist!");
		}
	}
  }

  ///////////////////////
  //Select the input VME for the operation
  if (m_SelectedVME)
    m_SelectedVME=m_SelectedVME->GetByPath(m_VmeSelect.c_str());
  
  if (m_SelectedVME)
  {
    //forward up vme selection event 
    //for view/logic/operation update
    mafEventMacro(mafEvent(this,VME_SELECT,m_SelectedVME));
  }
  else 
  {
    //If we cannot select the correct vme we need to abort the wizard
    mafLogMessage("Wizard Error: unable to select VME, path:\"%s\" base:\"%s\"",m_VmeSelect.c_str(),m_SelectedVME->GetName());
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
      mafEventMacro(mafEvent(this,VME_SHOW,m_SelectedVME,true));

    for(int i=0;i<m_VmeShow.size();i++)
    {
      //detecting all other vme starting on selected vme and show it
      mafNode *toShow=m_SelectedVME->GetByPath(m_VmeShow[i].c_str());
      if (toShow != NULL)
        mafEventMacro(mafEvent(this,VME_SHOW,toShow,true));
    }
  }
  
  //////////////////////////  
  //Run Operation
  if (m_Operation!="")
  {
    tmpStr=m_Operation;
    //ask logic for operation run the flow will continue after operation stop
    mafEventMacro(mafEvent(this,WIZARD_RUN_OP,&tmpStr));
  }
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::ExcutionEnd()
//----------------------------------------------------------------------------
{
  medWizardBlock::ExcutionEnd();
  //////////////////////////  
  //Hide the required VMEs
  for(int i=0;i<m_VmeHide.size();i++)
  {
    mafNode *toHide=m_SelectedVME->GetByPath(m_VmeHide[i]);
    mafEventMacro(mafEvent(this,VME_SHOW,toHide,false));
  }

  if(m_viewhastobedeleted) 
  {
      mafEventMacro(mafEvent(this,WIZARD_DELETE_VIEW,&m_viewtodelete));
  }
 
}

//----------------------------------------------------------------------------
wxString medWizardBlockOperation::GetRequiredOperation()
//----------------------------------------------------------------------------
{
  //return the name of required operation
  return m_Operation;
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::SetRequiredOperation( const const char *name )
//----------------------------------------------------------------------------
{
  //set the name of required operation
  m_Operation=name;
}

//----------------------------------------------------------------------------
void medWizardBlockOperation::SetAutoShowSelectedVME( bool autoShow )
//----------------------------------------------------------------------------
{
  m_AutoShowSelectedVME=autoShow;
}

//----------------------------------------------------------------------------
bool medWizardBlockOperation::GetAutoShowSelectedVME()
//----------------------------------------------------------------------------
{
  return m_AutoShowSelectedVME;
}




