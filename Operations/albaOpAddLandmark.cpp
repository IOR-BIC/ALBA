/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpAddLandmark
 Authors: Paolo Quadrani, Nicola Vanella
 
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

#include "albaOpAddLandmark.h"
#include "albaDecl.h"
#include "albaEvent.h"

#include "albaInteractorPicker.h"
#include "albaInteractor.h"
#include "albaGUI.h"

#include <set>

#include "albaVME.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMESurfaceParametric.h"

#include "vtkPoints.h"
#include "vtkDataSet.h"

#include "albaSideBar.h"
#include "albaGUIHolder.h"
#include "albaGUISplittedPanel.h"
#include "albaGUINamedPanel.h"
#include "albaGUIDictionaryWidget.h"

#include "albaXMLString.h"
#include "xercesc\util\PlatformUtils.hpp"
#include "xercesc\parsers\XercesDOMParser.hpp"
#include "mmuDOMTreeErrorReporter.h"
#include "mmaMaterial.h"
#include "xercesc\util\XercesDefs.hpp"
#include "xercesc\framework\LocalFileFormatTarget.hpp"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpAddLandmark);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaOpAddLandmark::albaOpAddLandmark(const wxString &label) :
albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_PickedVme = NULL;
	m_Cloud = NULL;

	m_LandmarkPicker = NULL;
	m_OldBehavior = NULL;

	m_LandmarkUndoVetc.clear();
	m_LandmarkRedoVect.clear();
	m_LandmarkNameVect.clear();
	m_LocalLandmarkNameVect.clear();
	m_AllItemsNameVect.clear();

	m_CurrentLandmark = NULL;
	m_SelectedLandmark = NULL;
	m_SelectedLandmarkCloud = NULL;
	m_Dict = NULL;

	m_CloudName = "New landmark cloud";
	m_LandmarkName = "";
	m_SelectedLandmarkName = "Add_New_Landmark";
	m_LandmarkNameCount = 1;
	m_Radius = -1;

	m_RemoveMessage = "";

	m_ShowMode = 0;
	m_ShowAllMode = true;

	m_AddModeActive = true;
	m_CloudCreatedFlag = false;
	m_HasSelection = false;
	m_AddLandmarkMode = true;
	m_FirstOpDo = true;
	m_DictionaryLoaded = false;

	m_LandmarkPosition[0] = m_LandmarkPosition[1] = m_LandmarkPosition[2] = 0;
}
//----------------------------------------------------------------------------
albaOpAddLandmark::~albaOpAddLandmark()
{
	m_LandmarkNameVect.clear();
	m_LocalLandmarkNameVect.clear();
	m_AllItemsNameVect.clear();

	for (int i=0; i< m_LandmarkUndoVetc.size(); i++)
	{
		albaDEL(m_LandmarkUndoVetc[i]);
	}

	m_LandmarkUndoVetc.clear();

	for (int i = 0; i < m_LandmarkRedoVect.size(); i++)
	{
		albaDEL(m_LandmarkRedoVect[i]);
	}

	m_LandmarkRedoVect.clear();

	albaDEL(m_LandmarkPicker);

	if (m_CloudCreatedFlag)
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
//----------------------------------------------------------------------------
{
	return (node != NULL); //accept all other vmes
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::OpRun()
{
	if (m_Input)
	{
		if (m_Input->IsALBAType(albaVMELandmarkCloud))
		{
			// Add a new landmark to existent landmarkCloud
			m_Cloud = (albaVMELandmarkCloud*)m_Input;
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
			m_PickedVme = m_Input;

			albaNEW(m_Cloud);

			if (m_TestMode)
			{
				m_Cloud->TestModeOn();
			}

			m_Cloud->SetName(m_CloudName);

			vtkDataSet * vtkData = m_PickedVme->GetOutput()->GetVTKData();
			if (vtkData)
				m_Cloud->SetRadius(vtkData->GetLength() / 60.0);
			else
				m_Cloud->SetRadius(7.0);

			m_Cloud->ReparentTo(m_PickedVme);

			m_CloudCreatedFlag = true;
		}

		GetLogicManager()->VmeShow(m_Cloud, true);
		GetLogicManager()->VmeShow(m_PickedVme, true);

		// Customize m_PickedVme behavior
		m_LandmarkPicker = albaInteractorPERPicker::New();

		albaEventMacro(albaEvent(this, PER_PUSH, (albaObject *)(m_LandmarkPicker)));
		m_LandmarkPicker->SetListener(this);

		// Create Selected LandmarkCloud
		albaNEW(m_SelectedLandmarkCloud);
		albaNEW(m_SelectedLandmark);
		m_SelectedLandmarkCloud->ReparentTo(m_PickedVme); 
		m_SelectedLandmark->ReparentTo(m_SelectedLandmarkCloud);

		if (m_TestMode)
		{
			m_SelectedLandmarkCloud->TestModeOn();
		}

		m_SelectedLandmarkCloud->SetName(_("Selected Landmark cloud"));
		m_SelectedLandmark->SetName(_("Selected Landmark"));

		m_SelectedLandmarkCloud->SetRadius(m_Cloud->GetRadius());		
	}

	// Set Selection color 
	mmaMaterial *select_material = m_SelectedLandmarkCloud->GetMaterial();
	SetMaterialRGBA(select_material, 0.1, 0.1, 1.0, 0.8);

	mmaMaterial *material = m_Cloud->GetMaterial();

	// Save old color
	m_OldColorCloud[0] = material->m_Diffuse[0];
	m_OldColorCloud[1] = material->m_Diffuse[1];
	m_OldColorCloud[2] = material->m_Diffuse[2];
	m_OldColorCloud[3] = material->m_Opacity;
	
	// Set Landmarks color 
	SetMaterialRGBA(material, 1.0, 0.1, 0.1, 0.8);

	GetLogicManager()->CameraUpdate();

	LoadLandmarksFromVME();

	if (!GetTestMode())
	{ 
		CreateGui();
	}
	
	//m_LandmarkNameVect.push_back(m_LocalLandmarkNameVect);
	ShowLandmarkGroup();
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::OpDo()
{
	int reparent_result = ALBA_OK;

	//On opstop we run the first opdo and the lmc is already ok
	if (!m_FirstOpDo)
	{
		if (m_CloudCreatedFlag)
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
	if (m_CloudCreatedFlag)
	{
		GetLogicManager()->VmeRemove(m_Cloud);
	}
	else
	{
		RestoreLandmarkVect(m_LandmarkUndoVetc);
	}
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::SetCloudName(albaString name)
{
	m_CloudName = name;

	if(m_Cloud)
		m_Cloud->SetName(name);
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

//----------------------------------------------------------------------------
void albaOpAddLandmark::CreateGui()
{
	// setup Gui
	m_Gui = new albaGUI(this);
	m_Gui->SetListener(this);

	const wxString choices[] = { "Show All" };
	m_ShowComboBox = m_Gui->Combo(ID_SHOW_GROUP, "", &m_ShowMode, 1, choices);
	m_Gui->Divider(2);

	// setup dictionary
	m_Dict = new albaGUIDictionaryWidget(m_Gui, -1);
	m_Dict->SetListener(this);
	m_Dict->SetCloud(m_Cloud);
	m_Dict->InitDictionary(NULL);
	m_Dict->SetTitle("Landmarks List");
	m_Gui->Add(m_Dict->GetWidget(), wxEXPAND);

	m_Gui->Divider();
	m_Gui->Divider(2);

	m_Gui->Divider();
	m_Gui->Button(ID_REMOVE_LANDMARK, "Remove");
	m_Gui->Label(&m_RemoveMessage, true);
	m_Gui->Divider();

	m_Gui->Divider(2);

	m_Gui->Label(_("Landmark Props"));
	m_Gui->String(ID_CHANGE_NAME, _("Name"), &m_LandmarkName);
	m_Gui->Vector(ID_CHANGE_POSITION, _("Position"), m_LandmarkPosition, MINFLOAT, MAXFLOAT, 2, _("landmark position"));

	albaString tooltip(_("If checked, add the landmark to the current time. \nOtherwise add the landmark at time = 0"));
	m_Gui->Bool(ID_CHANGE_TIME, _("Current time"), &m_AddToCurrentTime, 1, tooltip);
	m_Gui->Divider(2);

	m_Gui->Enable(ID_REMOVE_LANDMARK, false);
	m_Gui->Enable(ID_CHANGE_NAME, false);
	m_Gui->Enable(ID_CHANGE_POSITION, false);

	m_Gui->Label(_("Dictionary"));
	m_Gui->Button(ID_LOAD, _("Load dictionary"));
	m_Gui->Button(ID_SAVE, _("Save dictionary"));	

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");

	m_Gui->OkCancel();
	m_Gui->Label("");
	m_Gui->Label("");

	ShowGui();
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::OnEvent(albaEventBase *alba_event)
{
  vtkPoints *pts = NULL; 
  if(albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_LOAD:
			{
				LoadDictionary();
			}
		  break;

			case ID_SAVE:
			{
				SaveDictionary();
			}
			break;

			case ID_SHOW_GROUP:
			{
				ShowLandmarkGroup();
			}
			break;
      
			case ITEM_SELECTED:
			{
				SelectLandmark(*(e->GetString()));
			}
			break;

			case VME_PICKED:
			{
				pts = vtkPoints::SafeDownCast(e->GetVtkObj());
				albaVME *pickedVME = e->GetVme();
				if (pts && pickedVME!=m_Cloud)
				{
					pts->GetPoint(0, m_LandmarkPosition);

					if (m_AddLandmarkMode)
					{
						AddLandmark(m_LandmarkPosition);

						if (m_Gui && !GetTestMode())
						{
							m_Dict->SetCloud(m_Cloud);

							albaString nextItem = "Add_New_Landmark";
							int pos = m_Dict->GetItemIndex(m_SelectedLandmarkName);
							if (pos < m_Dict->GetSize() - 2)
							{
								nextItem = m_Dict->GetItemByIndex(pos + 1);
							}

							if (!m_AddModeActive && nextItem == "Add_New_Landmark")
							{
								nextItem = m_Dict->GetItemByIndex(m_Dict->GetSize() - 1);
							}

							SelectLandmark(nextItem);
						}
					}
					else
					{
						m_SelectedLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
						m_CurrentLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
					}
				}

				m_Gui->Update();
				GetLogicManager()->CameraUpdate();
			}
			break;

			case ID_REMOVE_LANDMARK:
			{
				RemoveLandmark();
			}
			break;

			case ID_CHANGE_NAME:
			{
				if (this->m_Cloud && m_CurrentLandmark)
					if (!this->m_Cloud->FindInTreeByName(m_LandmarkName))
					{
						m_CurrentLandmark->SetName(m_LandmarkName);

						// Update local landmark name
						for (int i = 0; i < m_LocalLandmarkNameVect.size(); i++)
						{
							if (m_LocalLandmarkNameVect[i] == m_SelectedLandmarkName)
							{
								m_LocalLandmarkNameVect[i] = m_LandmarkName;
								break;
							}
						}

						if (m_Gui && !GetTestMode())
						{
							m_Dict->UpdateItem(m_SelectedLandmarkName, m_LandmarkName);
						}
					}
					else
					{
						wxString existing_lm_msg(_("Landmark with that name already exist, Please change it!"));
						wxMessageBox(existing_lm_msg, _("Warning"), wxOK | wxICON_WARNING, NULL);
					}
			}
		  case ID_CHANGE_POSITION:
			{
				if (this->m_Cloud && m_CurrentLandmark)
				{
					m_CurrentLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
					m_SelectedLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
				}

				GetLogicManager()->CameraUpdate();

				if (m_Gui && !GetTestMode())
				{
					m_Gui->Update();
				}
			}
			break;

      case wxOK:
			{
				OpStop(OP_RUN_OK);
			}
      break;

      case wxCANCEL:
			{
				OpStop(OP_RUN_CANCEL);
			}
      break;

      default:
        albaEventMacro(*e);
      break; 
    }
  }
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::OpStop(int result)
{
	// Reset Landmarks color 
	mmaMaterial *material = m_Cloud->GetMaterial();
	SetMaterialRGBA(material, m_OldColorCloud[0], m_OldColorCloud[1], m_OldColorCloud[2], m_OldColorCloud[3]);

	if (m_CurrentLandmark)
	{
		DeselectLandmark();
	}

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

	//m_LocalLandmarkNameVect.clear();

	albaEventMacro(albaEvent(this, PER_POP));
	albaDEL(m_LandmarkPicker);

	cppDEL(m_Dict);

	if (!m_TestMode)
	{
		HideGui();
	}

	m_SelectedLandmarkCloud->ReparentTo(NULL);
	albaDEL(m_SelectedLandmarkCloud);

	albaEventMacro(albaEvent(this, result));
}

// LANDMARK MANAGER
//----------------------------------------------------------------------------
void albaOpAddLandmark::AddLandmark(double pos[3])
{
	if (m_Gui && !GetTestMode())
	{
		m_Dict->RemoveItem("Add_New_Landmark");
	}

	if (m_SelectedLandmarkName == "Add_New_Landmark")
	{
		// Create New Landmark
		char printStr[100];
		sprintf(printStr, "New_Landmark_%d", m_LandmarkNameCount);

		m_LandmarkName = printStr;
		m_SelectedLandmarkName = m_LandmarkName;

		if (m_Gui && !GetTestMode())
		{
			m_Dict->AddItem((wxString)m_LandmarkName);
		}

		m_LocalLandmarkNameVect.push_back((wxString)m_LandmarkName);

		m_LandmarkNameCount++;
	}
	else
	{
		m_LandmarkName = m_SelectedLandmarkName;
	}

	//
  m_LandmarkPosition[0] = pos[0];
  m_LandmarkPosition[1] = pos[1];
  m_LandmarkPosition[2] = pos[2];
  
  albaSmartPointer<albaVMELandmark> landmark;
  landmark->SetName(m_LandmarkName.GetCStr());
  landmark->ReparentTo(m_Cloud);

  if(NULL != m_PickedVme)
		landmark->SetTimeStamp(m_PickedVme->GetTimeStamp());

	if (m_AddToCurrentTime)
		landmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
	else
		landmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0, 0);

	if (m_Radius > 0)
	{
		landmark->SetRadius(m_Radius);
		m_SelectedLandmarkCloud->SetRadius(m_Radius);
	}

	landmark->Update();

  GetLogicManager()->VmeShow(landmark.GetPointer(), true);
	GetLogicManager()->CameraUpdate();

  if (m_Gui && !GetTestMode())
  {
		if(m_AddModeActive)
			m_Dict->AddItem("Add_New_Landmark");

		CheckEnableOkCondition();
  }
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::RemoveLandmark()
{	
	if (m_CurrentLandmark)
	{
		albaVME* item = m_CurrentLandmark;
		albaString itemName = item->GetName();

		int itemToRemove = -1;
		for (int i = 0; i < m_LocalLandmarkNameVect.size(); i++)
		{
			if (m_LocalLandmarkNameVect[i] == itemName)
			{
				itemToRemove = i;
				break;
			}
		}

		// Find item in dictionary (if loaded)
		if (m_LandmarkNameVect.size() > 0)
		{
			for (int i = 0; i < m_LandmarkNameVect[0].size(); i++)
			{
				if (m_LandmarkNameVect[0][i] == itemName)
				{
					itemToRemove = -1;
					break;
				}
			}
		}

		// Item found and it is not in dictionary - I can remove it
		if (itemToRemove >= 0)
		{
			DeselectLandmark();

			GetLogicManager()->VmeRemove(item);

			m_LocalLandmarkNameVect.erase(m_LocalLandmarkNameVect.begin() + itemToRemove);

			if (m_Gui && !GetTestMode())
			{
				m_Dict->RemoveItem(itemName);
				SelectLandmark("Add_New_Landmark");
			}

			GetLogicManager()->CameraUpdate();
		}
	}
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::SelectLandmark(albaString selection)
{
	if (m_CurrentLandmark)
	{
		GetLogicManager()->VmeShow(m_CurrentLandmark, true);
		GetLogicManager()->VmeShow(m_SelectedLandmarkCloud->GetLandmark(0), false);

		m_CurrentLandmark = NULL;
	}
		
	if (m_Gui && !GetTestMode())
	{
		m_RemoveMessage = "";
		m_Dict->DeselectItem(m_SelectedLandmarkName);
		int pos=m_Dict->SelectItem(selection);

		m_AddLandmarkMode = (pos == m_Dict->GetSize() - 1) && m_AddModeActive;
		
		if (m_DictionaryLoaded && !m_AddLandmarkMode && m_Cloud->GetLandmark(selection) == NULL)
			m_AddLandmarkMode = true;

		if(m_AddLandmarkMode)
			m_Dict->SetTitle("Add landmark");
		else
			m_Dict->SetTitle("Edit landmark");
		
		m_Gui->Enable(ID_CHANGE_NAME, !m_AddLandmarkMode);
		m_Gui->Enable(ID_CHANGE_POSITION, !m_AddLandmarkMode);
		m_Gui->Enable(ID_REMOVE_LANDMARK, !m_AddLandmarkMode);

		// Find item in dictionary
		if (m_LandmarkNameVect.size() > 0)
		{
			for (int i = 0; i < m_LandmarkNameVect[0].size(); i++)
			{
				if (m_LandmarkNameVect[0][i] == selection)
				{
					m_Gui->Enable(ID_CHANGE_NAME, false);
					m_Gui->Enable(ID_REMOVE_LANDMARK, false);
					m_RemoveMessage = "Landmark is a Dictionary entry";
					break;
				}
			}
		}

		m_Gui->Update();
	}

	m_SelectedLandmarkName = m_LandmarkName = selection;

	albaVMELandmark *landmark = albaVMELandmark::SafeDownCast(this->m_Cloud->FindInTreeByName(m_SelectedLandmarkName.GetCStr()));

	if (this->m_Cloud && landmark)
	{
		m_CurrentLandmark = landmark;
		m_CurrentLandmark->GetPoint(m_LandmarkPosition);
		m_SelectedLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
		m_SelectedLandmark->Update();

		GetLogicManager()->VmeShow(m_CurrentLandmark, false);
		GetLogicManager()->VmeShow(m_SelectedLandmarkCloud->GetLandmark(0), true);

		albaVME::albaVMESet dependenciesVMEs = landmark->GetDependenciesVMEs();

		if (!dependenciesVMEs.empty())
		{
			m_Gui->Enable(ID_REMOVE_LANDMARK, false);
			m_RemoveMessage = "Landmark has dependency";
			wxString message;
		}

		m_HasSelection = true;
	}

	if (m_Gui && !GetTestMode())
	{
		m_Gui->Update();
	}

	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaOpAddLandmark::DeselectLandmark()
{
	if (m_CurrentLandmark)
	{
		GetLogicManager()->VmeShow(m_CurrentLandmark, true);
		GetLogicManager()->VmeShow(m_SelectedLandmarkCloud->GetLandmark(0), true);
		m_CurrentLandmark = NULL;
	}

	if (m_Gui && !GetTestMode())
	{
		m_RemoveMessage = "";
		m_Dict->DeselectItem(m_SelectedLandmarkName);
	}

	GetLogicManager()->CameraUpdate();

	m_HasSelection = false;
}

//----------------------------------------------------------------------------
void albaOpAddLandmark::SetMaterialRGBA(mmaMaterial *material, double r, double g, double b, double a)
{
	material->m_Diffuse[0] = r;
	material->m_Diffuse[1] = g;
	material->m_Diffuse[2] = b;
	material->m_Opacity = a;
	material->UpdateProp();
}

// SAVE-LOAD LANDMARK DEFINITIONS
//----------------------------------------------------------------------------
void albaOpAddLandmark::LoadDictionary(wxString fileName)
{
	if (fileName == "")
	{
		wxString wild_dict = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
		wxString dict = albaGetLastUserFolder().c_str();

		fileName = albaGetOpenFile(dict, wild_dict, "Choose Dictionary File").c_str();
	}

	if (fileName != "") 
		LoadLandmarksDefinitions(fileName);

	m_DictionaryLoaded = true;
	m_ShowMode = 0;

	ShowLandmarkGroup();
}
//----------------------------------------------------------------------------
int albaOpAddLandmark::SaveDictionary(wxString fileName)
{
	if (fileName == "")
	{
		albaString initialFileName;
		initialFileName = albaGetLastUserFolder().c_str();
		initialFileName.Append("\\newLandmarkDictionary.xml");

		albaString wildc = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
		fileName = albaGetSaveFile(initialFileName.GetCStr(), wildc).c_str();
	}

	if (fileName != "") 
		return SaveLandmarksDefinitions(fileName);
}

//---------------------------------------------------------------------------
int albaOpAddLandmark::SaveLandmarksDefinitions(const char *landmarksFileName)
{
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

	//

	// Create all items List - Local items + Dictionary (if loaded)
	std::set<wxString> allItems;

	for (int i = 0; i < m_LocalLandmarkNameVect.size(); i++)
	{
		allItems.insert(m_LocalLandmarkNameVect[i]);
	}

	if (m_LandmarkNameVect.size() > 0)
		for (int i = 0; i < m_LandmarkNameVect[0].size(); i++)
		{
			allItems.insert(m_LandmarkNameVect[0][i]);
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

	int lastSelection = m_ShowComboBox->GetSelection();

	for (int i=1; i< m_LandmarkNameVect.size(); i++)
	{
		m_ShowComboBox->Select(i);
		wxString name = m_ShowComboBox->GetValue();

		// GROUP
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *group = doc->createElement(albaXMLString("Group"));
		group->setAttribute(albaXMLString("Name"), albaXMLString(name));

		for (int j=0; j < m_LandmarkNameVect[i].size(); j++)
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *elem = doc->createElement(albaXMLString("Landmark"));

			XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node = doc->createTextNode(albaXMLString("Landmark"));
			node->setNodeValue(albaXMLString(m_LandmarkNameVect[i][j]));
			elem->appendChild(node);

			group->appendChild(elem);
		}
		root->appendChild(group);
	}

	m_ShowComboBox->Select(lastSelection);

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

//----------------------------------------------------------------------------
int albaOpAddLandmark::LoadLandmarksDefinitions(wxString fileName)
{
	m_LandmarkNameVect.clear();
	StringVector firstVect;
	m_LandmarkNameVect.push_back(m_LocalLandmarkNameVect);
	m_LandmarkNameVect[0].clear();

	StringVector groupNameVect;

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

				groupNameVect.push_back(dicName);

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

				m_LandmarkNameVect.push_back(newVect);
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

	// Create All Items Vect
	for (int i = 0; i < m_AllItemsNameVect.size(); i++)
		m_LandmarkNameVect[0].push_back(m_AllItemsNameVect[i]);
	
	if (!m_TestMode)
	{
		// Update GUI ComboBox
		m_ShowComboBox->Clear();
		m_ShowComboBox->AppendString("Show All");
	}

	if (m_Gui && !GetTestMode())
	{
		m_Dict->SetTitle("Show All");
	}

	for (int g = 0; g < groupNameVect.size(); g++)
	{
		m_ShowComboBox->AppendString(groupNameVect[g]);
	}

	groupNameVect.clear();

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

//--------------------------------------------------------------------------
int albaOpAddLandmark::LoadLandmarksFromVME()
{
	m_LocalLandmarkNameVect.clear();
	int nLandmarks = m_Cloud->GetNumberOfLandmarks();

	for (int i = 0; i < nLandmarks; i++)
	{
		albaVMELandmark *lm = m_Cloud->GetLandmark(i);

		m_LocalLandmarkNameVect.push_back(lm->GetName());

		// Create Undo Vector
		albaVMELandmark* newLandmark = NULL;
		albaNEW(newLandmark);
		newLandmark->DeepCopy(lm);
		m_LandmarkUndoVetc.push_back(newLandmark);

		wxString name = lm->GetName();
		int pos = name.Find("New_Landmark_");
		if (pos>=0)
		{
			wxString subName = name.SubString(pos+13, name.Length());
				
			int val = atoi(subName);

			if(m_LandmarkNameCount < val)
				m_LandmarkNameCount = val + 1;
		}
	}

	return ALBA_OK;
}
//---------------------------------------------------------------------------
void albaOpAddLandmark::ShowLandmarkGroup()
{
	if (m_CurrentLandmark)
	{
		GetLogicManager()->VmeShow(m_CurrentLandmark, true);
		GetLogicManager()->VmeShow(m_SelectedLandmarkCloud->GetLandmark(0), true);
		m_CurrentLandmark = NULL;
	}

	wxString firstSelection = "";

	if (!m_TestMode)
	{
		if (m_ShowMode == 0)
		{
			for (int i = 0; i < m_LocalLandmarkNameVect.size(); i++)
			{				
				PushUniqueItem(m_LocalLandmarkNameVect[i]);
			}

			if (m_LandmarkNameVect.size() > 0)
				for (int i = 0; i < m_LandmarkNameVect[0].size(); i++)
				{
					PushUniqueItem(m_LandmarkNameVect[0][i]);
				}

			if (m_AllItemsNameVect.size() > 0) firstSelection = m_AllItemsNameVect[0];

			m_Dict->InitDictionary(&m_AllItemsNameVect);
		}
		else
		{
			m_Dict->InitDictionary(&m_LandmarkNameVect[m_ShowMode]);
		}

		m_Dict->SetTitle("Select landmark");

		if (m_AddModeActive)
		{
			m_Dict->AddItem("Add_New_Landmark");
			m_Dict->SelectItem("Add_New_Landmark");
		}
		else
		{
			SelectLandmark(firstSelection);
		}

		m_Gui->Enable(ID_SHOW_GROUP, true);
		m_Gui->Update();

		CheckEnableOkCondition();
	}
}
//---------------------------------------------------------------------------
void albaOpAddLandmark::CheckEnableOkCondition()
{
	if (m_ShowMode == 0 && m_ShowAllMode)
	{
		m_Gui->Enable(wxOK, true);
	}
	else
	{
		bool res = true;

		if(m_LandmarkNameVect.size()>0)
			for (int i = 0; i < m_LandmarkNameVect[m_ShowMode].size(); i++)
			{
				if (!this->m_Cloud->FindInTreeByName(m_LandmarkNameVect[m_ShowMode][i]))
				{
					res = false;
					break;
				}
			}

		m_Gui->Enable(wxOK, res);
	}
}

//---------------------------------------------------------------------------
void albaOpAddLandmark::PushUniqueItem(wxString item)
{
	for (int i=0; i<m_AllItemsNameVect.size(); i++)
	{
		if (item == m_AllItemsNameVect[i])
			return;
	}

	m_AllItemsNameVect.push_back(item);
}
