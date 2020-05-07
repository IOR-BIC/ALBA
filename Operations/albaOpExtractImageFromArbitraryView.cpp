/*=========================================================================
Program:   ALBA (Agile Library for Biomedical Applications)
Module:    albaOpExtractImageFromArbitraryView.cpp
Language:  C++
Date:      $Date: 2019-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2019 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpExtractImageFromArbitraryView.h"

#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaObject.h"
#include "albaOp.h"
#include "albaPipe.h"
#include "albaServiceClient.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEImage.h"
#include "albaView.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaViewArbitrarySlice.h"
#include "albaViewCompound.h"
#include "albaViewSlice.h"
#include "albaViewVTK.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkCamera.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkImageData.h"
#include "vtkImageExport.h"
#include "vtkPointData.h"
#include "vtkPropCollection.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkUnsignedCharArray.h"
#include "vtkWindowToImageFilter.h"

#include "wx\bitmap.h"
#include "wx\image.h"
#include "albaViewManager.h"
#include "albaAbsLogicManager.h"
#include "albaGUIValidator.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpExtractImageFromArbitraryView);

//----------------------------------------------------------------------------
albaOpExtractImageFromArbitraryView::albaOpExtractImageFromArbitraryView(wxString label) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_View = NULL;
	m_ImageSlicesGroup = NULL;

	m_Axis = 2; //Default Z
	m_Magnification = 0;

	m_ShowInTree = true;
	m_ChooseName = true;
	m_ShowExtractButton = true;
	m_ShowSliceList = true;

	m_SlicesListBox = NULL;

	m_CurrentImage = NULL;
	m_ImageName = "";

	m_GuiMessage = "";
}

//----------------------------------------------------------------------------
albaOpExtractImageFromArbitraryView::~albaOpExtractImageFromArbitraryView()
{
}

//----------------------------------------------------------------------------
bool albaOpExtractImageFromArbitraryView::Accept(albaVME *node)
{
	albaView *view = GetLogicManager()->GetViewManager()->GetSelectedView();
	bool hasView = view && (view->IsA("albaViewArbitrarySlice") || view->IsA("albaViewArbitraryOrthoSlice"));
	
	return (hasView && node->IsA("albaVMEVolumeGray"));
}

//----------------------------------------------------------------------------
albaOp* albaOpExtractImageFromArbitraryView::Copy()
{
	albaOpExtractImageFromArbitraryView *cp = new albaOpExtractImageFromArbitraryView(m_Label);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::OpRun()
{
	bool hasView = false;

	if (!m_TestMode)
	{
		albaEvent e(this, VIEW_SELECTED);
		albaEventMacro(e);

		if (e.GetBool())
		{
			m_View = e.GetView();
			hasView = m_View && (m_View->IsA("albaViewArbitrarySlice") || m_View->IsA("albaViewArbitraryOrthoSlice"));
		}

		m_ImageSlicesGroup = (albaVMEGroup *)m_Input->GetRoot()->FindInTreeByName("Slices");

		if (hasView)
		{
			GetLogicManager()->VmeShow(m_Input, true);
			CreateGui();
			UpdateListbox();
		}
		else
		{
			wxMessageBox("No View Selected");
			OpStop(OP_RUN_CANCEL);
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::OpStop(int result)
{
	if (result == OP_RUN_OK && 	!m_ShowExtractButton)
	{
		ExtractImage();
	}
	
	if (!m_TestMode)
	{
		//m_SlicesListBox->Clear();
		HideGui();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::OpDo()
{
}

//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_EXTRACT:
		{
			ExtractImage();
			UpdateListbox();

			if (m_ShowExtractButton && !m_ShowSliceList)
			{
				m_GuiMessage = "Extracted Slice image!";
				m_Gui->Update();
			}
		}
			break;

		case ID_SLICES_LIST:
		{
			SelectImageSlice();
			ShowImageSlice();
		}
			break;
 		case ID_REMOVE:
 		{
			RemoveImageSlice();
			UpdateListbox();
 		}
 		break;

		case ID_RENAME:
		{
			RenameImageSlice();
			UpdateListbox();			
		}
		break;
		
		case ID_RES:
		{
			ShowImageSlice();
		}
		break;
		case wxOK:
			OpStop(OP_RUN_OK);
			break;

		case wxCANCEL:
			OpStop(OP_RUN_CANCEL);
			break;

		default:
			Superclass::OnEvent(alba_event);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::CreateGui()
{
	// Interface:
	m_Gui = new albaGUI(this);

	if(m_View && m_View->IsA("albaViewArbitraryOrthoSlice"))
	{
		m_Gui->Label("");
		wxString axisChoice[3] = { "X","Y","Z" };
		m_Gui->Combo(ID_AXIS, "Axis", &m_Axis, 3, axisChoice);
	}

	wxString magnificationChoice[3] = { "x1","x2","x3" };
	//m_Gui->Combo(ID_AXIS, "Res", &m_Magnification, 3, magnificationChoice);
	m_Gui->Radio(NULL, "Res", &m_Magnification, 3, magnificationChoice,3);

	if (m_ShowExtractButton) 
	{
		m_Gui->Divider(1);
		m_Gui->Button(ID_EXTRACT, "Extract");
		m_Gui->Divider(1);

		if (m_ShowSliceList)
		{
			m_Gui->Label("Slice Images", 1);
			m_SlicesListBox = m_Gui->ListBox(ID_SLICES_LIST, "", 200);
			m_Gui->Divider();

			m_Gui->String(NULL, "Name", &m_ImageName);
			m_Gui->TwoButtons(ID_RENAME, ID_REMOVE, "Rename", "Remove");

			m_Gui->Enable(ID_RENAME, m_SlicesListBox->GetCount() != 0);
			m_Gui->Enable(ID_REMOVE, m_SlicesListBox->GetCount() != 0);
		}
		else
		{
			m_Gui->Divider();
			m_Gui->String(NULL, "Name", &m_ImageName);
		}
	}
	else
	{
		if (m_ChooseName)
		{
			m_Gui->Divider(1);
			m_Gui->String(NULL, "Name", &m_ImageName);
		}
	}

	//
	m_Gui->Label(&m_GuiMessage);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();
	m_Gui->Label("");

	ShowGui();
}

//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::UpdateListbox()
{
	if (m_SlicesListBox)
	{
		m_SlicesListBox->Clear();

		if (m_ImageSlicesGroup)
		{
			for (int i = 0; i < m_ImageSlicesGroup->GetNumberOfChildren(); i++)
			{
				if (m_ImageSlicesGroup->GetChild(i)->IsA("albaVMEImage"))
					m_SlicesListBox->Append(_(m_ImageSlicesGroup->GetChild(i)->GetName()));
			}

			m_Gui->Enable(ID_RENAME, m_SlicesListBox->GetCount() != 0);
			m_Gui->Enable(ID_REMOVE, m_SlicesListBox->GetCount() != 0);
			m_Gui->Update();
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::SelectImageSlice()
{
	m_CurrentImage = NULL;

	if (m_SlicesListBox)
	{
		if (m_ImageSlicesGroup)
		{
			int selection = m_SlicesListBox->GetSelection();

			if (selection >= 0 && selection < m_ImageSlicesGroup->GetNumberOfChildren())
				if (m_ImageSlicesGroup->GetChild(selection)->IsA("albaVMEImage"))
				{
					m_CurrentImage = (albaVMEImage*)m_ImageSlicesGroup->GetChild(selection);
					m_ImageName = m_CurrentImage->GetName();
					m_Gui->Update();
				}
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::RenameImageSlice()
{
	if (m_CurrentImage != NULL)
	{
		if (m_ImageSlicesGroup)
		{
			for (int i = 0; i < m_ImageSlicesGroup->GetNumberOfChildren(); i++)
			{
				if (m_ImageSlicesGroup->GetChild(i) == m_CurrentImage)
					m_ImageSlicesGroup->GetChild(i)->SetName(m_ImageName);
			}
		}		
	}

	m_ImageName = "";
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::RemoveImageSlice()
{
	if (m_CurrentImage != NULL)
	{
		if (m_ImageSlicesGroup)
		{
			for (int i = 0; i < m_ImageSlicesGroup->GetNumberOfChildren(); i++)
			{
				if (m_ImageSlicesGroup->GetChild(i) == m_CurrentImage)
					m_ImageSlicesGroup->RemoveChild(i);

				if (((i - 1) >= 0) && ((i - 1) < m_ImageSlicesGroup->GetNumberOfChildren()))
				{
					m_SlicesListBox->Select(i - 1);
				}

				((albaGUIValidator *)m_SlicesListBox->GetValidator())->TransferFromWindow();
				m_SlicesListBox->Update();
			}
		}
	}
}
//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::ShowImageSlice()
{
	if (m_CurrentImage != NULL)
	{
		if (m_CurrentImage->GetTagArray()->IsTagPresent("SLICE_MATRIX"))
		{
			albaTagItem *tagPoint = m_CurrentImage->GetTagArray()->GetTag("SLICE_MATRIX");

			// Matrix
			albaMatrix *matrix = NULL;
			albaNEW(matrix);

			int ind = 0;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
				{
					double value = tagPoint->GetValueAsDouble(ind++);
					matrix->SetElement(i, j, value);
				}

			if (m_View->IsA("albaViewArbitrarySlice"))
			{
				((albaViewArbitrarySlice*)m_View)->SetSlicerMatrix(matrix);
			}
			else if (m_View->IsA("albaViewArbitraryOrthoSlice"))
			{
				((albaViewArbitraryOrthoSlice*)m_View)->SetSlicerMatrix(matrix);
			}
		}
	}
}

//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::ExtractImage()
{
	// Find or Create Snapshots group
	albaVMEGroup *group = NULL;
	group = m_ImageSlicesGroup;

	if (group == NULL)
	{
		albaNEW(group);
		group->SetName("Slices");
		if (!m_ShowInTree) group->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
		group->ReparentTo(m_Input);
		m_ImageSlicesGroup = group;
		albaDEL(group);
	}

	//////////////////////////////////////////////////////////////////////////	
	// Create and Save Image
	albaVMEImage *vmeImage = NULL;
	albaNEW(vmeImage);

	vmeImage->SetData(GetSliceImageData(), 0);
	vmeImage->SetName(GenerateImageName());
	vmeImage->SetTimeStamp(0);
	vmeImage->ReparentTo(m_ImageSlicesGroup);

	SaveTags(vmeImage);

	m_CurrentImage = vmeImage;

	albaDEL(vmeImage);
}

//----------------------------------------------------------------------------
vtkImageData *albaOpExtractImageFromArbitraryView::GetSliceImageData()
{
	vtkRenderer *renderer = NULL;
	vtkRenderWindow *renderWindow = NULL;

	vtkNEW(renderer);
	vtkNEW(renderWindow);

	renderer->SetBackground(0.0, 0.0, 0.0);

	double bounds[6];
	m_Input->GetOutput()->GetVMELocalBounds(bounds);

	renderWindow->AddRenderer(renderer);
	renderWindow->SetSize(bounds[3] - bounds[2], bounds[1] - bounds[0]);
	renderWindow->SetPosition(0, 0);

	albaPipe *pipeSlice = NULL;
	if (m_View)
	{
		if (m_View->IsA("albaViewArbitrarySlice"))
		{
			pipeSlice = albaViewArbitrarySlice::SafeDownCast(m_View)->GetPipeSlice();
		}
		else if (m_View->IsA("albaViewArbitraryOrthoSlice"))
		{
			pipeSlice = albaViewArbitraryOrthoSlice::SafeDownCast(m_View)->GetPipeSlice(m_Axis);

			if (m_Axis == 0)
				renderWindow->SetSize(bounds[5] - bounds[4], bounds[3] - bounds[2]);
			else if (m_Axis == 1)
				renderWindow->SetSize(bounds[1] - bounds[0], bounds[5] - bounds[4]);
		}
	}
	if (pipeSlice == NULL) return NULL;

	vtkPropCollection *actorList = vtkPropCollection::New();
	pipeSlice->GetAssemblyFront()->GetActors(actorList);

	actorList->InitTraversal();
	vtkProp *actor = actorList->GetNextProp();
	while (actor)
	{
		renderer->AddActor(actor);
		renderWindow->Render();

		actor = actorList->GetNextProp();
	}

	// Set Camera Properties
	double x, y, z, vx, vy, vz;
	x = 0; y = 0; z = -1; vx = 0; vy = 1; vz = 0; // axis = Z

	renderer->GetActiveCamera()->ParallelProjectionOn();
	renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
	renderer->GetActiveCamera()->SetPosition(x * 100, y * 100, z * 100);
	renderer->GetActiveCamera()->SetViewUp(vx, vy, vz);
	renderer->GetActiveCamera()->SetClippingRange(0.1, 1000);
	renderer->GetActiveCamera()->Zoom(2.0);
	//renderer->ResetCamera();

	renderWindow->Render();

	int dim[3];
	renderWindow->OffScreenRenderingOn();

	vtkALBASmartPointer<vtkWindowToImageFilter> w2i;
	w2i->SetInput(renderWindow);
	w2i->SetMagnification(m_Magnification + 1);
	w2i->Update();
	w2i->GetOutput()->GetDimensions(dim);

	renderWindow->OffScreenRenderingOff();

	vtkDEL(renderer);
	vtkDEL(renderWindow);

	// Create ImageData
	vtkImageData *imageData = NULL;
	vtkNEW(imageData);

	imageData->SetNumberOfScalarComponents(3);
	imageData->SetScalarTypeToUnsignedChar();
	imageData->SetDimensions(dim[0], dim[1], 1);
	imageData->SetUpdateExtentToWholeExtent();
	assert(imageData->GetPointData());
	imageData->GetPointData()->SetScalars(w2i->GetOutput()->GetPointData()->GetScalars());

	return imageData;
}

//----------------------------------------------------------------------------
wxString albaOpExtractImageFromArbitraryView::GenerateImageName()
{
	wxString imageName = "Slice";
	wxString axisName[3] = { "X","Y","Z" };

	char tmp[20];
	int count = 1;
	do
	{
		sprintf(tmp, "%s_%s_%d", imageName, axisName[m_Axis], count);
		count++;
	} while (m_ImageSlicesGroup->FindInTreeByName(tmp) != NULL);

	imageName = tmp;

	if (m_ChooseName)
	{
		if (!m_ImageName.IsEmpty())
			imageName = m_ImageName;
	}

	return imageName;
}

//----------------------------------------------------------------------------
void albaOpExtractImageFromArbitraryView::SaveTags(albaVMEImage * image)
{
	// Visible in Tree
	if (!m_ShowInTree)
		image->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));

	// Extracted From View
	image->GetTagArray()->SetTag(albaTagItem("SLICE_EXTRACTED_FROM", m_View->GetTypeName()));

	// Axis
	wxString axis[3] = { "X","Y","Z" };
	image->GetTagArray()->SetTag(albaTagItem("SLICE_AXIS", axis[m_Axis]));

	// Matrix
	albaMatrix *matrix = NULL;

	if (m_View->IsA("albaViewArbitrarySlice"))
	{
		matrix = ((albaViewArbitrarySlice*)m_View)->GetSlicerMatrix();
	}
	else if (m_View->IsA("albaViewArbitraryOrthoSlice"))
	{
		matrix = ((albaViewArbitraryOrthoSlice*)m_View)->GetSlicerMatrix();
	}

	if (matrix)
	{
		albaTagItem tagMatrix;
		tagMatrix.SetName("SLICE_MATRIX");
		int ind = 0;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				tagMatrix.SetValue(matrix->GetElement(i, j), ind++);

		if (image->GetTagArray()->IsTagPresent("SLICE_MATRIX"))
			image->GetTagArray()->DeleteTag("SLICE_MATRIX");

		image->GetTagArray()->SetTag(tagMatrix);
	}

	//image->GetTagArray()->SetTag(albaTagItem("SLICE_MATRIX", textMatrix));
}