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

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpAddLandmark);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum ADD_LANDMARK_ID
{
	ID_LOAD = MINID,
	ID_ADD_TO_CURRENT_TIME,
	ID_CHANGE_POSITION,
	ID_CHANGE_NAME,
	ID_NEW_LANDMARK,
	ID_REMOVE_LANDMARK,
	ID_SHOW_GROUP,
};

//----------------------------------------------------------------------------
mafOpAddLandmark::mafOpAddLandmark(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_PickedVme = NULL;
	m_Cloud = NULL;

	m_LandmarkPicker = NULL;
	m_OldBehavior = NULL;

	m_LandmarkAdded.clear();
	m_LandmarkNameVect.clear();
	m_LocalLandmarkNameVect;

	m_CurrentLandmark = NULL;
	m_SelectedLadmarkCloud = NULL;

	m_LandmarkName = "";
	m_LandmarkSelected = "";
	m_PrevLandmarkSelected = "";

	m_ShowMode = 0;

	m_CloudCreatedFlag = false;
	m_PickingActiveFlag = false;
	m_LandmarkPosition[0] = m_LandmarkPosition[1] = m_LandmarkPosition[2] = 0;
}
//----------------------------------------------------------------------------
mafOpAddLandmark::~mafOpAddLandmark()
//----------------------------------------------------------------------------
{
	for (int i = 0; i < m_LandmarkAdded.size(); i++)
	{
		mafDEL(m_LandmarkAdded[i]);
	}

	m_LandmarkNameVect.clear();
	m_LocalLandmarkNameVect.clear();
	m_LandmarkAdded.clear();

	mafDEL(m_LandmarkPicker);

	mafDEL(m_SelectedLadmarkCloud);

	if (m_CloudCreatedFlag)
		mafDEL(m_Cloud);
}
//----------------------------------------------------------------------------
mafOp* mafOpAddLandmark::Copy()   
//----------------------------------------------------------------------------
{
	mafOpAddLandmark *op = new mafOpAddLandmark(m_Label);
  op->m_OpType = m_OpType;
	op->m_Canundo = m_Canundo;
	op->m_PickingActiveFlag = m_PickingActiveFlag;
  return op;
}
//----------------------------------------------------------------------------
bool mafOpAddLandmark::Accept(mafVME*node)
//----------------------------------------------------------------------------
{
	if (!node) return false;
	
  if( node->IsMAFType(mafVMESurface) || node->IsMAFType(mafVMEVolumeGray) || node->IsMAFType(mafVMESurfaceParametric)) 
	{
		m_PickingActiveFlag = true;
    return true;
	}

  if( node->IsMAFType(mafVMELandmarkCloud) && 
		  node->GetParent() && 
		  (node->GetParent()->IsMAFType(mafVMESurface) || node->GetParent()->IsMAFType(mafVMEVolumeGray) || node->GetParent()->IsMAFType(mafVMESurfaceParametric))) 
	{
		  m_PickingActiveFlag = true;
			return true;
	}

  if( node->IsMAFType(mafVMELandmark) && 
		  node->GetParent() && 
			node->GetParent()->IsMAFType(mafVMELandmarkCloud) &&
			node->GetParent()->GetParent() && 
			(node->GetParent()->GetParent()->IsMAFType(mafVMESurface) || node->GetParent()->GetParent()->IsMAFType(mafVMEVolumeGray) || node->GetParent()->GetParent()->IsMAFType(mafVMESurfaceParametric))) 
	{
		m_PickingActiveFlag = true;
		return true;
	}

	m_PickingActiveFlag = false;
  return true; //accept all other vmes
}

//----------------------------------------------------------------------------
void mafOpAddLandmark::OpRun()
//----------------------------------------------------------------------------
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
			m_Cloud = (mafVMELandmarkCloud *)m_Input->GetParent();
			if (m_Cloud)
				m_PickedVme = m_Cloud->GetParent();
		}
		else //if (m_Input->IsMAFType(mafVMESurface) || m_Input->IsMAFType(mafVMEVolumeGray) || m_Input->IsMAFType(mafVMESurfaceParametric))
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
			mafEventMacro(mafEvent(this, VME_SHOW, m_Cloud, true));
			m_CloudCreatedFlag = true;
		}

		// Customize m_PickedVme behavior
		m_LandmarkPicker = mafInteractorPERPicker::New();

		mafEventMacro(mafEvent(this, PER_PUSH, (mafObject *)(m_LandmarkPicker)));
		m_LandmarkPicker->SetListener(this);

		// Create Selected LandmarkCloud
		mafNEW(m_SelectedLadmarkCloud);

		if (m_TestMode)
		{
			m_SelectedLadmarkCloud->TestModeOn();
		}

		m_SelectedLadmarkCloud->SetName(_("Selected Landmark cloud"));

		vtkDataSet * vtkData = m_PickedVme->GetOutput()->GetVTKData();
		if (vtkData)
			m_SelectedLadmarkCloud->SetRadius(vtkData->GetLength() / 60.0);
		else
			m_SelectedLadmarkCloud->SetRadius(7.0);

		m_SelectedLadmarkCloud->ReparentTo(m_PickedVme);
	}

	// Set Selection color 
	mmaMaterial *select_material = m_SelectedLadmarkCloud->GetMaterial();
	SetMaterialRGBA(select_material, 0.1, 0.1, 1.0, 0.8);

	mmaMaterial *material = m_Cloud->GetMaterial();

	// Save old color
	m_OldColorCloud[0] = material->m_Diffuse[0];
	m_OldColorCloud[1] = material->m_Diffuse[1];
	m_OldColorCloud[2] = material->m_Diffuse[2];
	m_OldColorCloud[3] = material->m_Opacity;
	
	// Set Landmarks color 
	SetMaterialRGBA(material, 1.0, 0.1, 0.1, 0.8);

	///
	LoadLandmarksFromVME();

	if (!GetTestMode())
		CreateGui();

	m_LandmarkNameVect.push_back(m_LocalLandmarkNameVect);
	ShowLandmarkGroup();

	if (m_LandmarkNameVect[m_ShowMode].size() > 0)
		SelectLandmark(m_LandmarkNameVect[m_ShowMode][0]);
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpDo()
//----------------------------------------------------------------------------
{
  AddLandmarksToTree();
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpUndo()
//----------------------------------------------------------------------------
{
  RemoveLandmarksFromTree();

	mafEventMacro(mafEvent(this, VME_REMOVE, m_SelectedLadmarkCloud));
}

//----------------------------------------------------------------------------
void mafOpAddLandmark::CreateGui()
//----------------------------------------------------------------------------
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
	m_Gui->TwoButtons(ID_NEW_LANDMARK, ID_REMOVE_LANDMARK, "New", "Remove");
	m_Gui->Divider();

	m_Gui->Divider(2);

	m_Gui->Label(_("Landmark Props"));
	m_Gui->String(ID_CHANGE_NAME, _("Name"), &m_LandmarkName);

	m_Gui->Vector(ID_CHANGE_POSITION, _("Position"), m_LandmarkPosition, MINFLOAT, MAXFLOAT, 2, _("landmark position"));

	mafString tooltip(_("If checked, add the landmark to the current time. \nOtherwise add the landmark at time = 0"));
	m_Gui->Bool(ID_ADD_TO_CURRENT_TIME, _("Current time"), &m_AddToCurrentTime, 1, tooltip);
	m_Gui->Divider(2);

	m_Gui->Label(_("Dictionary"));
	m_Gui->Button(ID_LOAD, _("Load dictionary"));

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");

	m_Gui->OkCancel();
	m_Gui->Label("");
	m_Gui->Label("");

	ShowGui();
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  vtkPoints *pts = NULL; 
  if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_LOAD:
			{
				wxString wild_dict = "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
				wxString m_dict = mafGetApplicationDirectory().c_str();
				m_dict = m_dict + "\\Config\\Dictionary\\";
				wxString file = mafGetOpenFile(m_dict, wild_dict, "Choose Dictionary File").c_str();
				if (file != "") LoadLandmarksDefinitions(file);

				m_ShowMode = 0;
				ShowLandmarkGroup();
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
				m_Dict->SelectItem(m_LandmarkSelected);
			}
			break;

			case VME_PICKED:
			{
				if (this->m_Cloud && !m_CurrentLandmark)
				{
					pts = vtkPoints::SafeDownCast(e->GetVtkObj());
					if (pts  && m_LandmarkSelected != "")
					{
						pts->GetPoint(0, m_LandmarkPosition);
						AddLandmark(m_LandmarkPosition);
						m_Dict->SetCloud(m_Cloud);

						//SelectNextLandmark();
					}
				}
				else
				{
					pts = vtkPoints::SafeDownCast(e->GetVtkObj());
					if (pts)
					{
						pts->GetPoint(0, m_LandmarkPosition);
						m_CurrentLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
					}
				}

				m_Gui->Update();
				mafEventMacro(mafEvent(this, CAMERA_UPDATE));
			}
			break;

			case ID_CHANGE_POSITION:
			{
				if (this->m_Cloud && m_CurrentLandmark)
				{
					m_CurrentLandmark->SetAbsPose(m_LandmarkPosition[0], m_LandmarkPosition[1], m_LandmarkPosition[2], 0, 0, 0);
					m_Gui->Update();
					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				}
			}
			break;

			case ID_NEW_LANDMARK:
			{
				char printStr[100];
				sprintf(printStr, "New_Landmark_%d", m_Dict->GetSize()+1);
				
				m_LandmarkName = printStr;		
				m_PrevLandmarkSelected = m_LandmarkSelected;
				m_LandmarkSelected = m_LandmarkName;

				m_Dict->AddItem((wxString)m_LandmarkName);

				SelectLandmark(m_LandmarkSelected);

				m_Gui->Update();
			}
			break;

			case ID_REMOVE_LANDMARK:
				{
					RemoveLandmark(m_LandmarkSelected);
					m_Gui->Update();
				}
				break;

      case wxOK:
        OpStop(OP_RUN_OK);
      break;

      case wxCANCEL:
        RemoveLandmarksFromTree();
        OpStop(OP_RUN_CANCEL);
      break;

      case ID_CHANGE_NAME:
			{
				if (this->m_Cloud && m_CurrentLandmark)
				{
					m_CurrentLandmark->SetName(m_LandmarkName);
					m_Dict->UpdateItem(m_LandmarkSelected, m_LandmarkName);

					m_Gui->Update();
					mafEventMacro(mafEvent(this, CAMERA_UPDATE));
				}
			}
      break;

      default:
        mafEventMacro(*e);
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::ExistingLandmarkMessage()
//----------------------------------------------------------------------------
{
  wxString existing_lm_msg(_("Landmark with that name already exist, Please change it!"));
  wxMessageBox(existing_lm_msg, _("Warning"), wxOK|wxICON_WARNING , NULL);
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::OpStop(int result)
//----------------------------------------------------------------------------
{
	// Reset Landmarks color 
	mmaMaterial *material = m_Cloud->GetMaterial();
	SetMaterialRGBA(material, m_OldColorCloud[0], m_OldColorCloud[1], m_OldColorCloud[2], m_OldColorCloud[3]);

	if (m_CurrentLandmark && result == OP_RUN_OK)
	{
		m_CurrentLandmark->ReparentTo(m_Cloud);
		mafEventMacro(mafEvent(this, VME_SHOW, m_CurrentLandmark, true));
	}

	mafEventMacro(mafEvent(this, PER_POP));
	mafDEL(m_LandmarkPicker);

	cppDEL(m_Dict);

	if (!m_TestMode)
	{
		HideGui();
	}

	m_SelectedLadmarkCloud->ReparentTo(NULL);
	mafEventMacro(mafEvent(this, result));
}

// LANDMARK MANAGER
//----------------------------------------------------------------------------
void mafOpAddLandmark::AddLandmark(double pos[3])
//----------------------------------------------------------------------------
{
  m_LandmarkPosition[0] = pos[0];
  m_LandmarkPosition[1] = pos[1];
  m_LandmarkPosition[2] = pos[2];
  
  mafSmartPointer<mafVMELandmark> landmark;
  landmark->SetName(m_LandmarkName.GetCStr());
  landmark->ReparentTo(m_Cloud);
  if(NULL != m_PickedVme)
     landmark->SetTimeStamp(m_PickedVme->GetTimeStamp());
  landmark->Update();
  if(m_AddToCurrentTime)
    landmark->SetAbsPose(m_LandmarkPosition[0],m_LandmarkPosition[1],m_LandmarkPosition[2],0,0,0);
  else
    landmark->SetAbsPose(m_LandmarkPosition[0],m_LandmarkPosition[1],m_LandmarkPosition[2],0,0,0,0);

  mafEventMacro(mafEvent(this,VME_SHOW,landmark.GetPointer(),true));
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

  m_LandmarkAdded.push_back(landmark);
  m_LandmarkAdded[m_LandmarkAdded.size()-1]->Register(NULL);

  if (m_Gui && !GetTestMode())
  {
  	m_Gui->Enable(wxOK, true);
  }
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::RemoveLandmark(mafString selection)
//----------------------------------------------------------------------------
{
	int removedItemId = m_Dict->RemoveItem(selection);

	if(removedItemId>=0)
	{
		if (this->m_Cloud && this->m_Cloud->FindInTreeByName(selection))
		{
			mafEventMacro(mafEvent(this, VME_REMOVE, m_LandmarkAdded[removedItemId]));
			mafDEL(m_LandmarkAdded[removedItemId]);
			m_LandmarkAdded.erase(m_LandmarkAdded.begin() + removedItemId);
			mafEventMacro(mafEvent(this, CAMERA_UPDATE));
		}
		
		if (m_Dict->SelectItem(m_PrevLandmarkSelected) >= 0);
		{
			m_LandmarkSelected = m_PrevLandmarkSelected;
			m_LandmarkName = m_LandmarkSelected;

			SelectLandmark(m_LandmarkSelected);
			m_Gui->Update();
		}
	}
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::SelectLandmark(mafString selection)
//----------------------------------------------------------------------------
{
	if (m_CurrentLandmark)
	{
		m_CurrentLandmark->ReparentTo(m_Cloud);
		m_SelectedLadmarkCloud->CleanTree();

		mafEventMacro(mafEvent(this, VME_SHOW, m_CurrentLandmark, true));
		m_CurrentLandmark = NULL;
	}

	m_PrevLandmarkSelected = m_LandmarkSelected;
	m_LandmarkSelected = m_LandmarkName = selection;

	m_Dict->SetTitle("Edit landmark");

	mafVMELandmark *landmark = mafVMELandmark::SafeDownCast(this->m_Cloud->FindInTreeByName(m_LandmarkSelected.GetCStr()));

	if (this->m_Cloud && landmark)
	{
		m_CurrentLandmark = landmark;
		m_CurrentLandmark->ReparentTo(m_SelectedLadmarkCloud);
		m_CurrentLandmark->GetPoint(m_LandmarkPosition);

		mafEventMacro(mafEvent(this, VME_SHOW, landmark, true));
		landmark->GetPoint(m_LandmarkPosition);

		mafEventMacro(mafEvent(this, CAMERA_UPDATE));
	}
	else
		m_Dict->SetTitle("Add landmark");

	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::SelectNextLandmark()
//----------------------------------------------------------------------------
{
	std::vector<wxString>::iterator i = std::find(m_LandmarkNameVect[m_ShowMode].begin(), m_LandmarkNameVect[m_ShowMode].end(), m_LandmarkSelected);

	int selectionIndex = -1;

	if (i != m_LandmarkNameVect[m_ShowMode].end())
	{
		selectionIndex = std::distance(m_LandmarkNameVect[m_ShowMode].begin(), i);
	}

	if (selectionIndex >= 0 && selectionIndex < m_LandmarkNameVect[m_ShowMode].size() - 1)
	{
		SelectLandmark(m_LandmarkNameVect[m_ShowMode][selectionIndex + 1]);
		m_Dict->SelectItem(m_LandmarkNameVect[m_ShowMode][selectionIndex + 1]);
	}
}

//----------------------------------------------------------------------------
void mafOpAddLandmark::AddLandmarksToTree()
//----------------------------------------------------------------------------
{
  int reparent_result = MAF_OK;
  if(m_CloudCreatedFlag)
  {
		if (m_PickingActiveFlag == true)
			reparent_result = m_Cloud->ReparentTo(m_PickedVme);
		else
		  reparent_result = m_Cloud->ReparentTo(m_Input);

    if (reparent_result == MAF_OK)
    {
      mafEventMacro(mafEvent(this,VME_SHOW,m_Cloud,true));
    }
  }
  else
  {
    for (int l=0; l < m_LandmarkAdded.size(); l++)
    {
      reparent_result = m_LandmarkAdded[l]->ReparentTo(m_Cloud);

      if (reparent_result == MAF_OK)
      {
        mafEventMacro(mafEvent(this,VME_SHOW,m_LandmarkAdded[l],true));
      }
    }
  }

  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpAddLandmark::RemoveLandmarksFromTree()
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

//----------------------------------------------------------------------------
void mafOpAddLandmark::SetMaterialRGBA(mmaMaterial *material, double r, double g, double b, double a)
//----------------------------------------------------------------------------
{
	material->m_Diffuse[0] = r;
	material->m_Diffuse[1] = g;
	material->m_Diffuse[2] = b;
	material->m_Opacity = a;
	material->UpdateProp();
}

// LOAD LANDMARK DEFINITIONS
//----------------------------------------------------------------------------
int mafOpAddLandmark::LoadLandmarksDefinitions(wxString fileName)
//----------------------------------------------------------------------------
{
	m_LandmarkNameVect.clear();
	StringVector firstVect;
	m_LandmarkNameVect.push_back(m_LocalLandmarkNameVect);

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
	
	// Update GUI ComboBox
	m_ShowComboBox->Clear();
	m_ShowComboBox->AppendString("Show All");
	m_Dict->SetTitle("Show All");

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
//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
	{
		if (node->getNodeType() != XERCES_CPP_NAMESPACE_QUALIFIER DOMNode::ELEMENT_NODE)
			return "";

		return mafXMLString(((XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *)node)->getAttribute(mafXMLString(attributeName)));
	}

//--------------------------------------------------------------------------
int mafOpAddLandmark::LoadLandmarksFromVME()
//--------------------------------------------------------------------------
{
	m_LocalLandmarkNameVect.clear();
	int count = m_Cloud->GetNumberOfChildren();

	for (int i = 0; i < count; i++)
	{
		mafVME *node = m_Cloud->GetChild(i);

		if (node->IsMAFType(mafVMELandmark) && !this->m_Cloud->FindInTreeByName(node->GetName()))
		{
			m_LocalLandmarkNameVect.push_back(node->GetName());
		}
	}

	return MAF_OK;
}
//---------------------------------------------------------------------------
void mafOpAddLandmark::ShowLandmarkGroup()
//---------------------------------------------------------------------------
{
	if (m_CurrentLandmark)
	{
		m_CurrentLandmark->ReparentTo(m_Cloud);
		m_SelectedLadmarkCloud->CleanTree();

		mafEventMacro(mafEvent(this, VME_SHOW, m_CurrentLandmark, true));
		m_CurrentLandmark = NULL;
	}

	if (!m_TestMode)
	{
		m_Dict->InitDictionary(&m_LandmarkNameVect[m_ShowMode]);
		m_Dict->SetTitle("Select landmark");
		
		m_Gui->Enable(ID_SHOW_GROUP, true);
		m_Gui->Update();

		CheckEnableOkCondition();
	}
}

//---------------------------------------------------------------------------
void mafOpAddLandmark::CheckEnableOkCondition()
//---------------------------------------------------------------------------
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