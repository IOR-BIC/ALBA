/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgFlyPoseList.cpp,v $
  Language:  C++
  Date:      $Date: 2006-02-21 16:14:11 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmgFlyPoseList.h"

#include "mafDecl.h"
#include "mmgGui.h"
#include "mmgButton.h"
#include "mafOsgTerrainManipulator.h"

//----------------------------------------------------------------------------
mmgFlyPoseList::mmgFlyPoseList( mafOsgTerrainManipulator *manipulator)
//----------------------------------------------------------------------------
{
  m_Listener = NULL; 
  m_manip = manipulator; 	

  m_SelectedPosition	= "";
	m_AnimateFlag		= 1;
	m_PositionList			    = NULL;
	m_StorePositionButton		= NULL;
  m_RenamePositionButton	= NULL;
  m_DeletePositionButton	= NULL;
  m_AnimationLenght       = m_manip->GetFlyingDuration();
	
	CreateGui();
}
//----------------------------------------------------------------------------
mmgFlyPoseList::~mmgFlyPoseList() 
//----------------------------------------------------------------------------
{
  ResetKit();
}
//----------------------------------------------------------------------------
//constants:
//----------------------------------------------------------------------------
enum mmgFlyPoseListIDs
{
	ID_LIST = MINID,
	ID_STORE,
	ID_ANIMATE,
	ID_DELETE,
	ID_RENAME,
  ID_ANIMLENGHT,
};
//----------------------------------------------------------------------------
void mmgFlyPoseList::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new mmgGui(this);
	m_Gui->Show(true);

  int rm = m_Gui->GetMetrics(GUI_ROW_MARGIN);
  int dw = m_Gui->GetMetrics(GUI_DATA_WIDTH);
  int lw = m_Gui->GetMetrics(GUI_LABEL_WIDTH);
  int lm = m_Gui->GetMetrics(GUI_LABEL_MARGIN);
  int wm = m_Gui->GetMetrics(GUI_WIDGET_MARGIN);
  int bh = m_Gui->GetMetrics(GUI_BUTTON_HEIGHT);
  wxSize bs(dw/3-wm,bh);
	wxPoint dp = wxDefaultPosition;

	wxStaticText *lab = new wxStaticText(m_Gui,-1,"fly pose",dp,wxSize(lw,bh),wxALIGN_RIGHT |wxST_NO_AUTORESIZE );
  lab->SetFont(m_Gui->GetBoldFont());

	m_StorePositionButton  = new mmgButton (m_Gui,ID_STORE, "store", dp,bs);
  m_RenamePositionButton = new mmgButton (m_Gui,ID_RENAME,"rename",dp,bs);
  m_DeletePositionButton = new mmgButton (m_Gui,ID_DELETE,"delete",dp,bs);
	m_StorePositionButton->SetListener(this);
	m_RenamePositionButton->SetListener(this);
	m_DeletePositionButton->SetListener(this);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab,      0, wxRIGHT, lm);
	sizer->Add(m_StorePositionButton,  0, wxRIGHT, wm);
	sizer->Add(m_RenamePositionButton, 0, wxRIGHT, wm);
	sizer->Add(m_DeletePositionButton, 0, wxRIGHT, wm);
  m_Gui->Add(sizer,0,wxALL,rm); 

	m_PositionList = m_Gui->ListBox(ID_LIST," ",100);
  m_Gui->Double(ID_ANIMLENGHT,"anim.lenght",&m_AnimationLenght);
	m_Gui->Bool(ID_ANIMATE,"animate",&m_AnimateFlag);

	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmgFlyPoseList::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_STORE:
        StoreViewPoint();
        EnableWidgets();
      break;
      case ID_DELETE:
        DeleteViewPoint();
        EnableWidgets();
      break;
      case ID_RENAME:
        RenameViewPoint();
      break;
      case ID_ANIMLENGHT:
        m_manip->SetFlyingDuration(m_AnimationLenght);
      break;
      case ID_LIST:
        FlyTo();
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mmgFlyPoseList::EnableWidgets()
//----------------------------------------------------------------------------
{
	m_StorePositionButton->Enable( true ); 
	m_PositionList->Enable(  m_PositionList->Number()>0 );
	m_DeletePositionButton->Enable( m_PositionList->Number()>0 && m_SelectedPosition != "" );
	m_RenamePositionButton->Enable( m_PositionList->Number()>0 && m_SelectedPosition != "" );
	m_Gui->Enable( ID_ANIMATE, m_PositionList->Number()>0 );
}
//----------------------------------------------------------------------------
void mmgFlyPoseList::FlyTo()
//----------------------------------------------------------------------------
{
  assert(m_PositionList);

  int n = m_PositionList->GetSelection();
  ListElement *le = (ListElement *)m_PositionList->GetClientData(n);
  assert(le);
  
  if(m_AnimateFlag)
     m_manip->FlyToXY_YPD(le->pos[0], le->pos[1], le->yaw, le->pitch, le->dist);
  else
  {
     m_manip->SetCenter(le->pos);
     m_manip->SetYaw(le->yaw);
     m_manip->SetPitch(le->pitch);
     m_manip->SetDistance(le->dist);
  }
}
//----------------------------------------------------------------------------
void mmgFlyPoseList::StoreViewPoint()
//----------------------------------------------------------------------------
{
	assert(m_PositionList);

	// create a default unique name -----------------------
	int counter = 1;
  wxString name = "camera position 1";
	while(m_PositionList->FindString(name) != -1 && counter < 50)
		name = wxString::Format("camera position %d", ++counter);

  /*
	// prompt user for a name -----------------------
	wxTextEntryDialog *dlg = new wxTextEntryDialog(NULL,"please enter a name", "Store Camera Position", name );
	int result = dlg->ShowModal(); 
	name = dlg->GetValue();
	cppDEL(dlg);
	if(result != wxID_OK) return;
	
	// test if it is unique -----------------------
  counter = 1;
  wxString tmp = name; 
  while(m_PositionList->FindString(tmp) != -1 && counter < 50)
    tmp = wxString::Format("%s %d", name, ++counter);
  name = tmp;
  */

  // we have the new name -----------------------
	m_SelectedPosition = name;

  ListElement *le = new ListElement;
  m_manip->GetCenter( le->pos );
  le->yaw   = m_manip->GetYaw();
  le->pitch = m_manip->GetPitch();
  le->dist  = m_manip->GetDistance();

  m_PositionList->Append(m_SelectedPosition,le);
	m_PositionList->SetSelection(m_PositionList->Number() - 1);
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmgFlyPoseList::RenameViewPoint()
//----------------------------------------------------------------------------
{
	assert(m_SelectedPosition != "");

	// prompt user for the new name -----------------------
	wxTextEntryDialog *dlg = new wxTextEntryDialog(NULL,"please enter a name", "Rename Camera Position",m_SelectedPosition );
  int result = dlg->ShowModal(); 
  wxString name = dlg->GetValue();
  cppDEL(dlg);
	if(result != wxID_OK) return;

  if(name == m_SelectedPosition) return; // user doesn't give a different name, skip

  // test if is it unique -----------------------
	wxString flyto_tagName = "FLY_TO_" + name;
  if(m_PositionList->FindString(name) != -1)
	{
		wxString msg = "this name is already used, choose a different one";
		int res = wxMessageBox(msg,"Rename Camera Position", wxOK, NULL);
		return;
	}

	int n = m_PositionList->GetSelection();
	if(n >= 0) 
    m_PositionList->SetString(n, name);

  m_SelectedPosition = name;
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmgFlyPoseList::DeleteViewPoint()
//----------------------------------------------------------------------------
{
	assert(m_SelectedPosition != "");

	int n = m_PositionList->GetSelection();
	ListElement *le = (ListElement *)m_PositionList->GetClientData(n);
  delete le;
  m_PositionList->Delete(n);

	m_SelectedPosition = "";
	if(m_PositionList->Number()) 
	{
		m_PositionList->SetSelection(0);
	  m_SelectedPosition = m_PositionList->GetStringSelection();
	}
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmgFlyPoseList::ResetKit()
//----------------------------------------------------------------------------
{
  int num_items = m_PositionList->Number();
  for (int i=0; i<num_items;i++)
  {
    ListElement *le = (ListElement *)m_PositionList->GetClientData(0);
    delete le;
    m_PositionList->Delete(0);
  }
  EnableWidgets(); // disable all
  m_Gui->Update();
}

