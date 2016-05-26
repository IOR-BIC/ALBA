/*=========================================================================

 Program: MAF2
 Module: mafOpAddLandmark
 Authors: Paolo Quadrani, Nicola Vanella
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpAddLandmark.h"
#include "mafDecl.h"
#include "mafEvent.h"

#include "mafInteractorPicker.h"
#include "mafInteractor.h"
#include "mafGUI.h"

#include <set>

#include "mafVME.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurfaceParametric.h"

#include "vtkPoints.h"
#include "vtkDataSet.h"

#include "mafSideBar.h"
#include "mafGUIHolder.h"
#include "mafGUISplittedPanel.h"
#include "mafGUINamedPanel.h"
#include "mafGUIDictionaryWidget.h"

#include "mafXMLString.h"
#include "xercesc\util\PlatformUtils.hpp"
#include "xercesc\parsers\XercesDOMParser.hpp"
#include "mmuDOMTreeErrorReporter.h"
#include "mmaMaterial.h"
#include "xercesc\util\XercesDefs.hpp"
#include "xercesc\framework\LocalFileFormatTarget.hpp"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpAddLandmark);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpAddLandmark::mafOpAddLandmark(const wxString &label) :
mafOp(label)
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

	m_CurrentLandmark = NULL;
	m_SelectedLandmark = NULL;
	m_SelectedLandmarkCloud = NULL;
	m_Dict = NULL;

	m_LandmarkName = "";
	m_SelectedLandmarkName = "Add_New_Landmark";
	m_LandmarkNameCount = 1;

	m_RemoveMessage = "";

	m_ShowMode = 0;

	m_AddModeActive = true;
	m_CloudCreatedFlag = false;
	m_HasSelection = false;
	m_AddLandmarkMode = true;
	m_FirstOpDo = true;
	m_DictionaryLoaded = false;

	m_LandmarkPosition[0] = m_LandmarkPosition[1] = m_LandmarkPosition[2] = 0;
}
//----------------------------------------------------------------------------
mafOpAddLandmark::~mafOpAddLandmark()
{
	m_LandmarkNameVect.clear();
	m_LocalLandmarkNameVect.clear();

	for (int i=0; i< m_LandmarkUndoVetc.size(); i++)
	{
		mafDEL(m_LandmarkUndoVetc[i]);
	}

	m_LandmarkUndoVetc.clear();

	for (int i = 0; i < m_LandmarkRedoVect.size(); i++)
	{
		mafDEL(m_LandmarkRedoVect[i]);
	}

	m_LandmarkRedoVect.clear();

	mafDEL(m_LandmarkPicker);

	if (m_CloudCreatedFlag)
		mafDEL(m_Cloud);
}
//----------------------------------------------------------------------------
mafOp* mafOpAddLandmark::Copy()   
{
	mafOpAddLandmark *op = new mafOpAddLandmark(m_Label);
  op->m_OpType = m_OpType;
	op->m_Canundo = m_Canundo;
	return op;
}
//----------------------------------------------------------------------------
bool mafOpAddLandmark::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
	return (node != NULL); //accept all other vmes
}

//----------------------------------------------------------------------------
void mafOpAddLandmark::OpRun()
{
	if (m_Input)
	{
		if (m_Input->IsMAFType(mafVMELandmarkCloud))
		{
			// Add a new landmark to existent landmarkCloud
			m_Cloud = (mafVMELandmarkCloud*)m_Input;
			m_PickedVme = m_Input->GetParent();
		}
		else if (m_Input->IsMAFType(mafVMELandmark))
		{
			// Add a new landmark as brother of this one
			m_Cloud = mafVMELandmarkCloud::SafeDownCast(m_Input->GetParent());
			if (m_Cloud)
				m_PickedVme = m_Cloud->GetParent();
		}
		else
		{
			m_PickedVme = m_Input;

			mafNEW(m_Cloud);

			if (m_TestMode)
			{
				m_Cloud->TestModeOn();
			}

			m_Cloud->SetName(_("New landmark cloud"));

			vtkDataSet * vtkData = m_PickedVme->GetOutput()->GetVTKData();
			if (vtkData)
				m_Cloud->SetRadius(vtkData->GetLength() / 60.0);
			else
				m_Cloud->SetRadius(7.0);

			m_Cloud->ReparentTo(m_PickedVme);

			m_CloudCreatedFlag = true;
		}

		mafEventMacro(mafEvent(this, VME_SHOW, m_Cloud, true));
		mafEventMacro(mafEvent(this, VME_SHOW, m_PickedVme, true));

		// Customize m_PickedVme behavior
		m_LandmarkPicker = mafInteractorPERPicker::New();

		mafEventMacro(mafEvent(this, PER_PUSH, (mafObject *)(m_LandmarkPicker)));
		m_LandmarkPicker->SetListener(this);

		// Create Selected LandmarkCloud
		mafNEW(m_SelectedLandmarkCloud);
		mafNEW(m_SelectedLandmark);
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

	LoadLandmarksFromVME();

	if (!GetTestMode())
	{ 
		CreateGui();
	}
	
	//m_LandmarkNameVect.push_back(m_LocalLandmarkNameVect);
	ShowLandmarkGroup();
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpDo()
{
	int reparent_result = MAF_OK;

	//On opstop we run the first opdo and the lmc is already ok
	if (!m_FirstOpDo)
	{
		if (m_CloudCreatedFlag)
		{
			reparent_result = m_Cloud->ReparentTo(m_PickedVme);
		}
		else
		{
			mafEventMacro(mafEvent(this, VME_SHOW, m_Cloud, true));
			RestoreLandmarkVect(m_LandmarkRedoVect);
		}
	}

	m_FirstOpDo = false;
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpUndo()
{
	if (m_CloudCreatedFlag)
	{
		mafEventMacro(mafEvent(this, VME_REMOVE, m_Cloud));
	}
	else
	{
		RestoreLandmarkVect(m_LandmarkUndoVetc);
	}
}

//----------------------------------------------------------------------------
void mafOpAddLandmark::RestoreLandmarkVect(std::vector<mafVMELandmark*> &landmarkVect)
{
	while (m_Cloud->GetNumberOfLandmarks())
	{
		mafEventMacro(mafEvent(this, VME_REMOVE, m_Cloud->GetLandmark(0)));
	}

	for (int i = 0; i < landmarkVect.size(); i++)
	{
		mafSmartPointer<mafVMELandmark> newLandmark;
		
		newLandmark->DeepCopy(landmarkVect[i]);
		newLandmark->ReparentTo(m_Cloud);

		mafEventMacro(mafEvent(this, VME_SHOW, newLandmark, true));
	}

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mafOpAddLandmark::CreateGui()
{
	// setup Gui
	m_Gui = new mafGUI(this);
	m_Gui->SetListener(this);

	const wxString choices[] = { "Show All" };
	m_ShowComboBox = m_Gui->Combo(ID_SHOW_GROUP, "", &m_ShowMode, 1, choices);
	m_Gui->Divider(2);

	// setup dictionary
	m_Dict = new mafGUIDictionaryWidget(m_Gui, -1);
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

	mafString tooltip(_("If checked, add the landmark to the current time. \nOtherwise add the landmark at time = 0"));
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
void mafOpAddLandmark::OnEvent(mafEventBase *maf_event)
{
  vtkPoints *pts = NULL; 
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
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

				if (pts)
				{
					pts->GetPoint(0, m_LandmarkPosition);

					if (m_AddLandmarkMode)
					{
						AddLandmark(m_LandmarkPosition);

						if (m_Gui && !GetTestMode())
						{
							m_Dict->SetCloud(m_Cloud);

							mafString nextItem = "Add_New_Landmark";
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
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
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

				mafEventMacro(mafEvent(this, CAMERA_UPDATE));

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
        mafEventMacro(*e);
      break; 
    }
  }
}

//----------------------------------------------------------------------------
void mafOpAddLandmark::OpStop(int result)
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
			mafVMELandmark *lm = m_Cloud->GetLandmark(i);

			// Create Redo Vector
			mafVMELandmark* newLandmark = NULL;
			mafNEW(newLandmark);
			newLandmark->DeepCopy(lm);
			m_LandmarkRedoVect.push_back(newLandmark);
		}
	}
	else
	{
		OpUndo();
	}

	//m_LocalLandmarkNameVect.clear();

	mafEventMacro(mafEvent(this, PER_POP));
	mafDEL(m_LandmarkPicker);

	cppDEL(m_Dict);

	if (!m_TestMode)
	{
		HideGui();
	}

	m_SelectedLandmarkCloud->ReparentTo(NULL);
	mafDEL(m_SelectedLandmarkCloud);

	mafEventMacro(mafEvent(this, result));
}

// LANDMARK MANAGER
//----------------------------------------------------------------------------
void mafOpAddLandmark::AddLandmark(double pos[3])
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
  
  mafSmartPointer<mafVMELandmark> landmark;
  landmark->SetName(m_LandmarkName.GetCStr());
  landmark->ReparentTo(m_Cloud);

  if(NULL != m_PickedVme)
		landmark->SetTimeStamp(m_PickedVme->GetTimeStamp());

	if (m_AddToCurrentTime)
		landmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
	else
		landmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0, 0);

	landmark->Update();

  mafEventMacro(mafEvent(this,VME_SHOW,landmark.GetPointer(),true));
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

  if (m_Gui && !GetTestMode())
  {
		if(m_AddModeActive)
			m_Dict->AddItem("Add_New_Landmark");

		CheckEnableOkCondition();
  }
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::RemoveLandmark()
{	
	if (m_CurrentLandmark)
	{
		mafVME* item = m_CurrentLandmark;
		mafString itemName = item->GetName();

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

			mafEventMacro(mafEvent(this, VME_REMOVE, item));

			m_LocalLandmarkNameVect.erase(m_LocalLandmarkNameVect.begin() + itemToRemove);

			if (m_Gui && !GetTestMode())
			{
				m_Dict->RemoveItem(itemName);
				SelectLandmark("Add_New_Landmark");
			}

			mafEventMacro(mafEvent(this, CAMERA_UPDATE));
		}
	}
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::SelectLandmark(mafString selection)
{
	if (m_CurrentLandmark)
	{
		mafEventMacro(mafEvent(this, VME_SHOW, m_CurrentLandmark, true));
		mafEventMacro(mafEvent(this, VME_SHOW, m_SelectedLandmarkCloud->GetLandmark(0), false));

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

	mafVMELandmark *landmark = mafVMELandmark::SafeDownCast(this->m_Cloud->FindInTreeByName(m_SelectedLandmarkName.GetCStr()));

	if (this->m_Cloud && landmark)
	{
		m_CurrentLandmark = landmark;
		m_CurrentLandmark->GetPoint(m_LandmarkPosition);
		m_SelectedLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
		m_SelectedLandmark->Update();

		mafEventMacro(mafEvent(this, VME_SHOW, m_CurrentLandmark, false));
		mafEventMacro(mafEvent(this, VME_SHOW, m_SelectedLandmarkCloud->GetLandmark(0), true));

		mafVME::mafVMESet dependenciesVMEs = landmark->GetDependenciesVMEs();

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

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::DeselectLandmark()
{
	if (m_CurrentLandmark)
	{
		mafEventMacro(mafEvent(this, VME_SHOW, m_CurrentLandmark, true));
		mafEventMacro(mafEvent(this, VME_SHOW, m_SelectedLandmarkCloud->GetLandmark(0), false));
		m_CurrentLandmark = NULL;
	}

	if (m_Gui && !GetTestMode())
	{
		m_RemoveMessage = "";
		m_Dict->DeselectItem(m_SelectedLandmarkName);
	}

	mafEventMacro(mafEvent(this, CAMERA_UPDATE));

	m_HasSelection = false;
}

//----------------------------------------------------------------------------
void mafOpAddLandmark::SetMaterialRGBA(mmaMaterial *material, double r, double g, double b, double a)
{
	material->m_Diffuse[0] = r;
	material->m_Diffuse[1] = g;
	material->m_Diffuse[2] = b;
	material->m_Opacity = a;
	material->UpdateProp();
}

// SAVE-LOAD LANDMARK DEFINITIONS
//----------------------------------------------------------------------------
void mafOpAddLandmark::LoadDictionary(wxString fileName)
{
	if (fileName == "")
	{
		wxString wild_dict = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
		wxString dict = mafGetLastUserFolder().c_str();

		fileName = mafGetOpenFile(dict, wild_dict, "Choose Dictionary File").c_str();
	}

	if (fileName != "") 
		LoadLandmarksDefinitions(fileName);

	m_DictionaryLoaded = true;
	m_ShowMode = 0;

	ShowLandmarkGroup();
}
//----------------------------------------------------------------------------
int mafOpAddLandmark::SaveDictionary(wxString fileName)
{
	if (fileName == "")
	{
		mafString initialFileName;
		initialFileName = mafGetLastUserFolder().c_str();
		initialFileName.Append("\\newLandmarkDictionary.xml");

		mafString wildc = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
		fileName = mafGetSaveFile(initialFileName.GetCStr(), wildc).c_str();
	}

	if (fileName != "") 
		return SaveLandmarksDefinitions(fileName);
}

//---------------------------------------------------------------------------
int mafOpAddLandmark::SaveLandmarksDefinitions(const char *landmarksFileName)
{
	//Open the file xml
	try
	{
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		// Do your failure processing here
		return MAF_ERROR;
	}

	XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc;
	XMLCh tempStr[100];
	XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode("LS", tempStr, 99);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation *impl = XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationRegistry::getDOMImplementation(tempStr);
	XERCES_CPP_NAMESPACE_QUALIFIER DOMWriter* theSerializer = ((XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementationLS*)impl)->createDOMWriter();
	theSerializer->setNewLine(mafXMLString("\r"));

	if (theSerializer->canSetFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true))
		theSerializer->setFeature(XERCES_CPP_NAMESPACE_QUALIFIER XMLUni::fgDOMWRTFormatPrettyPrint, true);

	doc = impl->createDocument(NULL, mafXMLString("Landmarks_Dictionary"), NULL);

	doc->setEncoding(mafXMLString("UTF-8"));
	doc->setStandalone(true);
	doc->setVersion(mafXMLString("1.0"));

	// extract root element and wrap it with an mafXMLElement object
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
	assert(root);

	// attach version attribute to the root node
	root->setAttribute(mafXMLString("Name"), mafXMLString("nmsBuilder_Landmarks"));
	root->setAttribute(mafXMLString("Version"), mafXMLString("1.0"));

	// GROUP
	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *firtsGroup = doc->createElement(mafXMLString("Group"));
	firtsGroup->setAttribute(mafXMLString("Name"), mafXMLString(m_Cloud->GetName()));

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
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *elem = doc->createElement(mafXMLString("Landmark"));

		XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node = doc->createTextNode(mafXMLString("Landmark"));
		node->setNodeValue(mafXMLString(*it));
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
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *group = doc->createElement(mafXMLString("Group"));
		group->setAttribute(mafXMLString("Name"), mafXMLString(name));

		for (int j=0; j < m_LandmarkNameVect[i].size(); j++)
		{
			XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *elem = doc->createElement(mafXMLString("Landmark"));

			XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node = doc->createTextNode(mafXMLString("Landmark"));
			node->setNodeValue(mafXMLString(m_LandmarkNameVect[i][j]));
			elem->appendChild(node);

			group->appendChild(elem);
		}
		root->appendChild(group);
	}

	m_ShowComboBox->Select(lastSelection);

	//

	XERCES_CPP_NAMESPACE_QUALIFIER XMLFormatTarget *XMLTarget;
	mafString fileName = landmarksFileName;

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
		return MAF_ERROR;
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch)
	{
		char* message = XERCES_CPP_NAMESPACE_QUALIFIER XMLString::transcode(toCatch.msg);
		XERCES_CPP_NAMESPACE_QUALIFIER XMLString::release(&message);
		return MAF_ERROR;
	}
	catch (...) {
		return MAF_ERROR;
	}

	theSerializer->release();
	cppDEL(XMLTarget);
	doc->release();

	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	mafLogMessage(wxString::Format("Landmarks Dictionary has been written %s", fileName.GetCStr()));

	return MAF_OK;
}

//----------------------------------------------------------------------------
int mafOpAddLandmark::LoadLandmarksDefinitions(wxString fileName)
{
	m_LandmarkNameVect.clear();
	StringVector firstVect;
	m_LandmarkNameVect.push_back(m_LocalLandmarkNameVect);
	m_LandmarkNameVect[0].clear();

	StringVector groupNameVect;
	std::set<wxString> allItems;

	//Open the file xml
	try
	{
		XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize();
	}
	catch (const XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		// Do your failure processing here
		return MAF_ERROR;
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
			mafErrorMessage("Errors while parsing XML file");
			return MAF_ERROR;
		}

		// extract the root element and wrap inside a mafXMLElement
		XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = XMLParser->getDocument();
		XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root = doc->getDocumentElement();
		assert(root);

		if (CheckNodeElement(root, "Landmarks_Dictionary"))
		{
			mafString version = GetElementAttribute(root, "Version");

			//Check Config file version
			if (version != "1.0")
			{
				mafLogMessage("Wrong file Version:\n version:%s", version.GetCStr());
				return MAF_ERROR;
			}
		}
		else
		{
			mafLogMessage("Wrong check root node");
			return MAF_ERROR;
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
						allItems.insert(typeNode->getTextContent());
					}
				}

				m_LandmarkNameVect.push_back(newVect);
			}	
		}
	}
	catch (const  XERCES_CPP_NAMESPACE_QUALIFIER XMLException& toCatch)
	{
		return MAF_ERROR;
	}
	catch (const  XERCES_CPP_NAMESPACE_QUALIFIER DOMException& toCatch)
	{
		return MAF_ERROR;
	}
	catch (...)
	{
		return MAF_ERROR;
	}

	cppDEL(errReporter);
	delete XMLParser;

	// terminate the XML library
	XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate();

	mafLogMessage(_("Configuration file Loaded"));

	// Create All Items Vect
	for (std::set<wxString>::const_iterator it = allItems.begin(); it != allItems.end(); ++it)
		m_LandmarkNameVect[0].push_back(*it);
	
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
	allItems.clear();

	return MAF_OK;
}
//---------------------------------------------------------------------------
bool mafOpAddLandmark::CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName)
{
	//Reading nodes
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return false;

	XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *nodeElement = (XERCES_CPP_NAMESPACE_QUALIFIER DOMElement*)node;
	mafString nameElement = "";
	nameElement = mafXMLString(nodeElement->getTagName());

	return (nameElement == elementName);
}
//--------------------------------------------------------------------------
mafString mafOpAddLandmark::GetElementAttribute(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *attributeName)
{
	if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
		return "";

	return mafXMLString(((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *)node)->getAttribute(mafXMLString(attributeName)));
}

//--------------------------------------------------------------------------
int mafOpAddLandmark::LoadLandmarksFromVME()
{
	m_LocalLandmarkNameVect.clear();
	int nLandmarks = m_Cloud->GetNumberOfLandmarks();

	for (int i = 0; i < nLandmarks; i++)
	{
		mafVMELandmark *lm = m_Cloud->GetLandmark(i);

		m_LocalLandmarkNameVect.push_back(lm->GetName());

		// Create Undo Vector
		mafVMELandmark* newLandmark = NULL;
		mafNEW(newLandmark);
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

	return MAF_OK;
}
//---------------------------------------------------------------------------
void mafOpAddLandmark::ShowLandmarkGroup()
{
	if (m_CurrentLandmark)
	{
		mafEventMacro(mafEvent(this, VME_SHOW, m_CurrentLandmark, true));
		mafEventMacro(mafEvent(this, VME_SHOW, m_SelectedLandmarkCloud->GetLandmark(0), false));
		m_CurrentLandmark = NULL;
	}

	wxString firstSelection = "";

	if (!m_TestMode)
	{
		if (m_ShowMode == 0)
		{
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

			std::vector<wxString> output(allItems.size());
			std::copy(allItems.begin(), allItems.end(), output.begin());

			if (output.size() > 0) firstSelection = output[0];

			m_Dict->InitDictionary(&output);
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
void mafOpAddLandmark::CheckEnableOkCondition()
{
	if (m_ShowMode == 0)
	{
		m_Gui->Enable(wxOK, true);
	}
	else
	{
		bool res = true;

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