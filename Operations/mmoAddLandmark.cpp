/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoAddLandmark.cpp,v $
  Language:  C++
  Date:      $Date: 2006-05-16 13:05:46 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani    
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mmoAddLandmark.h"

#include "mafDecl.h"

#include "mmiPicker.h"
#include "mafInteractor.h"
#include "mmgGui.h"

//dictionary
#include "mmgGuiHolder.h"
#include "mmgSplittedPanel.h"
#include "mmgNamedPanel.h"
#include "mmgDictionaryWidget.h"
#include "mmoExplodeCollapse.h"
#include "mafSmartPointer.h"

#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"

#include "vtkDataSet.h"
#include "vtkPoints.h"

//----------------------------------------------------------------------------
mmoAddLandmark::mmoAddLandmark(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType			= OPTYPE_OP;
	m_Canundo			= true;

  m_PickedVme			  = NULL;
  m_Cloud				    = NULL;
  m_LandmarkPicker	= NULL;
  m_OldBehavior     = NULL;
  m_LandmarkAdded.clear();

  m_LandmarkName        = "new landmark";
	m_CloudCreatedFlag    = false; 
	m_LandmarkPosition[0] = m_LandmarkPosition[1] = m_LandmarkPosition[2] = 0;
	m_AddToCurrentTime    = 0;
}
//----------------------------------------------------------------------------
mmoAddLandmark::~mmoAddLandmark()
//----------------------------------------------------------------------------
{
	m_LandmarkAdded.clear();
  mafDEL(m_LandmarkPicker);
	if(m_CloudCreatedFlag) 
    mafDEL(m_Cloud);
}
//----------------------------------------------------------------------------
mafOp* mmoAddLandmark::Copy()   
//----------------------------------------------------------------------------
{
  return new mmoAddLandmark(m_Label);
}
//----------------------------------------------------------------------------
bool mmoAddLandmark::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
	if (!node) return false;
	
  if( node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMEVolumeGray)) 
    return true;

  if( node->IsMAFType(mafVMELandmarkCloud) && 
		  node->GetParent() && 
		  (node->GetParent()->IsMAFType(mafVMESurface) || node->GetParent()->IsMAFType(mafVMEVolumeGray))) 
      return true;

  if( node->IsMAFType(mafVMELandmark) && 
		  node->GetParent() && 
			node->GetParent()->IsMAFType(mafVMELandmarkCloud) &&
			node->GetParent()->GetParent() && 
			(node->GetParent()->GetParent()->IsMAFType(mafVMESurface) || node->GetParent()->GetParent()->IsMAFType(mafVMEVolumeGray))) 
      return true;

  return false;
}
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum ADD_LANDMARK_ID
{
  ID_LM_NAME = MINID,
  ID_ADD_TO_CURRENT_TIME,
  ID_LOAD,
};
//----------------------------------------------------------------------------
void mmoAddLandmark::OpRun()
//----------------------------------------------------------------------------
{
  if(m_Input->IsMAFType(mafVMESurface) || m_Input->IsMAFType(mafVMEVolumeGray))
  {
    m_PickedVme = mafVME::SafeDownCast(m_Input);
    mafNEW(m_Cloud);
    m_Cloud->Open();
    m_Cloud->SetName("new landmark cloud");
    m_Cloud->SetRadius(m_PickedVme->GetOutput()->GetVTKData()->GetLength()/60.0);
    m_Cloud->ReparentTo(m_PickedVme);
    mafEventMacro(mafEvent(this,VME_SHOW,m_Cloud,true));
    m_CloudCreatedFlag = true;
  }
  else if(m_Input->IsMAFType(mafVMELandmark))
  {
    // add a new landmark as brother of this one
    m_Cloud   = (mafVMELandmarkCloud *) ( m_Input->GetParent() );
    m_PickedVme = mafVME::SafeDownCast(m_Input->GetParent()->GetParent());
  }
  else if(m_Input->IsMAFType(mafVMELandmarkCloud))
  {
    m_Cloud   = (mafVMELandmarkCloud*) ( m_Input );
    m_PickedVme = mafVME::SafeDownCast(m_Input->GetParent());
  }
  else
  {
    assert(false); 
  }
  // customize m_PickedVme behavior
  m_LandmarkPicker = mmiPicker::New();
  m_LandmarkPicker->SetListener(this);

  m_OldBehavior = m_PickedVme->GetBehavior();
  m_PickedVme->SetBehavior(m_LandmarkPicker);

  mafString tooltip("If checked, add the landmark to the current time. \nOtherwise add the landmark at time = 0");
/*
  // setup Gui
  m_Gui = new mmgGui(this);
  m_Gui->Label("landmark name");
  m_Gui->String(ID_LM_NAME,"",&m_LandmarkName);
  m_Gui->Label("");
  m_Gui->Bool(ID_ADD_TO_CURRENT_TIME,"current time",&m_AddToCurrentTime,0,tooltip);
  m_Gui->Label("");
  m_Gui->OkCancel();
  m_Gui->Enable(wxOK, false);

  ShowGui();
*/
  // setup gui_panel
	m_GuiPanel = new mmgNamedPanel(mafGetFrame(),-1);
	m_GuiPanel->SetTitle("Add Landmark:");

  // setup splitter
	mmgSplittedPanel *sp = new mmgSplittedPanel(m_GuiPanel,-1,155);
	m_GuiPanel->Add(sp,1,wxEXPAND);

	// setup dict
	m_Dict = new mmgDictionaryWidget(sp,-1);
  m_Dict->SetListener(this);
  m_Dict->SetCloud(m_Cloud);
	sp->PutOnTop(m_Dict->GetWidget());

	// setup GuiHolder
	m_Guih = new mmgGuiHolder(sp,-1,false,true);
	sp->PutOnBottom(m_Guih);
  
  // setup Gui
	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);
	m_Gui->Button(ID_LOAD,"load dictionary");
	m_Gui->Label("");
	m_Gui->Label("landmark name");
	m_Gui->String(ID_LM_NAME,"",&m_LandmarkName);
	m_Gui->Label("");
	m_Gui->Bool(ID_ADD_TO_CURRENT_TIME,"current time",&m_AddToCurrentTime,0,tooltip);
	m_Gui->Label("");
	m_Gui->Label("choose a name from the dictionary");
	m_Gui->Label("and place landmark by");
	m_Gui->Label("clicking on the parent surface");
	m_Gui->Label("");
  m_Gui->OkCancel();
	m_Gui->Enable(wxOK, false);
	
	// Show Gui
	m_Guih->Put(m_Gui);
	mafEventMacro(mafEvent(this,OP_SHOW_GUI,(wxWindow *)m_GuiPanel));

}
//----------------------------------------------------------------------------
void mmoAddLandmark::OpDo()
//----------------------------------------------------------------------------
{
  AddLandmark();
}
//----------------------------------------------------------------------------
void mmoAddLandmark::OpUndo()
//----------------------------------------------------------------------------
{
  RemoveLandmark();
}
//----------------------------------------------------------------------------
void mmoAddLandmark::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  vtkPoints *pts = NULL; 
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_LOAD:
		  m_Dict->LoadDictionary();
		  break;
      
      case ITEM_SELECTED:
		  m_LandmarkName = *(e->GetString());
			if(this->m_Cloud && this->m_Cloud->FindInTreeByName(m_LandmarkName.GetCStr()))
			{
				wxString msg("Landmark with that name already exist, Please change it!");
				wxMessageBox( msg,"Warning", wxOK|wxICON_WARNING , NULL);
				m_LandmarkName = "";
			}
			m_Gui->Update();
		  break;

      case VME_PICKED:
        if(this->m_Cloud && this->m_Cloud->FindInTreeByName(m_LandmarkName.GetCStr()))
        {
          wxString msg("Landmark with that name already exist, Please change it!");
          wxMessageBox( msg,"Warning", wxOK|wxICON_WARNING , NULL);
          m_LandmarkName = "";
          m_Gui->Update();
        }
        else
        {
          pts = (vtkPoints *)e->GetVtkObj();
          pts->GetPoint(0,m_LandmarkPosition);
          AddLandmark(m_LandmarkPosition);
        }
      break;
      case wxOK:
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        RemoveLandmark();
        OpStop(OP_RUN_CANCEL);
      break;
      case ID_LM_NAME:
      if(this->m_Cloud && this->m_Cloud->FindInTreeByName(m_LandmarkName.GetCStr()))
      {
        wxString msg("Landmark with that name already exist, Please change it!");
        wxMessageBox( msg,"Warning", wxOK|wxICON_WARNING , NULL);
        m_LandmarkName = "";
        m_Gui->Update();
      }
      break;
      default:
        mafEventMacro(*e);
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mmoAddLandmark::OpStop(int result)
//----------------------------------------------------------------------------
{
  mafEventMacro(mafEvent(this,OP_HIDE_GUI,(wxWindow *)m_GuiPanel));
	cppDEL(m_Dict);
	cppDEL(m_GuiPanel)

  m_PickedVme->SetBehavior(m_OldBehavior);
	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoAddLandmark::AddLandmark(double pos[3])
//----------------------------------------------------------------------------
{
  bool cloud_was_open = m_Cloud->IsOpen();
  if (!cloud_was_open)
  {
    m_Cloud->Open();
  }
  
  mafSmartPointer<mafVMELandmark> landmark;
  landmark->SetName(m_LandmarkName.GetCStr());
  landmark->ReparentTo(m_Cloud);
  landmark->Update();
  if(m_AddToCurrentTime)
    landmark->SetAbsPose(m_LandmarkPosition[0],m_LandmarkPosition[1],m_LandmarkPosition[2],0,0,0);
  else
    landmark->SetAbsPose(m_LandmarkPosition[0],m_LandmarkPosition[1],m_LandmarkPosition[2],0,0,0,0);

  mafEventMacro(mafEvent(this,VME_SHOW,landmark.GetPointer(),true));
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

  m_LandmarkAdded.push_back(landmark);
  m_LandmarkAdded[m_LandmarkAdded.size()-1]->Register(NULL);

  if (!cloud_was_open)
  {
    m_Cloud->Close();
  }
  m_Gui->Enable(wxOK, true);
}
//----------------------------------------------------------------------------
void mmoAddLandmark::AddLandmark()
//----------------------------------------------------------------------------
{
  int reparent_result = MAF_OK;
  if(m_CloudCreatedFlag)
  {
    reparent_result = m_Cloud->ReparentTo(m_PickedVme);
    if (reparent_result == MAF_OK)
    {
      mafEventMacro(mafEvent(this,VME_SHOW,m_Cloud,true));
    }
  }
  else
  {
    bool cloud_was_open = m_Cloud->IsOpen();
    if (!cloud_was_open)
    {
      m_Cloud->Open();
    }
    for (int l=0; l < m_LandmarkAdded.size(); l++)
    {
      reparent_result = m_LandmarkAdded[l]->ReparentTo(m_Cloud);
      if (reparent_result == MAF_OK)
      {
        mafEventMacro(mafEvent(this,VME_SHOW,m_LandmarkAdded[l],true));
      }
    }
    if (!cloud_was_open)
    {
      m_Cloud->Close();
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mmoAddLandmark::RemoveLandmark()
//----------------------------------------------------------------------------
{
  if(m_CloudCreatedFlag)
  {
    mafEventMacro(mafEvent(this,VME_REMOVE,m_Cloud));
  }
  else
  {
    for (int l=0; l < m_LandmarkAdded.size(); l++)
    {
      mafEventMacro(mafEvent(this,VME_REMOVE,m_LandmarkAdded[l]));
    }
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
