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

#include "albaAbsLogicManager.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"
#include "albaOp.h"
#include "albaPipe.h"
#include "albaPipeVolumeArbSlice.h"
#include "albaServiceClient.h"
#include "albaTagArray.h"
#include "albaVME.h"
#include "albaVMEImage.h"
#include "albaView.h"
#include "albaViewArbitraryOrthoSlice.h"
#include "albaViewArbitrarySlice.h"
#include "albaViewManager.h"

#include "vtkImageData.h"
#include "vtkImageReslice.h"
#include "vtkTransform.h"

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
	
	m_SlicesListBox = NULL;

	m_CurrentImage = NULL;
	m_ImageName = "";

	m_Reslice = NULL;
	vtkNEW(m_Reslice);
}

//----------------------------------------------------------------------------
albaOpExtractImageFromArbitraryView::~albaOpExtractImageFromArbitraryView()
{
	vtkDEL(m_Reslice);
}

//----------------------------------------------------------------------------
bool albaOpExtractImageFromArbitraryView::InternalAccept(albaVME *node)
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
	if (!m_TestMode)
	{
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

	if (m_View && m_View->IsA("albaViewArbitraryOrthoSlice"))
	{
		m_Gui->Label("");
		wxString axisChoice[3] = { "Sagittal", "Coronal", "Axial" };
		m_Gui->Combo(ID_AXIS, "Plane", &m_Axis, 3, axisChoice);
	}
	
	m_Gui->Label("Slice Images", 1);
	m_SlicesListBox = m_Gui->ListBox(ID_SLICES_LIST, "", 200);
	m_Gui->Divider();

	m_Gui->String(NULL, "Name", &m_ImageName);
	m_Gui->TwoButtons(ID_RENAME, ID_REMOVE, "Rename", "Remove");

	m_Gui->Divider(1);
	m_Gui->Button(ID_EXTRACT, "Extract");
	m_Gui->Divider(1);

	m_Gui->Enable(ID_RENAME, m_SlicesListBox->GetCount() != 0);
	m_Gui->Enable(ID_REMOVE, m_SlicesListBox->GetCount() != 0);

	//////////////////////////////////////////////////////////////////////////
	m_Gui->Label("");
	m_Gui->Divider(1);
	//m_Gui->OkCancel();
	m_Gui->Button(wxCANCEL, "Close");
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
	if (m_CurrentImage != NULL && !m_ImageName.IsEmpty())
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
			if (m_View->IsA("albaViewArbitrarySlice"))
				((albaViewArbitrarySlice*)m_View)->RestoreFromVME(m_CurrentImage);
			else if (m_View->IsA("albaViewArbitraryOrthoSlice"))
				((albaViewArbitraryOrthoSlice*)m_View)->RestoreFromVME(m_CurrentImage);
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
	
	vtkImageData *imageData = GetSliceImageData();
	
	vmeImage->SetData(imageData, 0);
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
	double bounds[6];
	vtkImageData *imageData = NULL;
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
		}
	}

	if (!pipeSlice) return NULL;

	imageData = ((albaPipeVolumeArbSlice*)pipeSlice)->GetImageData();

	//return imageData;

	// Rotate Image
	imageData->GetBounds(bounds);

	double angle = -90;
	double center[3];
	center[0] = (bounds[1] + bounds[0]) / 2.0;
	center[1] = (bounds[3] + bounds[2]) / 2.0;
	center[2] = (bounds[5] + bounds[4]) / 2.0;

	if (m_Axis == 0 || m_Axis == 1) angle = 180;

	vtkTransform *transform = NULL;
	vtkNEW(transform);
	transform->Translate(center[0], center[1], center[2]);
	transform->RotateWXYZ(angle, 0, 0, 1);
	transform->Translate(-center[0], -center[1], -center[2]);

		// Reslice does all of the work
	m_Reslice->SetInputData(imageData);
	m_Reslice->SetResliceTransform(transform);
	m_Reslice->SetInterpolationModeToCubic();
	m_Reslice->SetOutputSpacing(imageData->GetSpacing());
	m_Reslice->SetOutputOrigin(imageData->GetOrigin());
	m_Reslice->SetOutputExtent(imageData->GetExtent());

	vtkDEL(transform);

	return m_Reslice->GetOutput();
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

	if (!m_ImageName.IsEmpty())
		imageName = m_ImageName;

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


	if (m_View->IsA("albaViewArbitrarySlice"))
	{
		((albaViewArbitrarySlice*)m_View)->SetRestoreTagToVME(image);
	}
	else if (m_View->IsA("albaViewArbitraryOrthoSlice"))
	{
		((albaViewArbitraryOrthoSlice*)m_View)->SetRestoreTagToVME(image);
	}


}