/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpEditMetadata
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


#include "albaOpEditMetadata.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"

#include "albaVME.h"
#include "albaTagArray.h"
#include "albaTagItem.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpEditMetadata);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpEditMetadata::albaOpEditMetadata(const wxString &label) :
albaOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;

  m_MetadataList  = NULL;
  m_TagArray      = NULL;
  m_OldTagArray   = NULL;
  m_SelectedTag   = NULL;

  m_TagName = _("Name");
  m_TagType = albaOpEditMetadata::NUMERIC_TAG;
  m_TagMulteplicity   = 1;
  m_TagComponent      = 0;
  m_TagValueAsString  = "0";
  m_TagValueAsDouble  = 0.0;
}

//----------------------------------------------------------------------------
albaOpEditMetadata::~albaOpEditMetadata()
//----------------------------------------------------------------------------
{
  albaDEL(m_OldTagArray);
}

//----------------------------------------------------------------------------
albaOp* albaOpEditMetadata::Copy()
//----------------------------------------------------------------------------
{
	return new albaOpEditMetadata(m_Label);
}

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum METADATA_EDIT_ID
{
  ID_METADATA_LIST = MINID,
  ID_ADD_METADATA,
  ID_REMOVE_METADATA,
  ID_TAG_NAME,
  ID_TAG_TYPE,
  ID_TAG_MULTEPLICITY,
  ID_TAG_COMPONENT,
  ID_TAG_DOUBLE_VALUE,
  ID_TAG_STRING_VALUE,
};

//----------------------------------------------------------------------------
void albaOpEditMetadata::OpRun()
//----------------------------------------------------------------------------
{
  m_TagArray = m_Input->GetTagArray();
  albaNEW(m_OldTagArray);
  m_OldTagArray->DeepCopy(m_TagArray);
  std::vector<std::string> tag_list;
  m_TagArray->GetTagList(tag_list);

	if (!m_TestMode)
	{
		m_Gui = new albaGUI(this);

		m_Gui->Label("");

		m_MetadataList = m_Gui->ListBox(ID_METADATA_LIST, "", 120);
		for (int t = 0; t < tag_list.size(); t++)
			m_MetadataList->Insert(tag_list[t].c_str(), 0);

		if (!m_MetadataList->IsEmpty())
		{
			m_MetadataList->SetSelection(0, true);//
			SelectTag(m_MetadataList->GetString(0).ToAscii());
		}

		wxString tag_type[2] = { _("Numeric"),_("String") };

		m_Gui->TwoButtons(ID_ADD_METADATA, ID_REMOVE_METADATA, "Add", "Remove");
		m_Gui->Divider(2);
		m_Gui->Label("Property", true);
		m_Gui->String(ID_TAG_NAME, _("Name"), &m_TagName, "", false, false, false, 0.35);
		m_Gui->Combo(ID_TAG_TYPE, _("Type"), &m_TagType, 2, tag_type, "", 0.35);
		m_Gui->Integer(ID_TAG_MULTEPLICITY, _("Multep."), &m_TagMulteplicity, 1, MAXINT, "", false, 0.35);
		m_Gui->Integer(ID_TAG_COMPONENT, _("Comp"), &m_TagComponent, 0, MAXINT, "", false, 0.35);
		m_Gui->Double(ID_TAG_DOUBLE_VALUE, _("Num. value"), &m_TagValueAsDouble, MINDOUBLE, MAXDOUBLE, -1, "", false, 0.35);
		m_Gui->String(ID_TAG_STRING_VALUE, _("String value"), &m_TagValueAsString, "", true, false, false, 0.35);

		EnableWidgets();

		//////////////////////////////////////////////////////////////////////////
		m_Gui->Label("");
		m_Gui->Divider(1);
		m_Gui->OkCancel();
		m_Gui->Label("");

		ShowGui();
	}
}
//----------------------------------------------------------------------------
void albaOpEditMetadata::SelectTag(const char *tag_name)
//----------------------------------------------------------------------------
{
  m_SelectedTag = m_TagArray->GetTag(tag_name);
  if (m_SelectedTag == NULL)
  {
    wxMessageBox(_("Try to select a not existing Tag!"), _("Warning"));
    return;
  }
  m_TagName = m_SelectedTag->GetName();
  m_TagType = m_SelectedTag->GetType() - 1;
  m_TagMulteplicity = m_SelectedTag->GetNumberOfComponents();
  m_TagComponent = 0;
  m_TagValueAsString = m_SelectedTag->GetComponent(m_TagComponent);
  m_TagValueAsDouble = m_SelectedTag->GetComponentAsDouble(m_TagComponent);

  if(!m_TestMode)
    EnableWidgets();
}
//----------------------------------------------------------------------------
void albaOpEditMetadata::EnableWidgets()
//----------------------------------------------------------------------------
{
  bool enable_edit = m_MetadataList->GetCount() > 0;
  m_Gui->Enable(ID_REMOVE_METADATA,enable_edit);
  m_Gui->Enable(ID_TAG_NAME,enable_edit);
  m_Gui->Enable(ID_TAG_TYPE,enable_edit);
  m_Gui->Enable(ID_TAG_MULTEPLICITY,enable_edit);
  m_Gui->Enable(ID_TAG_COMPONENT,enable_edit);
  m_Gui->Enable(ID_TAG_DOUBLE_VALUE,m_TagType == albaOpEditMetadata::NUMERIC_TAG && enable_edit);
  m_Gui->Enable(ID_TAG_STRING_VALUE,m_TagType == albaOpEditMetadata::STRING_TAG && enable_edit);
}
//----------------------------------------------------------------------------
void albaOpEditMetadata::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_METADATA_LIST:
      {
        int sel = m_MetadataList->GetSelection();
        if (sel >= 0)
        {
          wxString tag_name = m_MetadataList->GetString(sel);
          SelectTag(tag_name.ToAscii());
          m_Gui->Update();
        }
      }
      break;
      case ID_ADD_METADATA:
      {
        m_TagName = "New Tag";
        AddNewTag(m_TagName);
        m_Gui->Update();
      }
      break;
      case ID_TAG_NAME:
      {
        SetTagName(m_TagName.GetCStr());
      }
      break;
      case ID_TAG_TYPE:
        m_SelectedTag->SetType(m_TagType+1);
        EnableWidgets();
      break;
      case ID_TAG_MULTEPLICITY:
      {
        int answere = wxYES;
        int num_comp = m_SelectedTag->GetNumberOfComponents();
        if (m_TagMulteplicity < num_comp)
        {
          answere = wxMessageBox(_("You are reducing the number of components: you may lost previous data. \nAre you sure?"),_("Warning"), wxYES_NO);
        }
        if (answere == wxNO)
        {
          m_TagMulteplicity = num_comp;
          m_Gui->Update();
          return;
        }
        m_SelectedTag->SetNumberOfComponents(m_TagMulteplicity);
      }
      break;
      case ID_TAG_COMPONENT:
        if (m_TagComponent >= m_TagMulteplicity)
        {
          m_TagComponent = m_TagMulteplicity-1;
        }
        if (m_TagType == albaOpEditMetadata::NUMERIC_TAG)
        {
          m_TagValueAsDouble = m_SelectedTag->GetComponentAsDouble(m_TagComponent);
        }
        else
        {
          m_TagValueAsString = m_SelectedTag->GetComponent(m_TagComponent);
        }
        m_Gui->Update();
      break;
      case ID_TAG_DOUBLE_VALUE:
        m_SelectedTag->SetComponent(m_TagValueAsDouble,m_TagComponent);
      break;
      case ID_TAG_STRING_VALUE:
        m_SelectedTag->SetComponent(m_TagValueAsString,m_TagComponent);
      break;
      case ID_REMOVE_METADATA:
      {
        RemoveTag();
      }
      break;
      case wxOK:
        OpStop(OP_RUN_OK);
      break;
      case wxCANCEL:
        OpUndo();
        OpStop(OP_RUN_CANCEL);
      break;
      default:
        albaEventMacro(*e);
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void albaOpEditMetadata::SetTagName(const char *name)
//----------------------------------------------------------------------------
{
  albaTagItem new_named_tag;
  new_named_tag = *m_SelectedTag;
  new_named_tag.SetName(name);
  m_TagArray->DeleteTag(m_SelectedTag->GetName());
  m_TagArray->SetTag(new_named_tag);
  if(!m_TestMode)
  {
    m_SelectedTag = m_TagArray->GetTag(name);
    int sel = m_MetadataList->GetSelection();
    m_MetadataList->SetString(sel,name);

    if(!m_TestMode)
      m_Gui->Update();
}
}

//----------------------------------------------------------------------------
void albaOpEditMetadata::RemoveTag()
//----------------------------------------------------------------------------
{
  m_TagArray->DeleteTag(m_SelectedTag->GetName());
  m_SelectedTag = NULL;
  
  if(!m_TestMode)
  {
    int sel = m_MetadataList->GetSelection();
    m_MetadataList->Delete(sel);

    if (m_MetadataList->GetCount() == 0)
    {
      EnableWidgets();
      return;
    }
    else
    {
      sel = sel > 0 ? sel-1 : 0;
      m_MetadataList->SetSelection(sel,true);
    }
    
    OnEvent(&albaEvent(m_Gui,ID_METADATA_LIST));
  }
}
//----------------------------------------------------------------------------
void albaOpEditMetadata::AddNewTag(albaString &name)
//----------------------------------------------------------------------------
{
  int n = -1;
  m_TagName = name;
  wxString new_name;
  new_name = m_TagName;
  m_TagType = albaOpEditMetadata::NUMERIC_TAG;
  m_TagMulteplicity   = 1;
  m_TagComponent      = 0;
  m_TagValueAsString  = "0";
  m_TagValueAsDouble  = 0.0;

  m_SelectedTag = new albaTagItem();
  m_SelectedTag->SetName(new_name.ToAscii());
  m_SelectedTag->SetType(m_TagType+1);
  m_SelectedTag->SetNumberOfComponents(m_TagMulteplicity);
  m_SelectedTag->SetComponent(m_TagValueAsDouble,m_TagComponent);
  bool tag_present = false;
  while (m_TagArray->IsTagPresent(new_name.ToAscii()))
  {
    tag_present = true;
    new_name = m_TagName;
    new_name << n++;
  }
  if (tag_present)
  {
    wxString msg = _("Tag named ");
    msg << m_TagName.GetCStr();
    msg << " renamed in ";
    msg << new_name;
    msg <<" because already exists!";
    wxMessageBox(msg, _("Warning"));
  }
  if(!m_TestMode)
    m_MetadataList->Insert(new_name, m_MetadataList->GetCount());
  m_TagName = new_name;
  m_SelectedTag->SetName(m_TagName.GetCStr());
  m_TagArray->SetTag(*m_SelectedTag);
  cppDEL(m_SelectedTag);
  SelectTag(m_TagName.GetCStr());
  if(!m_TestMode)
    m_MetadataList->SetSelection(m_MetadataList->GetCount()-1,true);
}
//----------------------------------------------------------------------------
void albaOpEditMetadata::OpUndo()
//----------------------------------------------------------------------------
{
  // Paolo 22/11/2006: DeepCopy copy the tags coming from the m_OldTagArray but
  // if in m_TagArray there are other tags they will be present also after the DeepCopy
  // => DeepCopy do not produce equals tag array in this case! Perhaps the 
  std::vector<std::string> tag_list;
  m_TagArray->GetTagList(tag_list);
  for (int t=0; t<tag_list.size();t++)
    m_TagArray->DeleteTag(tag_list[t].c_str());
  
  m_TagArray->DeepCopy(m_OldTagArray);
}
