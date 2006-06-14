/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGuiSaveRestorePose.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-14 14:46:33 $
  Version:   $Revision: 1.4 $
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


#include "mafGuiSaveRestorePose.h"
#include "mafDecl.h"
#include "mmgGui.h"
#include "mafSmartPointer.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafTagArray.h"
#include "mafTagItem.h"

#include "vtkMatrix4x4.h"

//----------------------------------------------------------------------------
mafGuiSaveRestorePose::mafGuiSaveRestorePose(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  assert(input);

  m_Listener = listener;
  InputVME = input;
  m_Gui = NULL;
  
  CreateGui();
}
//----------------------------------------------------------------------------
mafGuiSaveRestorePose::~mafGuiSaveRestorePose() 
//----------------------------------------------------------------------------
{ 
}

//----------------------------------------------------------------------------
void mafGuiSaveRestorePose::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->SetListener(this);
  m_Gui->Divider(2);
  m_Gui->Label("save/restore pose", true);
  m_Gui->Button(ID_SAVE,"save pose");
  m_Gui->Button(ID_REMOVE,"remove pose");
	m_Gui->Label("");
  m_PositionsList = m_Gui->ListBox(ID_APPLY,"apply pose");
  m_PositionsList->Clear();
  ReadSavedPoses();

  EnableWidgets(true);
  m_Gui->Update();
}

//----------------------------------------------------------------------------
void mafGuiSaveRestorePose::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_SAVE:
        SavePose(InputVME->GetOutput()->GetAbsMatrix());
        m_Gui->Enable(ID_APPLY, true);
      break;
      case ID_REMOVE:
      {
        int sel_pose = m_PositionsList->GetSelection();
        if (sel_pose != -1)
          RemovePose(sel_pose);
        if (m_PositionsList->GetCount() == 0)
          m_Gui->Enable(ID_REMOVE, false);
      }
      break;
      case ID_APPLY:
      {
        int sel_pose = m_PositionsList->GetSelection();
        if (sel_pose == -1)
          return;
        EnableWidgets(true);
        RestorePose(sel_pose);
      }
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
}

//----------------------------------------------------------------------------
void mafGuiSaveRestorePose::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
  int num_stored_poses = m_PositionsList->GetCount();

  m_Gui->Enable(ID_SAVE, enable);
  m_Gui->Enable(ID_REMOVE, enable && num_stored_poses > 0);
  m_Gui->Enable(ID_APPLY, enable && num_stored_poses > 0);
}
//----------------------------------------------------------------------------
void mafGuiSaveRestorePose::ReadSavedPoses()
//----------------------------------------------------------------------------
{
	mafTagArray *tag_array = InputVME->GetTagArray();
  int n = tag_array->GetNumberOfTags();
  std::vector<std::string> tag_list;
  tag_array->GetTagList(tag_list);
	for(int t=0; t<n; t++)
	{
	  mafTagItem *item = tag_array->GetTag(tag_list[t].c_str());
		if(item->GetNumberOfComponents() == 16)
		{
			wxString name = item->GetName();
      if(name.Find("STORED_ABS_POS_") != -1)
      {
			  name = name.Remove(0,15);
			  m_PositionsList->Append(name);
      }
		}
	}
}
//----------------------------------------------------------------------------
void mafGuiSaveRestorePose::SavePose(mafMatrix *abs_pose)
//----------------------------------------------------------------------------
{
  wxString pose_name = "";
  
  do {
  	pose_name = wxGetTextFromUser("Enter the pose name","Save position","Saved pose", m_Gui);
  } while(m_PositionsList->FindString(pose_name) != wxNOT_FOUND);
  
  pose_name.Trim();
  pose_name.Trim(FALSE);

  wxString AbsPos_tagName = "STORED_ABS_POS_" + pose_name;

  mafTagArray *tagArray = InputVME->GetTagArray();
  if(tagArray->IsTagPresent(AbsPos_tagName.c_str())) 
  {
		wxString msg = "this name is already used, do you want to to overwrite it ?";
		int res = wxMessageBox(msg,"Store Position", wxOK|wxCANCEL|wxICON_QUESTION, NULL);
		if(res == wxCANCEL) return;

    //remove item to be overwritten
		tagArray->DeleteTag(AbsPos_tagName.c_str());
	}
	
  mafTagItem item;
	//item.SetName(m_selected.c_str());
	item.SetName(AbsPos_tagName.c_str());
	item.SetNumberOfComponents(16);
	item.SetComponent(abs_pose->GetElement(0,0),0);
	item.SetComponent(abs_pose->GetElement(0,1),1);
	item.SetComponent(abs_pose->GetElement(0,2),2);
	item.SetComponent(abs_pose->GetElement(0,3),3);
	item.SetComponent(abs_pose->GetElement(1,0),4);
	item.SetComponent(abs_pose->GetElement(1,1),5);
	item.SetComponent(abs_pose->GetElement(1,2),6);
	item.SetComponent(abs_pose->GetElement(1,3),7);
	item.SetComponent(abs_pose->GetElement(2,0),8);
	item.SetComponent(abs_pose->GetElement(2,1),9);
	item.SetComponent(abs_pose->GetElement(2,2),10);
	item.SetComponent(abs_pose->GetElement(2,3),11);
	item.SetComponent(abs_pose->GetElement(3,0),12);
	item.SetComponent(abs_pose->GetElement(3,1),13);
	item.SetComponent(abs_pose->GetElement(3,2),14);
	item.SetComponent(abs_pose->GetElement(3,3),15);
	tagArray->SetTag(item);
  
  m_PositionsList->Append(pose_name);
}
//----------------------------------------------------------------------------
void mafGuiSaveRestorePose::RemovePose(int sel_pose)
//----------------------------------------------------------------------------
{
  wxString pose_name = m_PositionsList->GetString(sel_pose);
  wxString AbsPos_tagName = "STORED_ABS_POS_" + pose_name;
  if (InputVME->GetTagArray()->IsTagPresent(AbsPos_tagName.c_str()))
    InputVME->GetTagArray()->DeleteTag(AbsPos_tagName.c_str());
  m_PositionsList->Delete(sel_pose);
}
//----------------------------------------------------------------------------
void mafGuiSaveRestorePose::RestorePose(int sel_pose)
//----------------------------------------------------------------------------
{
  wxString pose_name = m_PositionsList->GetString(sel_pose);
  wxString AbsPos_tagName = "STORED_ABS_POS_" + pose_name;
  int comp = 0;
  mafMatrix abs_pose;
  if (InputVME->GetTagArray()->IsTagPresent(AbsPos_tagName.c_str()))
  {
    mafTagItem *item = InputVME->GetTagArray()->GetTag(AbsPos_tagName.c_str());
    for (int r=0; r<4; r++)
      for (int c=0; c<4; c++)
        abs_pose.SetElement(r,c,item->GetComponentAsDouble(comp++));
  }
  else
  {
    return;
  }

  InputVME->SetAbsMatrix(abs_pose);

  mafEvent e2s;
  e2s.SetSender(this);
  e2s.SetMatrix(&abs_pose);
  e2s.SetId(ID_TRANSFORM);
  mafEventMacro(e2s);
  
//  mafEventMacro(mafEvent(this, CAMERA_UPDATE));  // Paolo 20-07-2005
}
