/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaOpAddLandmark.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Paolo Quadrani, Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaOpAddLandmark.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUIDictionaryWidget.h"
#include "albaInteractor.h"
#include "albaInteractorPicker.h"
#include "albaLogicWithManagers.h"
#include "albaVME.h"
#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMESurface.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEVolumeGray.h"
#include "albaXMLString.h"

#include "mmaMaterial.h"
#include "mmuDOMTreeErrorReporter.h"
#include "vtkDataSet.h"
#include "vtkPoints.h"

#include "xercesc\framework\LocalFileFormatTarget.hpp"
#include "xercesc\parsers\XercesDOMParser.hpp"
#include "xercesc\util\PlatformUtils.hpp"
#include "xercesc\util\XercesDefs.hpp"
#include <set>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpAddLandmark);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpAddLandmark::albaOpAddLandmark(const wxString &label) :
albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;
	
	m_LandmarkUndoVetc.clear();
	m_LandmarkRedoVect.clear();

	m_GroupsNameVect.clear();
	m_LandmarkGroupVect.clear();
	m_LandmarkNameVect.clear();

	m_PickedVme = NULL;
	m_LandmarkPicker = NULL;

	m_Cloud = NULL;
	m_SelectedLandmark = NULL;

	m_AuxLandmarkCloud = NULL;
	m_AuxLandmark = NULL;

	m_GroupComboBox = NULL;
	m_LandmarkGuiDict = NULL;
	m_SelectedGroup = 0;
	m_SelectedItem = -1;

	m_CloudName = "New landmark cloud";
	m_LandmarkName = "";
	m_LandmarkRadius = 0.1;

	m_LandmarkNameFromDef = "";
	m_AddLandmarkFromDef = false;

	m_RemoveMessage = "";
	m_DictMessage = "";

	// Flags
	m_AddModeFlag = true;

	m_IsCloudCreated = false;
	m_FirstOpDo = true;
	m_DictionaryLoaded = false;
}
//----------------------------------------------------------------------------
albaOpAddLandmark::~albaOpAddLandmark()
{
	m_GroupsNameVect.clear();
	m_LandmarkGroupVect.clear();
	m_LandmarkNameVect.clear();

	// 
	for (int i=0; i< m_LandmarkUndoVetc.size(); i++)
	{
		albaDEL(m_LandmarkUndoVetc[i]);
	}

	m_LandmarkUndoVetc.clear();

	//
	for (int i = 0; i < m_LandmarkRedoVect.size(); i++)
	{
		albaDEL(m_LandmarkRedoVect[i]);
	}

	m_LandmarkRedoVect.clear();

	//
	albaDEL(m_LandmarkPicker);

	if (m_IsCloudCreated)
		albaDEL(m_Cloud);
}

//----------------------------------------------------------------------------
albaOp* albaOpAddLandmark::Copy()
{
	albaOpAddLandmark *op = new albaOpAddLandmark(m_Label);
	op->m_OpType = m_OpType;
	op->m_Canundo = m_Canundo;
	return op;
}
//----------------------------------------------------------------------------
bool albaOpAddLandmark::Accept(albaVME*node)
{
	return (node != NULL); // Accept all other VMEs
}

//----------------------------------------------------------------------------
char** albaOpAddLandmark::GetIcon()
{
#include "pic/MENU_OP_ADD_LANDMARK.xpm"
	return MENU_OP_ADD_LANDMARK_xpm;
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::OpRun()
{
	if (m_Input)
	{
		if (m_Input->IsALBAType(albaVMELandmarkCloud))
		{
			// Add a new landmark to existent landmarkCloud
			m_Cloud = albaVMELandmarkCloud::SafeDownCast(m_Input);
			m_PickedVme = m_Input->GetParent();
		}
		else if (m_Input->IsALBAType(albaVMELandmark))
		{
			// Add a new landmark as brother of this one
			m_Cloud = albaVMELandmarkCloud::SafeDownCast(m_Input->GetParent());
			if (m_Cloud)
				m_PickedVme = m_Cloud->GetParent();
		}
		else
		{
			// Create New LandmarkCloud
			albaNEW(m_Cloud);
			m_Cloud->SetName(m_CloudName);
			if (m_TestMode) m_Cloud->TestModeOn();

			m_PickedVme = m_Input;

			// Calculate Landmark radius
			vtkDataSet * vtkData = m_PickedVme->GetOutput()->GetVTKData();
			double radius = (vtkData != NULL) ? vtkData->GetLength() / 60.0 : 7.0;
			SetLandmarkRadius(radius);

			m_Cloud->ReparentTo(m_PickedVme);

			m_IsCloudCreated = true;
		}

		GetLogicManager()->VmeShow(m_Cloud, true);
		GetLogicManager()->VmeShow(m_PickedVme, true);

		// Customize m_PickedVme behavior
		m_LandmarkPicker = albaInteractorPERPicker::New();

		albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)(m_LandmarkPicker)));
		m_LandmarkPicker->SetListener(this);
	}

	// Save old color
	mmaMaterial *material = m_Cloud->GetMaterial();
	m_OldColorCloud[0] = material->m_Diffuse[0];
	m_OldColorCloud[1] = material->m_Diffuse[1];
	m_OldColorCloud[2] = material->m_Diffuse[2];
	m_OldColorCloud[3] = material->m_Opacity;

	// Set Landmarks color 
	SetCloudColor(m_Cloud, 1.0, 0.1, 0.1, 0.8);

	m_GroupsNameVect.push_back("Show All");
	StringVector newVect;
	m_LandmarkGroupVect.push_back(newVect);

	LoadLandmarksFromVME();

	if (!GetTestMode())
	{
		CreateGui();
		UpdateGui();
	}

	// Create Aux LandmarkCloud
	albaNEW(m_AuxLandmarkCloud);
	m_AuxLandmarkCloud->ReparentTo(m_PickedVme);
	m_AuxLandmarkCloud->SetRadius(m_Cloud->GetRadius() + 0.01);
	m_AuxLandmarkCloud->SetName(_("Aux Cloud"));
	if (m_TestMode) m_AuxLandmarkCloud->TestModeOn();
	SetCloudColor(m_AuxLandmarkCloud, 0, 1, 1, 0.8);

	albaNEW(m_AuxLandmark);
	m_AuxLandmark->SetName(_("Aux Landmark"));
	m_AuxLandmark->ReparentTo(m_AuxLandmarkCloud);

	GetLogicManager()->VmeShow(m_AuxLandmarkCloud, true);
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::OpStop(int result)
{
	// Reset Landmarks color 
	SetCloudColor(m_Cloud, m_OldColorCloud[0], m_OldColorCloud[1], m_OldColorCloud[2], m_OldColorCloud[3]);

	if (result == OP_RUN_OK)
	{
		int nLandmarks = m_Cloud->GetNumberOfLandmarks();

		for (int i = 0; i < nLandmarks; i++)
		{
			albaVMELandmark *lm = m_Cloud->GetLandmark(i);

			// Create Redo Vector
			albaVMELandmark* newLandmark = NULL;
			albaNEW(newLandmark);
			newLandmark->DeepCopy(lm);
			m_LandmarkRedoVect.push_back(newLandmark);
		}
	}
	else
	{
		OpUndo();
	}

	albaEventMacro(albaEvent(this, PER_POP));
	albaDEL(m_LandmarkPicker);

	cppDEL(m_LandmarkGuiDict);

	// Delete Aux Landmark 
	m_AuxLandmarkCloud->RemoveLandmark(0);
	albaDEL(m_AuxLandmark);
	m_AuxLandmarkCloud->ReparentTo(NULL);
	albaDEL(m_AuxLandmarkCloud);

	if (!m_TestMode)
	{
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::OpDo()
{
	int reparent_result = ALBA_OK;

	//On OpStop we run the first OpDo and the Lmc is already ok
	if (!m_FirstOpDo)
	{
		if (m_IsCloudCreated)
		{
			reparent_result = m_Cloud->ReparentTo(m_PickedVme);
		}
		else
		{
			GetLogicManager()->VmeShow(m_Cloud, true);
			RestoreLandmarkVect(m_LandmarkRedoVect);
		}
	}

	m_FirstOpDo = false;
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::OpUndo()
{
	if (m_IsCloudCreated)
	{
		GetLogicManager()->VmeRemove(m_Cloud);
	}
	else
	{
		RestoreLandmarkVect(m_LandmarkUndoVetc);
	}
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::CreateGui()
{
	// Setup Gui
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	const wxString choices[] = { "Show All" };
	m_GroupComboBox = m_Gui->Combo(ID_SHOW_LANDMARK_GROUP, "", &m_SelectedGroup, 1, choices);
	m_Gui->Divider(2);

	// Setup dictionary
	m_LandmarkGuiDict = new albaGUIDictionaryWidget(m_Gui, -1);
	m_LandmarkGuiDict->SetListener(this);
	m_LandmarkGuiDict->SetCloud(m_Cloud);
	m_LandmarkGuiDict->InitDictionary(NULL);
	m_LandmarkGuiDict->SetTitle("Landmarks List");
	m_Gui->Add(m_LandmarkGuiDict->GetWidget(), wxEXPAND);

	m_Gui->Divider();
	m_Gui->Divider(2);

	m_Gui->Divider();
	m_Gui->Button(ID_LANDMARK_REMOVE, "Remove");
	m_Gui->Label(&m_RemoveMessage, true);
	m_Gui->Divider();

	m_Gui->Divider(2);

	m_Gui->Label(_("Landmark Props"));
	m_Gui->String(ID_LANDMARK_NAME, _("Name"), &m_LandmarkName);
	m_Gui->Vector(ID_LANDMARK_POSITION, _("Position"), m_LandmarkPosition, MINFLOAT, MAXFLOAT, 2, _("Landmark position"));

	albaString tooltip(_("If checked, add the landmark to the current time. \nOtherwise add the landmark at time = 0"));
	m_Gui->Bool(ID_CHANGE_TIME, _("Current time"), &m_AddToCurrentTime, 1, tooltip);
	m_Gui->Divider(2);

	m_Gui->Enable(ID_LANDMARK_REMOVE, false);
	m_Gui->Enable(ID_LANDMARK_NAME, false);
	m_Gui->Enable(ID_LANDMARK_POSITION, false);

	m_Gui->Label(_("Dictionary"));
	m_Gui->Button(ID_LOAD_DICTIONARY, _("Load dictionary"));
	m_Gui->Button(ID_LOAD_DICTIONARY_FROM_CLOUD, _("Load dictionary from Cloud"));
	m_Gui->Button(ID_SAVE_DICTIONARY, _("Save dictionary"));	

	m_Gui->Divider(2);
	m_Gui->Double(ID_LANDMARK_RADIUS,"Radius:", &m_LandmarkRadius);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
 	m_Gui->Label("");

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::UpdateGui()
{
	if (m_Gui && !GetTestMode())
	{
		m_GroupComboBox->Clear();
		for (int i = 0; i < m_GroupsNameVect.size(); i++)
		{
			m_GroupComboBox->Insert(m_GroupsNameVect[i], i);
		}

		if (m_SelectedGroup >= 0)
		{
			m_GroupComboBox->Select(m_SelectedGroup);

			m_LandmarkGuiDict->InitDictionary(NULL); // Clear Dictionary

			if (m_SelectedGroup == 0) // Group 0 = All elements
			{
				for (int i = 0; i < m_LandmarkNameVect.size(); i++)
					m_LandmarkGuiDict->AddItem(m_LandmarkNameVect[i]);
			}
			else
			{
				for (int i = 0; i < m_LandmarkGroupVect[m_SelectedGroup].size(); i++)
					m_LandmarkGuiDict->AddItem(m_LandmarkGroupVect[m_SelectedGroup][i]);
			}

			if (m_AddModeFlag)
			{
				m_LandmarkGuiDict->AddItem("Add_New_Landmark");
				m_LandmarkGuiDict->SelectItem("Add_New_Landmark");
				m_LandmarkGuiDict->SetTitle("Add New Landmark");
			}

			bool isItemSelected = (m_SelectedItem >= 0 && m_SelectedItem < m_LandmarkGroupVect[m_SelectedGroup].size());
			if (isItemSelected)
			{
				m_LandmarkGuiDict->SelectItem((m_LandmarkGroupVect[m_SelectedGroup][m_SelectedItem]));
				m_LandmarkGuiDict->SetTitle("Edit selected Landmark");
			}

			m_Gui->Enable(ID_LANDMARK_REMOVE, isItemSelected && m_SelectedLandmark && m_SelectedLandmark->GetDependenciesVMEs().empty());
			m_Gui->Enable(ID_LANDMARK_POSITION, isItemSelected && m_SelectedLandmark);
			m_Gui->Enable(ID_LANDMARK_NAME, isItemSelected && m_SelectedLandmark);

			// Check Enable Ok Condition
			if (m_SelectedGroup == 0)
			{
				m_Gui->Enable(wxOK, true);
			}
			else
			{
				bool res = true;

				if (m_LandmarkGroupVect.size() > 0)
					for (int i = 0; i < m_LandmarkGroupVect[m_SelectedGroup].size(); i++)
					{
						if (!this->m_Cloud->FindInTreeByName(m_LandmarkGroupVect[m_SelectedGroup][i]))
						{
							res = false;
							break;
						}
					}

				m_Gui->Enable(wxOK, res);
			}
		}

		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::OnEvent(albaEventBase *alba_event)
{
  if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
	  switch (e->GetId())
	  {
	  case ID_LOAD_DICTIONARY: LoadDictionary(); break;
	  case ID_LOAD_DICTIONARY_FROM_CLOUD: LoaDictionaryFromCloud(); break;
	  case ID_SAVE_DICTIONARY: SaveDictionary(); break;

	  case ID_SHOW_LANDMARK_GROUP: SelectGroup(m_SelectedGroup, 0); break;
	  case ITEM_SELECTED: SelectLandmarkByName(*(e->GetString())); break;

	  case ID_LANDMARK_NAME: SetLandmarkName(m_LandmarkName); break;
	  case ID_LANDMARK_POSITION: SetLandmarkPosition(m_LandmarkPosition); break;
	  case ID_LANDMARK_RADIUS: SetLandmarkRadius(m_LandmarkRadius);  break;

	  case ID_LANDMARK_REMOVE: RemoveLandmark(); break;

	  case VME_PICKED:
	  {
		  double point[3];
		  bool hasPoint = false;
		  vtkPoints *pts = vtkPoints::SafeDownCast(e->GetVtkObj());
		  albaVME *pickedVME = e->GetVme();
		  if (pts && (pickedVME != m_Cloud) && (pickedVME != m_AuxLandmarkCloud))
		  {
			  pts->GetPoint(0, point);
			  hasPoint = true;
		  }

		  if (hasPoint)
		  {
			  if (m_AddModeFlag || (!m_AddModeFlag && m_AddLandmarkFromDef))
			  {
				  if (m_SelectedLandmark == NULL)
					  AddLandmark(point);
				  else
					  SetLandmarkPosition(point);
			  }
			  else
			  {
				  SetLandmarkPosition(point);
			  }
		  }
	  }
	  break;

	  case wxOK: OpStop(OP_RUN_OK); break;
	  case wxCANCEL: OpStop(OP_RUN_CANCEL); break;

	  default:
		  albaEventMacro(*e);
		  break;
	  }
  }
}

//--------------------------------------------------------------------------
int albaOpAddLandmark::LoadLandmarksFromVME()
{
	m_LandmarkNameVect.clear();

	for (int i = 0; i < m_Cloud->GetNumberOfLandmarks(); i++)
	{
		albaVMELandmark *lm = m_Cloud->GetLandmark(i);
		m_LandmarkNameVect.push_back(lm->GetName());
		m_LandmarkGroupVect[0].push_back(lm->GetName());

		// Create Undo Vector
		albaVMELandmark* newLandmark = NULL;
		albaNEW(newLandmark);
		newLandmark->DeepCopy(lm);
		m_LandmarkUndoVetc.push_back(newLandmark);
	}

	return ALBA_OK;
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::RestoreLandmarkVect(std::vector<albaVMELandmark*> &landmarkVect)
{
	while (m_Cloud->GetNumberOfLandmarks())
	{
		GetLogicManager()->VmeRemove(m_Cloud->GetLandmark(0));
	}

	for (int i = 0; i < landmarkVect.size(); i++)
	{
		albaSmartPointer<albaVMELandmark> newLandmark;

		newLandmark->DeepCopy(landmarkVect[i]);
		newLandmark->ReparentTo(m_Cloud);

		GetLogicManager()->VmeShow(newLandmark, true);
	}

	GetLogicManager()->CameraUpdate();
}

/// LANDMARK MANAGER
//----------------------------------------------------------------------------
void albaOpAddLandmark::AddLandmark(double pos[3])
 {
	wxString landmarkName = wxString::Format("New_Landmark_%d", m_LandmarkNameVect.size() + 1);

	albaSmartPointer<albaVMELandmark> landmark;
	landmark->SetName(landmarkName);
	landmark->SetRadius(m_LandmarkRadius);
	landmark->ReparentTo(m_Cloud);

	if (NULL != m_PickedVme)
		landmark->SetTimeStamp(m_PickedVme->GetTimeStamp());

	GetLogicManager()->VmeShow(landmark.GetPointer(), true);
	
	SelectLandmarkByName(landmarkName); // Last
	SetLandmarkPosition(pos);

	if (m_AddLandmarkFromDef)
	{
		landmark->SetName(m_LandmarkNameFromDef);
		m_AddLandmarkFromDef = false;

		// Select Next Item
		int next = m_LandmarkGuiDict->GetItemIndex(m_LandmarkNameFromDef) + 1;
		if (next < m_LandmarkGuiDict->GetSize())
			SelectLandmarkByName(m_LandmarkGuiDict->GetItemByIndex(next));
	}
	else
	{
		m_LandmarkNameVect.push_back(landmarkName);
		m_LandmarkGroupVect[m_SelectedGroup].push_back(landmarkName);

		// Select Empty Item = Enable Add Mode
		SelectLandmarkByName("NONE");
	}

	// Show Last Landmark Added 
	SetCloudColor(m_AuxLandmarkCloud, 0, 1, 0, 0.8);
	GetLogicManager()->VmeShow(m_AuxLandmarkCloud, true);

	GetLogicManager()->CameraUpdate();
	UpdateGui();
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::RemoveLandmark()
{
	if (m_Cloud && m_SelectedLandmark)
	{
		RemoveItem(m_SelectedLandmark->GetName());
		GetLogicManager()->VmeRemove(m_SelectedLandmark);

		SelectLandmarkByName("NONE");

		GetLogicManager()->CameraUpdate();
		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::SelectLandmarkByName(albaString name)
{
	m_RemoveMessage = "Add Landmark";
	m_LandmarkName = "";
	m_LandmarkPosition[0] = m_LandmarkPosition[1] = m_LandmarkPosition[2] = 0.0;

	if (m_LandmarkGuiDict)
		m_SelectedItem = m_LandmarkGuiDict->GetItemIndex(name);

	m_SelectedLandmark = m_Cloud->GetLandmark(name);
	if (m_SelectedLandmark)
	{
		m_LandmarkName = name;
		m_RemoveMessage = "";
		m_DictMessage = "Edit selected Landmark";

		double x, y, z;
		m_SelectedLandmark->GetPoint(x, y, z);
		m_LandmarkPosition[0] = x;
		m_LandmarkPosition[1] = y;
		m_LandmarkPosition[2] = z;
	
		// Check Landmark Dependencies
		albaVME::albaVMESet dependenciesVMEs = m_SelectedLandmark->GetDependenciesVMEs();
		if (!dependenciesVMEs.empty())
		{
			m_Gui->Enable(ID_LANDMARK_REMOVE, false);
			m_RemoveMessage = "Landmark has dependency";
		}

		// Update Aux Landmark
		m_AuxLandmark->SetAbsPose(x, y, z, 0, 0, 0);
		SetCloudColor(m_AuxLandmarkCloud, 0, 0, 1, 0.8);
		GetLogicManager()->VmeShow(m_AuxLandmarkCloud, true);

		UpdateGui();
	}
	else
	{
		FindDefinition(name);

		// Hide Aux Landmark
		GetLogicManager()->VmeShow(m_AuxLandmarkCloud, false);
	}
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::FindDefinition(albaString &name)
{
	m_AddLandmarkFromDef = false;

	for (int i = 0; i < m_LandmarkNameVect.size(); i++)
	{
		if (m_LandmarkNameVect[i] == name.GetCStr())
		{
			m_RemoveMessage = "Landmark is a Dictionary entry";
			m_LandmarkNameFromDef = name;
			m_AddLandmarkFromDef = true;
			return;
		}
	}
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::SetLandmarkName(albaString name)
{
	if (m_Cloud && m_SelectedLandmark)
	{
		if (m_SelectedLandmark)
		{
			wxString oldName = m_SelectedLandmark->GetName();
			m_SelectedLandmark->SetName(name); // If name is present, restore old name

			ReplaceItem(oldName, m_SelectedLandmark->GetName());

			m_LandmarkName = m_SelectedLandmark->GetName();

			m_SelectedLandmark->Update();
			UpdateGui();
		}
	}
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::SetLandmarkPosition(double pos[3])
{
	if (m_Cloud && m_SelectedLandmark)
	{
		m_LandmarkPosition[0] = pos[0];
		m_LandmarkPosition[1] = pos[1];
		m_LandmarkPosition[2] = pos[2];

		if (m_AddToCurrentTime)
			m_SelectedLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
		else
			m_SelectedLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0, 0);

		m_SelectedLandmark->Update();

		// Update Aux Landmark
		m_AuxLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
		m_AuxLandmark->Update();

		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::SetLandmarkRadius(double radius)
{
	if (m_Cloud)
	{
		m_LandmarkRadius = radius;
		m_Cloud->SetRadius(m_LandmarkRadius);

		if (m_AuxLandmarkCloud)
			m_AuxLandmarkCloud->SetRadius(m_LandmarkRadius + 0.01);

		GetLogicManager()->CameraUpdate();
	}
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::SetCloudColor(albaVMELandmarkCloud *cloud, double r, double g, double b, double a)
{
	if (cloud)
	{
		mmaMaterial *material = cloud->GetMaterial();
		material->m_Diffuse[0] = r;
		material->m_Diffuse[1] = g;
		material->m_Diffuse[2] = b;
		material->m_Opacity = a;
		material->UpdateProp();
	}
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::SelectGroup(int index, int item)
{
	if (index >= 0 && index < m_LandmarkGroupVect.size())
	{
		m_SelectedGroup = index;

		if (item >= 0 && item < m_LandmarkGroupVect[m_SelectedGroup].size())
		{
			m_SelectedItem = item;
			SelectLandmarkByName(m_LandmarkGroupVect[m_SelectedGroup][item]);
			m_SelectedItem = item;
		}

		UpdateGui();
	}	
}

/// SAVE-LOAD LANDMARK DEFINITIONS
//----------------------------------------------------------------------------
void albaOpAddLandmark::LoadDictionary(wxString fileName)
{
	if (fileName.IsEmpty())
	{
		// Select Dictionary file
		wxString wild_dict = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
		wxString dict = albaGetLastUserFolder().c_str();

		fileName = albaGetOpenFile(dict, wild_dict, "Choose Dictionary File").c_str();
	}

	if (!fileName.IsEmpty())
	{
		LoadLandmarksDefinitions(fileName);

		//
		m_DictionaryLoaded = true;
		SelectGroup(m_GroupsNameVect.size() - 1); // Last Group
		m_AddModeFlag = false;

		UpdateGui();
	}
}
//--------------------------------------------------------------------------
void albaOpAddLandmark::LoaDictionaryFromCloud(albaVMELandmarkCloud *cloud)
{
	if (cloud == NULL)
	{
		albaString title = _("Choose Landmark Cloud");
		albaEvent e(this, VME_CHOOSE, &title);
		e.SetPointer(&albaOpAddLandmark::LandmarkCloudAccept);
		albaEventMacro(e);

		cloud = (albaVMELandmarkCloud *)e.GetVme();
	}

	if (cloud)
	{
		m_LandmarkGroupVect.clear();
		m_LandmarkGroupVect.push_back(m_LandmarkNameVect);
		//m_LandmarkGroupVect[0].clear();

		StringVector newVect;
		m_GroupsNameVect.push_back(cloud->GetName());

		for (int i = 0; i < cloud->GetNumberOfLandmarks(); i++)
		{
			albaVMELandmark *lm = cloud->GetLandmark(i);
			PushUniqueItem(lm->GetName());
			newVect.push_back(lm->GetName());
		}

		m_LandmarkGroupVect.push_back(newVect);

		//
		m_DictionaryLoaded = true;
		SelectGroup(m_GroupsNameVect.size() - 1); // Last Group
		m_AddModeFlag = false;

		UpdateGui();
	}
}
//----------------------------------------------------------------------------
int albaOpAddLandmark::SaveDictionary(wxString fileName)
{
	if (fileName.IsEmpty())
	{
		// Select destination file
		albaString initialFileName;
		initialFileName = albaGetLastUserFolder().c_str();
		initialFileName.Append("\\newLandmarkDictionary.dic");

		albaString wildc = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
		fileName = albaGetSaveFile(initialFileName.GetCStr(), wildc).c_str();
	}

	if (!fileName.IsEmpty())
		return SaveLandmarksDefinitions(fileName);
}

//----------------------------------------------------------------------------
int albaOpAddLandmark::LoadLandmarksDefinitions(wxString fileName)
{
	m_LandmarkGroupVect.clear();
	m_LandmarkGroupVect.push_back(m_LandmarkNameVect);
	//m_LandmarkGroupVect[0].clear();

	//Open the file xml
	try
	{
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		// Do your failure processing here
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser *XMLParser = new  XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser;

	XMLParser->setValidationScheme(XERCES_CPP_NAMESPACE_QUALIFIER XercesDOMParser::Val_Auto);
	XMLParser->setDoNamespaces(false);
	XMLParser->setDoSchema(false);
	XMLParser->setCreateEntityReferenceNodes(false);

	mmuDOMTreeErrorReporter *errReporter = new mmuDOMTreeErrorReporter();
	XMLParser->setErrorHandler(errReporter);

	try
	{
		XMLParser->parse(fileName);
		int errorCount = XMLParser->getErrorCount();

		if (errorCount != 0)
		{
			// errors while parsing...
			albaErrorMessage("Errors while parsing XML file");
			return ALBA_ERROR;
		}

		// extract the root element and wrap inside a albaXMLElement
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = XMLParser->getDocument();
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
		assert(root);

		if (CheckNodeElement(root, "Landmarks_Dictionary"))
		{
			albaString version = GetElementAttribute(root, "Version");

			//Check Config file version
			if (version != "1.0")
			{
				albaLogMessage("Wrong file Version:\n version:%s", version.GetCStr());
				return ALBA_ERROR;
			}
		}
		else
		{
			albaLogMessage("Wrong check root node");
			return ALBA_ERROR;
		}

		XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *typesChildren = root->getChildNodes();

		for (unsigned int i = 0; i < typesChildren->getLength(); i++)
		{
			//Reading Type nodes 
			XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *typeNode = typesChildren->item(i);

			if (CheckNodeElement(typeNode, "Group"))
			{
				wxString dicName = GetElementAttribute(typeNode, "Name");

				m_GroupsNameVect.push_back(dicName);

				StringVector newVect;

				XERCES_CPP_NAMESPACE_QUALIFIER DOMNodeList *typeChildren = typeNode->getChildNodes();

				for (unsigned int j = 0; j < typeChildren->getLength(); j++)
				{
					//Reading Type nodes 
					XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *typeNode = typeChildren->item(j);

					if (CheckNodeElement(typeNode, "Landmark"))
					{
						newVect.push_back(typeNode->getTextContent());
						PushUniqueItem(typeNode->getTextContent());
					}
				}

				m_LandmarkGroupVect.push_back(newVect);
			}
		}
	}
	catch (const  XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		return ALBA_ERROR;
	}
	catch (const  XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch)
	{
		return ALBA_ERROR;
	}
	catch (...)
	{
		return ALBA_ERROR;
	}

	cppDEL(errReporter);
	delete XMLParser;

	// terminate the XML library
	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(_("Configuration file Loaded"));

	return ALBA_OK;
}
//---------------------------------------------------------------------------
int albaOpAddLandmark::SaveLandmarksDefinitions(const char *landmarksFileName)
{	
	try // Open the file xml
	{
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		// Do your failure processing here
		return ALBA_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
	XMLCh tempStr[100];
	XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("LS", tempStr, 99);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(tempStr);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* theSerializer = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)impl)->createDOMWriter();
	theSerializer->setNewLine(albaXMLString("\r"));

	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);

	doc = impl->createDocument(NULL, albaXMLString("Landmarks_Dictionary"), NULL);

	doc->setEncoding(albaXMLString("UTF-8"));
	doc->setStandalone(true);
	doc->setVersion(albaXMLString("1.0"));

	// extract root element and wrap it with an albaXMLElement object
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
	assert(root);

	// attach version attribute to the root node
	root->setAttribute(albaXMLString("Name"), albaXMLString("nmsBuilder_Landmarks"));
	root->setAttribute(albaXMLString("Version"), albaXMLString("1.0"));

	// GROUP
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *firtsGroup = doc->createElement(albaXMLString("Group"));
	firtsGroup->setAttribute(albaXMLString("Name"), albaXMLString(m_Cloud->GetName()));

	// Create all items List - Local items + Dictionary (if loaded)
	std::set<wxString> allItems;

	for (int i = 0; i < m_LandmarkNameVect.size(); i++)
	{
		allItems.insert(m_LandmarkNameVect[i]);
	}

	if (m_LandmarkGroupVect.size() > 0)
		for (int i = 0; i < m_LandmarkGroupVect[0].size(); i++)
		{
			allItems.insert(m_LandmarkGroupVect[0][i]);
		}

	std::set<wxString>::iterator it;
	for (it=allItems.begin(); it != allItems.end(); it++)
	{
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *elem = doc->createElement(albaXMLString("Landmark"));

		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node = doc->createTextNode(albaXMLString("Landmark"));
		node->setNodeValue(albaXMLString(*it));
		elem->appendChild(node);

		firtsGroup->appendChild(elem);
	}

	root->appendChild(firtsGroup);

	int lastSelection = m_GroupComboBox->GetSelection();

	for (int i=1; i< m_LandmarkGroupVect.size(); i++)
	{
		m_GroupComboBox->Select(i);
		wxString name = m_GroupComboBox->GetValue();

		// GROUP
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *group = doc->createElement(albaXMLString("Group"));
		group->setAttribute(albaXMLString("Name"), albaXMLString(name));

		for (int j=0; j < m_LandmarkGroupVect[i].size(); j++)
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *elem = doc->createElement(albaXMLString("Landmark"));

			XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node = doc->createTextNode(albaXMLString("Landmark"));
			node->setNodeValue(albaXMLString(m_LandmarkGroupVect[i][j]));
			elem->appendChild(node);

			group->appendChild(elem);
		}
		root->appendChild(group);
	}

	m_GroupComboBox->Select(lastSelection);

	//

	XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *XMLTarget;
	albaString fileName = landmarksFileName;

	XMLTarget = new XERCES_CPP_NAMESPACE_QUALIFIER LocalFileFormatTarget(fileName);

	try
	{
		// do the serialization through DOMWriter::writeNode();
		theSerializer->writeNode(XMLTarget, *doc);
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER  XMLException& toCatch)
	{
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.getMessage());
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return ALBA_ERROR;
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch)
	{
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.msg);
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return ALBA_ERROR;
	}
	catch (...) {
		return ALBA_ERROR;
	}

	theSerializer->release();
	cppDEL(XMLTarget);
	doc->release();

	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	albaLogMessage(wxString::Format("Landmarks Dictionary has been written %s", fileName.GetCStr()));

	return ALBA_OK;
}
//---------------------------------------------------------------------------
bool albaOpAddLandmark::CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName)
{
	//Reading nodes
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return false;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *nodeElement = (XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)node;
	albaString nameElement = "";
	nameElement = albaXMLString(nodeElement->getTagName());

	return (nameElement == elementName);
}
//--------------------------------------------------------------------------
albaString albaOpAddLandmark::GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName)
{
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return "";

	return albaXMLString(((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *)node)->getAttribute(albaXMLString(attributeName)));
}

/// VECTOR UTILITIES
//---------------------------------------------------------------------------
void albaOpAddLandmark::PushUniqueItem(wxString item)
{
	for (int i = 0; i < m_LandmarkNameVect.size(); i++)
	{
		if (item == m_LandmarkNameVect[i])
			return;
	}

	m_LandmarkNameVect.push_back(item);
}
//---------------------------------------------------------------------------
void albaOpAddLandmark::ReplaceItem(wxString oldItem, wxString newItem)
{
	for (int i = 0; i < m_LandmarkNameVect.size(); i++)
	{
		if (oldItem == m_LandmarkNameVect[i])
		{
			m_LandmarkNameVect[i] = newItem;
			break;
		}
	}

	for (int i = 0; i < m_LandmarkGroupVect[m_SelectedGroup].size(); i++)
	{
		if (oldItem == m_LandmarkGroupVect[m_SelectedGroup][i])
		{
			m_LandmarkGroupVect[m_SelectedGroup][i] = newItem;
			break;
		}
	}
}
//---------------------------------------------------------------------------
void albaOpAddLandmark::RemoveItem(wxString item)
{
	for (int i = 0; i < m_LandmarkNameVect.size(); i++)
	{
		if (item == m_LandmarkNameVect[i])
		{
			m_LandmarkNameVect.erase(m_LandmarkNameVect.begin() + i);
			break;
		}
	}

	for (int i = 0; i < m_LandmarkGroupVect[m_SelectedGroup].size(); i++)
	{
		if (item == m_LandmarkGroupVect[m_SelectedGroup][i])
		{
			m_LandmarkGroupVect[m_SelectedGroup].erase(m_LandmarkGroupVect[m_SelectedGroup].begin() + i);
			break;
		}
	}
}
