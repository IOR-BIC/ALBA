/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAnimate
 Authors: Paolo Quadrani
 
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

#include "albaAnimate.h"

#include "mmuIdFactory.h"
#include "albaDecl.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaGUIButton.h"
#include "albaGUIMovieCtrl.h"

#include "albaVME.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaXMLStorage.h"

#include "vtkMath.h"
#include "vtkCamera.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"


//----------------------------------------------------------------------------
albaAnimate::albaAnimate(vtkRenderer *renderer, albaVME *vme, albaObserver *listener)
//----------------------------------------------------------------------------
{
	m_Listener	= listener;
	m_Renderer	= renderer;
  m_Tags			= NULL;
  m_StoredPositions = NULL;

  m_SelectedPosition	= "";
	m_InterpolateFlag		= 1;
	m_PositionList			    = NULL;
	m_StorePositionButton		= NULL;
  m_RenamePositionButton	= NULL;
  m_DeletePositionButton	= NULL;
  m_AnimatePlayer         = NULL;
  m_Root = NULL;

	CreateGui();
	SetInputVME(vme); //widgets must already exist
}
//----------------------------------------------------------------------------
albaAnimate::~albaAnimate() 
//----------------------------------------------------------------------------
{
  albaDEL(m_StoredPositions);
}
//----------------------------------------------------------------------------
void albaAnimate::SetInputVME(albaVME *vme) 
//----------------------------------------------------------------------------
{
  assert(m_Gui && m_PositionList && m_StorePositionButton && m_RenamePositionButton && m_DeletePositionButton);

  //ResetKit();

  if(!vme) return;
  m_Root = vme->GetRoot();
	if(!m_Root) return;
  m_Tags = m_Root->GetTagArray();
	
  RetrieveStoredPositions();
  EnableWidgets(); 
}
//----------------------------------------------------------------------------
//constants:
//----------------------------------------------------------------------------
enum ANIMATE_KIT_WIDGET_ID
{
	ID_LIST = MINID,
	ID_STORE,
	ID_INTERPOLATE,
	ID_DELETE,
	ID_RENAME,
  ID_ANIMATE,
  ID_REFRESH,
  ID_IMPORT,
  ID_EXPORT
};
//----------------------------------------------------------------------------
void albaAnimate::CreateGui()
//----------------------------------------------------------------------------
{
	m_Gui = new albaGUI(this);
	m_Gui->Show(true);

  int rm = m_Gui->GetMetrics(GUI_ROW_MARGIN);
  int dw = m_Gui->GetMetrics(GUI_DATA_WIDTH);
  int lw = m_Gui->GetMetrics(GUI_LABEL_WIDTH);
  int lm = m_Gui->GetMetrics(GUI_LABEL_MARGIN);
  int wm = m_Gui->GetMetrics(GUI_WIDGET_MARGIN);
  int bh = m_Gui->GetMetrics(GUI_BUTTON_HEIGHT);
  wxSize bs(dw/2-wm,bh);
	wxPoint dp = wxDefaultPosition;

	wxStaticText *lab = new wxStaticText(m_Gui,-1,"Fly pose",dp,wxSize(lw,bh));

	m_StorePositionButton  = new albaGUIButton (m_Gui,ID_STORE, "Add", dp,bs);
  m_DeletePositionButton = new albaGUIButton (m_Gui,ID_DELETE,"Remove",dp,bs);
  
  m_StorePositionButton->SetListener(this);
	m_DeletePositionButton->SetListener(this);

	wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(lab,      0, wxRIGHT, lm);
	sizer->Add(m_StorePositionButton,   0, wxRIGHT, wm);
	sizer->Add(m_DeletePositionButton,  0, wxRIGHT, wm);
  m_Gui->Add(sizer,0,wxALL,rm); 

	m_PositionList = m_Gui->ListBox(ID_LIST," ");

  wxStaticText *labRefresh = new wxStaticText(m_Gui,-1,"        ",dp,wxSize(lw,bh));
  m_RefreshPositionButton = new albaGUIButton (m_Gui,ID_REFRESH,"Refresh",dp,bs);
  m_RenamePositionButton = new albaGUIButton (m_Gui,ID_RENAME,"Rename",dp,bs);


  m_RefreshPositionButton->SetListener(this);
  m_RenamePositionButton->SetListener(this);

  wxBoxSizer *sizerRefresh = new wxBoxSizer(wxHORIZONTAL);
  sizerRefresh->Add(labRefresh,      0, wxRIGHT, lm);
  sizerRefresh->Add(m_RefreshPositionButton,  0, wxRIGHT, wm);
  sizerRefresh->Add(m_RenamePositionButton,  0, wxRIGHT, wm);
  m_Gui->Add(sizerRefresh,0,wxALL,rm); 


  wxStaticText *labImport = new wxStaticText(m_Gui,-1,"        ",dp,wxSize(lw,bh));
  m_ImportPositionButton = new albaGUIButton (m_Gui,ID_IMPORT,"Import",dp,bs);
  m_ExportPositionButton = new albaGUIButton (m_Gui,ID_EXPORT,"Export",dp,bs);

  m_ImportPositionButton->SetListener(this);
  m_ExportPositionButton->SetListener(this);

  wxBoxSizer *sizerImport = new wxBoxSizer(wxHORIZONTAL);
  sizerImport->Add(labImport,      0, wxRIGHT, lm);
  sizerImport->Add(m_ImportPositionButton,  0, wxRIGHT, wm);
  sizerImport->Add(m_ExportPositionButton,  0, wxRIGHT, wm);
  m_Gui->Add(sizerImport,0,wxALL,rm); 

	m_Gui->Bool(ID_INTERPOLATE,_("Interpolate"),&m_InterpolateFlag, 1);
  
  m_AnimatePlayer = new albaGUIMovieCtrl(m_Gui);
  m_AnimatePlayer->SetListener(this);
  m_Gui->Add(m_AnimatePlayer);

  EnableWidgets();

	m_Gui->Divider();
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaAnimate::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    wxString wildcard = "xml file (*.xml)|*.xml|all files (*.*)|*.*";
    albaString fileName = "";
    wxString lastFolder = albaGetLastUserFolder().char_str();

    switch(e->GetId())
    {
      case ID_STORE:
      case MOVIE_RECORD:
        StoreViewPoint();
        RetrieveStoredPositions(false);
        EnableWidgets();
      break;
      case ID_DELETE:
        DeleteViewPoint(m_PositionList->GetSelection());
        RetrieveStoredPositions(false);
        EnableWidgets();
      break;
      case ID_RENAME:
        RenameViewPoint();
        RetrieveStoredPositions(false);
      break;
      case ID_LIST:
        FlyTo();
        EnableWidgets();
      break;
      case ID_REFRESH:
        m_PositionList->Clear();
        RetrieveStoredPositions();
        EnableWidgets();
        break;
      case ID_IMPORT:
        fileName = albaGetOpenFile(lastFolder, wildcard);
        if (fileName != "")
        {
          LoadPoseFromFile(fileName);
          EnableWidgets();
        }
      break;
      case ID_EXPORT:
        fileName = albaGetSaveFile(lastFolder, wildcard);
        if (fileName != "")
          StorePoseToFile(fileName);
      break;
      case TIME_SET:
        SetCurrentSelection((int)e->GetDouble());
        FlyTo();
      break;
    }
  }
}
//----------------------------------------------------------------------------
void albaAnimate::EnableWidgets()
//----------------------------------------------------------------------------
{
  m_StorePositionButton->Enable( m_Tags != NULL ); 
  m_PositionList->Enable( m_Tags != NULL && m_PositionList->GetCount()>0 );
  m_PositionList->Enable( m_Tags != NULL && m_PositionList->GetCount()>0 );
  m_DeletePositionButton->Enable( m_Tags != NULL && m_PositionList->GetCount()>0 && m_SelectedPosition != "" );
  m_RenamePositionButton->Enable( m_Tags != NULL && m_PositionList->GetCount()>0 && m_SelectedPosition != "" );
  m_Gui->Enable( ID_INTERPOLATE, m_Tags != NULL && m_PositionList->GetCount()>0 );
  m_AnimatePlayer->Enable(m_PositionList->GetCount()>1);
  m_AnimatePlayer->SetFrameBounds(0,m_PositionList->GetCount()-1);
}
//----------------------------------------------------------------------------
void albaAnimate::LoadPoseFromFile(albaString &fileName)
//----------------------------------------------------------------------------
{
  albaTagArray *newCam = new albaTagArray();

  // XML storage to restore
  albaXMLStorage restore;
  restore.SetURL(fileName.GetCStr());
  restore.SetFileType("CAM");
  restore.SetVersion("1.0");
  restore.SetDocument(newCam);
  restore.Restore();

  SetStoredPositions(newCam);
 
  cppDEL(newCam);
}
//----------------------------------------------------------------------------
void albaAnimate::StorePoseToFile(albaString &fileName)
//----------------------------------------------------------------------------
{	
  int i = 0;
  if(m_StoredPositions->GetNumberOfTags() == 0) return;

  // XML storage to restore
  albaXMLStorage restore;
  restore.SetURL(fileName.GetCStr());
  restore.SetFileType("CAM");
  restore.SetVersion("1.0");
  restore.SetDocument(m_StoredPositions);
  restore.Store();
}
//----------------------------------------------------------------------------
void albaAnimate::FlyTo(const char *fly_position)
//----------------------------------------------------------------------------
{
  wxString fly_pos(fly_position);
  int pos = m_PositionList->FindString(fly_pos);
  if (pos != -1)
  {
    m_PositionList->Select(pos);
    FlyTo();
  }
}
//----------------------------------------------------------------------------
void albaAnimate::FlyTo()
//----------------------------------------------------------------------------
{
  assert(m_Tags && m_Renderer && m_PositionList);
  m_SelectedPosition = m_PositionList->GetStringSelection();
  wxString flyto_tagName = "FLY_TO_" + m_SelectedPosition;
  albaTagItem *item = m_Tags->GetTag(flyto_tagName.char_str());
  if(item == NULL)
    item = m_Tags->GetTag(m_SelectedPosition.char_str()); // support old style

  vtkCamera *camera = m_Renderer->GetActiveCamera();

  double fly0[10]; // from
  double fly1[10]; // to
  double fly [10]; // interpolated position

  fly0[0] = camera->GetFocalPoint()[0];
  fly0[1] = camera->GetFocalPoint()[1];
  fly0[2] = camera->GetFocalPoint()[2];
  fly0[3] = camera->GetPosition()[0];
  fly0[4] = camera->GetPosition()[1];
  fly0[5] = camera->GetPosition()[2];
  fly0[6] = camera->GetViewUp()[0];
  fly0[7] = camera->GetViewUp()[1];
  fly0[8] = camera->GetViewUp()[2];
  fly0[9] = camera->GetParallelScale();

  fly1[0] = item->GetComponentAsDouble(0);
  fly1[1] = item->GetComponentAsDouble(1);
  fly1[2] = item->GetComponentAsDouble(2);
  fly1[3] = item->GetComponentAsDouble(3);
  fly1[4] = item->GetComponentAsDouble(4);
  fly1[5] = item->GetComponentAsDouble(5);
  fly1[6] = item->GetComponentAsDouble(6);
  fly1[7] = item->GetComponentAsDouble(7);
  fly1[8] = item->GetComponentAsDouble(8);
  fly1[9] =(item->GetNumberOfComponents() == 10) ? item->GetComponentAsDouble(9) : fly0[9];

  int numSteps = 30;
  int rate = 15;
  double pi = vtkMath::Pi();

  if(m_InterpolateFlag)
  {
    for (int i = 0; i <= numSteps; i++)
    {
      double t  = ( i * 1.0 ) / numSteps;
      double t2 = 0.5 + 0.5 * sin( t*pi - pi/2 );
      //wxLogMessage("t=%g; \t t2=%g",t,t2);

      for(int j = 0; j < 10 ; j ++ ) 
      {
        fly[j] = (1-t2) * fly0[j] + t2 * fly1[j];
      }

      camera->SetFocalPoint(fly[0],fly[1],fly[2]);
      camera->SetPosition(fly[3],fly[4],fly[5]);
      camera->SetViewUp(fly[6],fly[7],fly[8]);
      camera->SetParallelScale(fly[9]);

			GetLogicManager()->CameraUpdate();
    }
  }
  else
  {
    camera->SetFocalPoint(fly1[0],fly1[1],fly1[2]);
    camera->SetPosition(fly1[3],fly1[4],fly1[5]);
    camera->SetViewUp(fly1[6],fly1[7],fly1[8]);
    camera->SetParallelScale(fly1[9]);
  }
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaAnimate::StoreViewPoint()
//----------------------------------------------------------------------------
{
	assert(m_Tags && m_PositionList);

	// create a default unique name -----------------------
	int counter = 1;
  wxString name = "camera position 1";
	while(m_PositionList->FindString(name) != -1 && counter < 50)
		name = wxString::Format("camera position %d", ++counter);

	// prompt user for a name -----------------------
	wxTextEntryDialog *dlg = new wxTextEntryDialog(NULL,"please enter a name", "Store Camera Position", name );
	int result = dlg->ShowModal(); 
	name = dlg->GetValue();
	cppDEL(dlg);
	if(result != wxID_OK) return;
	
	// test if is it unique -----------------------
  wxString flyto_tagName = "FLY_TO_" + name;
	if(m_Tags->IsTagPresent(flyto_tagName.char_str()))
	{
		wxString msg = "this name is already used, do you want to overwrite it ?";
		int res = wxMessageBox(msg,"Store Camera Position", wxOK|wxCANCEL|wxICON_QUESTION, NULL);
		if(res == wxCANCEL) return;

    //remove item to be overwritten
		m_Tags->DeleteTag(flyto_tagName.char_str());
		m_PositionList->Delete(m_PositionList->FindString(name));
	}
  
	// we have the new name -----------------------
	m_SelectedPosition = name;

	vtkCamera *camera = m_Renderer->GetActiveCamera();
	double fp[3],cam_pos[3],view_up[3], par_scale;
	camera->GetFocalPoint(fp);
	camera->GetPosition(cam_pos);
	camera->GetViewUp(view_up);
  par_scale = camera->GetParallelScale();
	
	albaTagItem item;
	item.SetName(flyto_tagName.char_str());
	item.SetNumberOfComponents(10);
	item.SetComponent(fp[0],0);
	item.SetComponent(fp[1],1);
	item.SetComponent(fp[2],2);
	item.SetComponent(cam_pos[0],3);
	item.SetComponent(cam_pos[1],4);
	item.SetComponent(cam_pos[2],5);
	item.SetComponent(view_up[0],6);
	item.SetComponent(view_up[1],7);
	item.SetComponent(view_up[2],8);
  item.SetComponent(par_scale,9);
	m_Tags->SetTag(item);

	m_PositionList->Append(m_SelectedPosition);
  SetCurrentSelection(m_PositionList->GetCount() - 1);
}
//----------------------------------------------------------------------------
void albaAnimate::RenameViewPoint()
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
  if(m_Tags->IsTagPresent(flyto_tagName.char_str()))
	{
		wxString msg = "this name is already used, do you want to overwrite it ?";
		int res = wxMessageBox(msg,"Rename Camera Position", wxOK|wxCANCEL|wxICON_QUESTION, NULL);
		if(res == wxCANCEL) return;

    //remove item to be overwritten
		m_Tags->DeleteTag(flyto_tagName.char_str());
		m_PositionList->Delete(m_PositionList->FindString(name));
	}

	wxString flyto_oldTagName = "FLY_TO_" + m_SelectedPosition;
  albaTagItem *item = m_Tags->GetTag(flyto_oldTagName.char_str());
  assert(item  && item->GetNumberOfComponents() == 10);

  albaTagItem itemNew;
  itemNew.DeepCopy(item);
	itemNew.SetName(flyto_tagName.char_str());
  m_Tags->DeleteTag(flyto_oldTagName);
  m_Tags->SetTag(itemNew);

	int n = m_PositionList->GetSelection();
	if(n >= 0) 
    m_PositionList->SetString(n, name);

  m_SelectedPosition = name;
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void albaAnimate::DeleteViewPoint(int pos /*= 0*/)
//----------------------------------------------------------------------------
{
  m_SelectedPosition = m_PositionList->GetStringSelection();
	if (m_SelectedPosition == "" && m_PositionList->GetCount()>0)
  {
    m_SelectedPosition = m_PositionList->GetString(0);
    pos = 0;
  };

	m_PositionList->Delete(pos);

	wxString flyto_tagName = "FLY_TO_" + m_SelectedPosition;
  if(m_Tags->IsTagPresent(flyto_tagName))
	  m_Tags->DeleteTag(flyto_tagName);

	m_SelectedPosition = "";
  if(m_PositionList->GetCount()) 
	{
		SetCurrentSelection(0);
	  m_SelectedPosition = m_PositionList->GetStringSelection();
	}
}
//----------------------------------------------------------------------------
void albaAnimate::ResetKit()
//----------------------------------------------------------------------------
{
  int num_items = m_PositionList->GetCount();
  for (int i=0; i<num_items;i++)
  {
    DeleteViewPoint(0);
  }
  albaDEL(m_StoredPositions); // Initialize the tag array.
  albaNEW(m_StoredPositions);
  EnableWidgets(); // disable all
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void albaAnimate::RetrieveStoredPositions(bool update_listbox /*= true*/)
//----------------------------------------------------------------------------
{
  if(!m_Tags) return;
  albaDEL(m_StoredPositions); // Initialize the tag array.
  albaNEW(m_StoredPositions);
  
  std::vector<std::string> tag_list;
  m_Tags->GetTagList(tag_list);

  for(int t=0; t<tag_list.size(); t++)
  {
    albaTagItem *item = m_Tags->GetTag(tag_list[t].c_str());
    if(item && ((item->GetNumberOfComponents() == 9) || (item->GetNumberOfComponents() == 10)))
    {
      wxString name = item->GetName();
      if(name.Find("FLY_TO_") != -1)
      {
        m_StoredPositions->SetTag(*item);
        if (update_listbox)
        {
	        name = name.Remove(0,7);
	        m_PositionList->Append(name);
        }
      }
    }
  }
}
//----------------------------------------------------------------------------
void albaAnimate::SetStoredPositions(albaTagArray *positions)
//----------------------------------------------------------------------------
{
  ResetKit();
  std::vector<std::string> tag_list;
  positions->GetTagList(tag_list);

  for(int t=0; t<tag_list.size(); t++)
  {
    albaTagItem *item = positions->GetTag(tag_list[t].c_str());
    m_Tags->SetTag(*item);
  }
  
  if (tag_list.size() > 0)
  {
    RetrieveStoredPositions();
    SetCurrentSelection(0);
    m_SelectedPosition = m_PositionList->GetStringSelection();
    FlyTo();
  }
}
//----------------------------------------------------------------------------
void albaAnimate::SetCurrentSelection( int pos )
//----------------------------------------------------------------------------
{
  m_PositionList->SetSelection(pos);// needed line below because 
                                  // SetSelection doesn't rise event
                                  // wxEVT_COMMAND_LISTBOX_SELECTED
                                  // trapped from albaGUI
  ((albaGUIValidator *)m_PositionList->GetValidator())->TransferFromWindow();
}
//----------------------------------------------------------------------------
albaGUI * albaAnimate::GetGui()
//----------------------------------------------------------------------------
{
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaAnimate::SetListener( albaObserver *listener )
//----------------------------------------------------------------------------
{
  m_Listener = listener;
}
//----------------------------------------------------------------------------
albaTagArray * albaAnimate::GetStoredPositions()
//----------------------------------------------------------------------------
{
  RetrieveStoredPositions(false);
  return m_StoredPositions;
}
