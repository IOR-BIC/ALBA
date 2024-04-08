/*=========================================================================
Program:   AssemblerPro
Module:    albaVMEProsthesis.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi
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

#include "albaDecl.h"
#include "albaGUIDialogProsthesisSelection.h"

#include "albaGUI.h"
#include "albaProsthesesDBManager.h"
#include "albaTransform.h"
#include "albaVMEOutput.h"
#include "albaVMEProsthesis.h"
#include "albaVMESurface.h"

#include "mmaMaterial.h"
#include "vtkAppendPolyData.h"
#include "vtkLookupTable.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "wx/listctrl.h"
#include "albaGUITransformMouseFloatVME.h"
#include "albaInteractorGenericMouseFloatVME.h"
#include "vtkSphereSource.h"
#include "albaVME.h"
#include "albaTagArray.h"
#include "albaStorageElement.h"
#include "albaMatrixVector.h"

//-------------------------------------------------------------------------
albaCxxTypeMacro(albaVMEProsthesis)

//-------------------------------------------------------------------------
albaVMEProsthesis::albaVMEProsthesis()
{
	m_AppendPolydata = NULL;
	m_ProsthesisName = "";
	m_Prosthesis = NULL;

	m_ContentGui = NULL;
	m_GroupGui = NULL;
	m_InteractorTransformFloatVME = NULL;
  m_InteractorGenericMouseFloatVME = NULL;

	m_RotCenterVME = NULL;
	m_LockOnOpRun = true;
}
//-------------------------------------------------------------------------
albaVMEProsthesis::~albaVMEProsthesis()
{
	vtkDEL(m_AppendPolydata);
}

//-------------------------------------------------------------------------
char** albaVMEProsthesis::GetIcon()
{
#include "albaVMEProsthesis.xpm"
	return albaVMEProsthesis_xpm;
}

//-------------------------------------------------------------------------
int albaVMEProsthesis::InternalInitialize()
{

	if (m_InteractorTransformFloatVME == NULL)
	{
		m_InteractorTransformFloatVME = new albaGUITransformMouseFloatVME(this, this);
		m_InteractorGenericMouseFloatVME = m_InteractorTransformFloatVME->CreateBehavior(MOUSE_LEFT_SHIFT);
		m_InteractorGenericMouseFloatVME->SetListener(this);
		m_InteractorGenericMouseFloatVME->SetVME(this);
	}

	if (Superclass::InternalInitialize() == ALBA_OK)
	{
		// force material allocation
		GetMaterial();

		return ALBA_OK;
	}


	return ALBA_ERROR;
}
//-------------------------------------------------------------------------
mmaMaterial *albaVMEProsthesis::GetMaterial()
{
	mmaMaterial *material = (mmaMaterial *)GetAttribute("MaterialAttributes");
	if (material == NULL)
	{
		material = mmaMaterial::New();
		SetAttribute("MaterialAttributes", material);
	}
	return material;
}
//-------------------------------------------------------------------------
bool albaVMEProsthesis::IsAnimated()
{
	return false;
}
//-------------------------------------------------------------------------
void albaVMEProsthesis::GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes)
{
	kframes.clear(); // no timestamps
}

//-------------------------------------------------------------------------
albaGUI* albaVMEProsthesis::CreateGui()
{
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);
		
		m_Gui->String(ID_PROSTHESIS_NAME, "", &m_ProsthesisName);
		m_Gui->Enable(ID_PROSTHESIS_NAME, false);

		m_Gui->Button(ID_PROSTHESIS_CHANGE, "Change Prosthesis");
		m_Gui->Divider(1);

		m_ContentGui = new albaGUI(this);
		m_GroupGui = new albaGUI(this);

		m_ContentGui->Add(m_GroupGui);
		m_Gui->Add(m_ContentGui);

		m_Gui->Divider();
		m_Gui->FitGui();
	}
	return m_Gui;
}
//-------------------------------------------------------------------------
void albaVMEProsthesis::UpdateGui()
{
	if (m_Gui)
	{
		m_GroupGui->FitGui();
		m_ContentGui->FitGui();
		m_Gui->FitGui();

		m_Gui->FitInside();
		m_Gui->Update();
	}
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::FitParentGui()
{
	albaGUI* parent = (albaGUI*)m_Gui->GetParent();
	if (parent)
	{
		parent->FitGui();
		parent->Update();
	}

	albaGUI* grandParent = (albaGUI*)m_Gui->GetParent();
	if (grandParent)
	{
		grandParent->FitGui();
		grandParent->Update();
	}
}

//----------------------------------------------------------------------------
void albaVMEProsthesis::AddComponentGroup(albaProDBCompGroup *componentGroup)
{
	//VTK Stuffs
	vtkTransformPolyDataFilter *compTraFilter;
	vtkNEW(compTraFilter);

	m_TransformFilters.push_back(compTraFilter);

	vtkTransform *compTra;
	vtkNEW(compTra);
	compTra->PreMultiply();
	m_Transforms.push_back(compTra);

	int currGroup = m_Transforms.size()-1;
	if (currGroup > 1)
	{
		compTra->SetMatrix(m_Transforms[currGroup - 1]->GetMatrix());
	}

	std::vector<albaProDBComponent *> *components = componentGroup->GetComponents();

	if (currGroup >= 0)
	{		
		if (currGroup >= 1)
		{
			albaProDBCompGroup *prevGroup = m_Prosthesis->GetCompGroups()->at(currGroup - 1);
			int prevSelComp = m_ComponentListBox[currGroup - 1]->GetSelection();
			albaProDBComponent *prevComp = prevGroup->GetComponents()->at(prevSelComp);
			
			compTra->Concatenate(prevComp->GetMatrix().GetVTKMatrix());
		}
		albaProDBComponent *currentComp = components->at(0);
		vtkPolyData * compVTKData = currentComp->GetVTKData();
		if (compVTKData)
		{
			compTraFilter->SetInputData(compVTKData);
			compVTKData->UnRegister(NULL);
		}
	}
	compTra->Update();

	compTraFilter->SetTransform(compTra);
	m_AppendPolydata->AddInputConnection(compTraFilter->GetOutputPort());

	CreateComponentGui(currGroup, componentGroup);
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::CreateComponentGui(int currGroup, albaProDBCompGroup * componentGroup)
{
	//GUI Stuffs
	int baseID = ID_LAST + m_ComponentGui.size()*ID_LAST_COMP_ID;

	std::vector<albaProDBComponent *> *components = componentGroup->GetComponents();
	
	albaGUI *compGui = new albaGUI(this);

	//show all by default
	m_ShowComponents[currGroup] = true;
	compGui->Bool(baseID + ID_SHOW_COMPONENT, componentGroup->GetName(), &m_ShowComponents[currGroup], 1, "Show/Hide");
	wxListBox *listBox = compGui->ListBox(baseID + ID_SELECT_COMPONENT, "");

	for (int comp = 0; comp < components->size(); comp++)
	{
		listBox->Append(components->at(comp)->GetName().GetCStr());
	}
	compGui->Divider(1);

	listBox->SetSelection(0);

	m_ComponentListBox.push_back(listBox);
	m_ComponentGui.push_back(compGui);

	// Add to Gui
	m_GroupGui->Add(compGui);
	compGui->FitGui();
	compGui->Update();

	UpdateGui();
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::ClearComponentGroups()
{
	for (int i = 0; i < m_TransformFilters.size(); i++)
	{
		m_AppendPolydata->RemoveInputConnection(0, m_TransformFilters[i]->GetOutputPort());
		vtkDEL(m_TransformFilters[i]);
		vtkDEL(m_Transforms[i]);
		//m_ContentGui->Remove(m_ComponentGui[i]);

		delete m_ComponentGui[i];
	}

	m_TransformFilters.clear();
	m_Transforms.clear();
	m_ComponentListBox.clear();
	m_ComponentGui.clear();


	// Rebuild Gui
	m_GroupGui = NULL;
	m_GroupGui = new albaGUI(this);

	m_ContentGui->DestroyChildren();
	m_ContentGui->Add(m_GroupGui);

	UpdateGui();
}

//-------------------------------------------------------------------------
void albaVMEProsthesis::SelectComponent(int compGroup, int compId)
{
	std::vector<albaProDBCompGroup *> * compGroups = m_Prosthesis->GetCompGroups();
	albaProDBCompGroup *group = compGroups->at(compGroup);
	int groups = compGroups->size();
	std::vector<albaProDBComponent *> *components = group->GetComponents();
	
	m_ComponentListBox[compGroup]->Select(compId);

	if (groups > 0)
	{
		//update current VTK data 
		albaProDBComponent * currentComp = components->at(compId);
		m_TransformFilters[compGroup]->SetInputData(currentComp->GetVTKData());


		//update current following transforms
		for (int i = compGroup + 1; i < groups; i++)
		{
			vtkTransform *compTra = m_Transforms[i];
			compTra->SetMatrix(m_Transforms[i - 1]->GetMatrix());

			albaProDBComponent * prevComp = currentComp;


			albaProDBCompGroup *currentGroup = compGroups->at(i);
			int prevSelComp = m_ComponentListBox[i]->GetSelection();
			currentComp = currentGroup->GetComponents()->at(prevSelComp);

			compTra->Concatenate(prevComp->GetMatrix().GetVTKMatrix());

			compTra->Update();
		}
	}
	m_AppendPolydata->Update();
	GetOutput()->Update();

	GetLogicManager()->CameraUpdate();
}
//-------------------------------------------------------------------------
void albaVMEProsthesis::ShowComponent(int compGroup, bool show)
{
	if (show)
		m_AppendPolydata->AddInputConnection(m_TransformFilters[compGroup]->GetOutputPort());
	else
		m_AppendPolydata->RemoveInputConnection(0, m_TransformFilters[compGroup]->GetOutputPort());
	m_AppendPolydata->Update();
	GetOutput()->Update();
	
	int nShow = 0, nComponents = m_ComponentGui.size();
	for (int i= 0; i < nComponents; i++)
		if (m_ShowComponents[i])
			nShow++;
	
	GetLogicManager()->CameraUpdate(); 
}

//----------------------------------------------------------------------------
void albaVMEProsthesis::SetProsthesis(albaProDBProsthesis *prosthesis)
{
	if (prosthesis)
	{
		CreateGui();

		if (m_AppendPolydata == NULL)
			vtkNEW(m_AppendPolydata);

		// Remove current components
		ClearComponentGroups();

		m_Prosthesis = prosthesis;

		m_ProsthesisName = m_Prosthesis->GetProducer();
		m_ProsthesisName += " | " + m_Prosthesis->GetName();

		std::vector<albaProDBCompGroup *> *componentsVector = prosthesis->GetCompGroups();

		for (int i = 0; i < componentsVector->size(); i++)
		{
			AddComponentGroup(componentsVector->at(i));
		}

		SetData(m_AppendPolydata->GetOutput(), 0, ALBA_VME_REFERENCE_DATA);
			
		UpdateGui();
	}
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::ChangeProsthesis()
{
	albaGUIDialogProsthesisSelection pd(_("Select Prosthesis"));
	if(m_Prosthesis!=NULL)
		pd.SetProducer(m_Prosthesis->GetProducer());
	pd.Show();

	if (pd.OkClosed())
	{
		albaProDBProsthesis *prosthesis = pd.GetProsthesis();

		SetProsthesis(prosthesis);
		UpdateGui();
		FitParentGui();

		GetLogicManager()->VmeVisualModeChanged(this);
	}
}

//-------------------------------------------------------------------------
void albaVMEProsthesis::OnEvent(albaEventBase *alba_event)
{
	// events to be sent up or down in the tree are simply forwarded
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{

		albaID eventId = e->GetId();
		int compNum = m_ComponentGui.size();
		switch (eventId)
		{
		case ID_START: break;
		case ID_PROSTHESIS_CHANGE: ChangeProsthesis(); break;
		case ID_TRANSFORM: OnTranfromEvent(e); break;

		default:
			if (eventId >= ID_LAST && eventId < ID_LAST + ID_LAST_COMP_ID*compNum)
			{
				int baseId = eventId - ID_LAST;
				int comp = baseId / ID_LAST_COMP_ID;
				int id = baseId % ID_LAST_COMP_ID;
				OnComponentEvent(comp, id);
			}
			break;
		}
	}
	else
	{
		Superclass::OnEvent(alba_event);
	}
}
//----------------------------------------------------------------------------
void albaVMEProsthesis::OnComponentEvent(int compGroup, int id)
{
	switch (id)
	{
	case ID_SHOW_COMPONENT: ShowComponent(compGroup, m_ShowComponents[compGroup]); break;
	case ID_SELECT_COMPONENT: SelectComponent(compGroup, m_ComponentListBox[compGroup]->GetSelection()); break;

	default:
		break;
	}
}

//----------------------------------------------------------------------------
void albaVMEProsthesis::OnTranfromEvent(albaEvent *e)
{
	long arg = e->GetArg();
	
	if (arg == albaInteractorGenericMouse::MOUSE_MOVE && (!m_LockOnOpRun || !GetLogicManager()->IsOperationRunning()))
	{
		// Update Matrix
		// handle incoming transform events
		vtkTransform *tr = vtkTransform::New();
		tr->PostMultiply();
		tr->SetMatrix(GetOutput()->GetAbsMatrix()->GetVTKMatrix());
		vtkMatrix4x4 * eventMatrix = e->GetMatrix()->GetVTKMatrix();
		tr->Concatenate(eventMatrix);
		tr->Update();

		SetAbsMatrix(tr->GetMatrix());

		tr->Delete();

		GetLogicManager()->CameraUpdate();
	}
}

//-------------------------------------------------------------------------
void albaVMEProsthesis::CreateRotCenterVME()
{
	if (m_RotCenterVME == NULL)
	{
		vtkALBASmartPointer<vtkSphereSource> Sphere;
		Sphere->SetRadius(2.5);
		Sphere->Update();
		albaNEW(m_RotCenterVME);
		m_RotCenterVME->SetData(Sphere->GetOutput(), 0.0);
		m_RotCenterVME->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
		m_RotCenterVME->ReparentTo(this);
		m_RotCenterVME->Delete();

		m_RotCenterVME->SetAbsMatrix(*(this->GetOutput()->GetAbsMatrix()));//RefSys start with prosthesis origin

		SetRotCenterVME(m_RotCenterVME);
		ShowRotCenter(true);
	}
}

//----------------------------------------------------------------------------
void albaVMEProsthesis::SetRotCenterVME(albaVME *rotCenter)
{

	if (rotCenter == NULL)
		return;

	m_RotCenterVME=albaVMESurface::SafeDownCast(rotCenter);

	m_InteractorTransformFloatVME->SetRefSys(m_RotCenterVME);

	SetLink("ROT_REFSYS", rotCenter);
}
//----------------------------------------------------------------------------
albaVME* albaVMEProsthesis::GetRotCenterVME()
{
	if (m_RotCenterVME == NULL)
		SetRotCenterVME(GetLink("ROT_REFSYS"));

	if (m_RotCenterVME == NULL)
		CreateRotCenterVME();

	return	m_RotCenterVME;
}

//----------------------------------------------------------------------------
int albaVMEProsthesis::InternalStore(albaStorageElement *parent)
{
	int nComp = m_ComponentListBox.size();
	albaVME::InternalStore(parent);
	
	parent->StoreText("ProName", m_Prosthesis->GetName());
	parent->StoreInteger("ProSide", m_Prosthesis->GetSide());
	parent->StoreInteger("ProNcomp", nComp);
	
	int *compSel = new int[nComp];
	for (int i = 0; i < nComp; i++)
		compSel[i] = m_ComponentListBox[i]->GetSelection();

	parent->StoreVectorN("ProCompSel",compSel, nComp);
	
	delete[] compSel;

	// sub-element for storing the matrix vector
	albaStorageElement *matrix_vector = parent->AppendChild("MatrixVector");
	if (m_MatrixVector->Store(matrix_vector) == ALBA_ERROR)
		return ALBA_ERROR;

	return ALBA_OK;
}

//----------------------------------------------------------------------------
int albaVMEProsthesis::InternalRestore(albaStorageElement *node)
{
	albaVME::InternalRestore(node);

	albaString proName; 
	int nComp, side;
	node->RestoreText("ProName", proName);
	node->RestoreInteger("ProNcomp", nComp);
	node->RestoreInteger("ProSide", side);
	int *compSel = new int[nComp];
	
	node->RestoreVectorN("ProCompSel", compSel, nComp);


	albaProsthesesDBManager * prosthesesDBManager = GetLogicManager()->GetProsthesesDBManager();
	if (prosthesesDBManager == NULL)
	{
		albaErrorMessage("Error on Reload Prosthesis, No prostheses DB manager found!\n This project may be created under an other application please reload it with the appropriate software");
		return ALBA_ERROR;
	}

	albaProDBProsthesis *prosthesis 	= prosthesesDBManager->GetProsthesis(proName, (albaProDBProsthesis::PRO_SIDES) side);

	if (prosthesis == NULL)
	{
		albaErrorMessage("Error on Reload Prosthesis, prostheses '%s' not found on DB,\n please install it and reload the project!", proName.GetCStr());
		return ALBA_ERROR;
	}

	SetProsthesis(prosthesis);

	for (int i = 0; i < nComp; i++)
		SelectComponent(i, compSel[i]);

	UpdateGui();
	delete[] compSel;

	// restore Matrix Vector  
	if (m_MatrixVector)
	{
		albaStorageElement *matrix_vector = node->FindNestedElement("MatrixVector");
		if (matrix_vector && m_MatrixVector->Restore(matrix_vector))
			return ALBA_ERROR;
	}

	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaVMEProsthesis::GetZMinMax(double &zMin, double &zMax)
{
	zMin = VTK_DOUBLE_MAX;
	zMax = VTK_DOUBLE_MIN;
	double in[4], out[4];

	albaMatrix *absMtr=GetOutput()->GetAbsMatrix();

	in[3] = 1;
	vtkDataSet *outputDataSet = GetOutput()->GetVTKData();
	int nPoints = outputDataSet->GetNumberOfPoints();
	for (int i = 0; i < nPoints; i++) {
		outputDataSet->GetPoint(i, in);
		absMtr->MultiplyPoint(in, out);
		zMin = MIN(zMin, out[2]);
		zMax = MAX(zMax, out[2]);
	}
}

//----------------------------------------------------------------------------
void albaVMEProsthesis::Set2dModality(bool mod2d)
{
	albaInteractorGenericMouseFloatVME* isaRotate = albaInteractorGenericMouseFloatVME::SafeDownCast(m_InteractorTransformFloatVME->GetIsaRotate());
	isaRotate->Set2DModality(mod2d);
}

//-------------------------------------------------------------------------
void albaVMEProsthesis::SetRotCenter(double center[3])
{
	albaVMESurface *refSys;
	refSys = (albaVMESurface*)GetRotCenterVME();

	if (refSys)
	{
		double oldOrientation[3];
		double oldPosition[3];

		refSys->Update();
		refSys->GetOutput()->GetAbsPose(oldPosition, oldOrientation, m_CurrentTime);

		refSys->SetAbsPose(center[0], center[1], center[2], oldOrientation[0], oldOrientation[1], oldOrientation[2], m_CurrentTime);
		refSys->Update();
	}


	GetLogicManager()->CameraUpdate();
}

//-------------------------------------------------------------------------
void albaVMEProsthesis::ShowRotCenter(bool show)
{
	GetLogicManager()->VmeShow(GetRotCenterVME(), show);
}

//-------------------------------------------------------------------------
albaVMEOutput *albaVMEProsthesis::GetOutput()
{
	// allocate the right type of output on demand
	if (m_Output == NULL)
	{
		SetOutput(albaVMEOutputSurface::New()); // create the output
	}
	return m_Output;
}
