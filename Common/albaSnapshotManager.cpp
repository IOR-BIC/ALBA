/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module:  albaSnapshotManager
 Authors: Nicola Vanella
 
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

#include "albaDecl.h"
#include "albaSnapshotManager.h"

#include "albaDeviceButtonsPadMouse.h"
#include "albaGUIImageViewer.h"
#include "albaLogicWithManagers.h"
#include "albaRWIBase.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaVME.h"
#include "albaVMEGroup.h"
#include "albaVMEImage.h"
#include "albaView.h"
#include "albaViewCompound.h"
#include "albaViewManager.h"

#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"
#include "albaGUI.h"

//----------------------------------------------------------------------------
albaSnapshotManager::albaSnapshotManager(/*albaObserver *Listener, const albaString &label*/) :
	albaGUISettings(this, _("Snapshot Manager"))
{
	m_SnapshotsGroup = NULL;
	m_ImageViewer = NULL;
	m_SettingsGui = NULL;

	InitializeSettings();

	m_GroupName = "Snapshots";

	m_ImageViewer = new albaGUIImageViewer(this);
	m_ImageViewer->SetTitle("Snapshot Manager");
	m_ImageViewer->EnableDeleteButton(true);
	m_ImageViewer->EnableSaveButton(true);
	m_ImageViewer->SetListener(this);
}

//----------------------------------------------------------------------------
albaSnapshotManager::~albaSnapshotManager()
{
	cppDEL(m_ImageViewer);
	//albaDEL(m_SnapshotsGroup);
}

//----------------------------------------------------------------------------
void albaSnapshotManager::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		switch (e->GetId())
		{
		case ID_IMAGE_DELETE:
		{
			m_ImageViewer->DeleteSelectedImage();
		}
		break;

		case ID_SETTING_ENABLE_RESOLUTION:
		{
			m_SettingsGui->Enable(ID_SETTING_RESOLUTION, m_SnapshotsCustomResoluzion);
			m_SettingsGui->Update();
			m_Config->Write("Snapshot_CustomResolution", m_SnapshotsCustomResoluzion);
			m_Config->Flush();
		}
		break;
		case  ID_SETTING_RESOLUTION:
		{
			m_Config->Write("Snapshot_WidthResolution", m_SnapshotsWidth);
			m_Config->Write("Snapshot_HeightResolution", m_SnapshotsHeight);
			m_Config->Flush();
		}
		break;

		case ID_SETTING_COLOR:
		{
			m_Config->Write("Snapshot_ColorBackgroundR", m_ColorBackground.Red());
			m_Config->Write("Snapshot_ColorBackgroundB", m_ColorBackground.Blue());
			m_Config->Write("Snapshot_ColorBackgroundG", m_ColorBackground.Green());
			m_Config->Flush();
		}
		break;

		default:
			break;
		}
	}
}

//----------------------------------------------------------------------------
void albaSnapshotManager::CreateSnapshot(albaVME *root, albaView *selectedView)
{
	FindOrCreateSnapshotGroup(root);
		
	//////////////////////////////////////////////////////////////////////////

	wxImage img;

	if (selectedView)
	{
		wxColor color = selectedView->GetBackgroundColor();
		selectedView->SetBackgroundColor(m_ColorBackground);

		wxBitmap btmComp;
		wxSize wSize = selectedView->GetWindow()->GetSize();

		float w = wSize.x;
		float h = wSize.y;

		if (m_SnapshotsCustomResoluzion)
		{
			w = MAX(wSize.x, m_SnapshotsWidth);
			h = (w == wSize.x) ? wSize.y : w * wSize.y / wSize.x;

			if(h<m_SnapshotsHeight)
			{
				h = MAX(wSize.y, m_SnapshotsHeight);
				w = (h == wSize.y) ? wSize.x : h * wSize.x / wSize.y;
			}
		}

		selectedView->SetWindowSize(w, h);
		selectedView->CameraUpdate();
		selectedView->GetImage(btmComp);
		selectedView->SetWindowSize(wSize.x, wSize.y);;
		selectedView->SetBackgroundColor(color);
		img = btmComp.ConvertToImage();
	}
	else
	{
		return;
	}

	// Generate Snapshot name
	wxString imageName = "Snapshot";
 	
	char tmp[20];
	int count = 1;
	do
	{
		sprintf(tmp, "%s_%d", imageName, count);
		count++;
	} 
	while (m_SnapshotsGroup->FindInTreeByName(tmp) != NULL);

	imageName = tmp;

	//////////////////////////////////////////////////////////////////////////
	
	int NumberOfComponents = 3;
	vtkUnsignedCharArray *buffer;
	vtkNEW(buffer);
	buffer->SetNumberOfComponents(NumberOfComponents);

	unsigned char *p = img.GetData();
	assert(p);
	for (int i = 0; i < img.GetWidth() * img.GetHeight(); i++)
	{
		unsigned char r = *p++;
		unsigned char g = *p++;
		unsigned char b = *p++;

		buffer->InsertNextTuple3(r, g, b);
	}

	vtkImageData *vtkimg;
	vtkNEW(vtkimg);
	vtkimg->AllocateScalars(VTK_UNSIGNED_CHAR, NumberOfComponents);
	vtkimg->SetDimensions(img.GetWidth(), img.GetHeight(), 1);
	assert(vtkimg->GetPointData());
	vtkimg->GetPointData()->SetScalars(buffer);

	//////////////////////////////////////////////////////////////////////////	
	albaVMEImage *image;
	albaNEW(image);

	image->SetData(vtkimg, 0);
	image->SetName(imageName);
	image->SetTimeStamp(0);
	image->ReparentTo(m_SnapshotsGroup);
	image->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));

	albaDEL(image);
	//////////////////////////////////////////////////////////////////////////
	vtkDEL(buffer);
	vtkDEL(vtkimg);
}

//----------------------------------------------------------------------------
void albaSnapshotManager::FindOrCreateSnapshotGroup(albaVME *root)
{
	// Find or Create Snapshots group
	if (m_SnapshotsGroup == NULL || m_SnapshotsGroup != (albaVMEGroup *)root->FindInTreeByName(m_GroupName))
	{
		m_SnapshotsGroup = (albaVMEGroup *)root->FindInTreeByName(m_GroupName);

		if (m_SnapshotsGroup == NULL)
		{
			albaNEW(m_SnapshotsGroup);
			m_SnapshotsGroup->SetName(m_GroupName);
			m_SnapshotsGroup->GetTagArray()->SetTag(albaTagItem("VISIBLE_IN_THE_TREE", 0.0));
			m_SnapshotsGroup->ReparentTo(root);
			m_SnapshotsGroup->Delete();
		}
	}
}

//----------------------------------------------------------------------------
void albaSnapshotManager::ShowSnapshotPreview()
{
	if (m_ImageViewer && m_SnapshotsGroup)
	{
		int numImages = GetNSnapshots(); 

		if (numImages > 0) // Select Last Image
			m_ImageViewer->ShowImageDialog(m_SnapshotsGroup, numImages-1);
		else
			m_ImageViewer->ShowImageDialog(m_SnapshotsGroup);
	}
}

//----------------------------------------------------------------------------
int albaSnapshotManager::GetNSnapshots()
{
	if (m_SnapshotsGroup)
		return m_SnapshotsGroup->GetNumberOfChildren();
	else
		return 0;
}

//----------------------------------------------------------------------------
bool albaSnapshotManager::HasSnapshots(albaVME *root)
{
	if (m_SnapshotsGroup == NULL || m_SnapshotsGroup != (albaVMEGroup *)root->FindInTreeByName(m_GroupName))
		m_SnapshotsGroup = (albaVMEGroup *)root->FindInTreeByName(m_GroupName);

	return m_SnapshotsGroup && m_SnapshotsGroup->GetNumberOfChildren() > 0;
}

//----------------------------------------------------------------------------
void albaSnapshotManager::SetMouse(albaDeviceButtonsPadMouse *mouse)
{
	m_ImageViewer->SetMouse(mouse);
}

//----------------------------------------------------------------------------
albaGUI* albaSnapshotManager::GetSettingsGui()
{
	if (m_SettingsGui == NULL)
	{
		m_SettingsGui = new albaGUI(this);
		
		m_SettingsGui->Label("Snapshot Settings", true);
		m_SettingsGui->Label("");

		m_SettingsGui->Label("Image Resolution");
		m_SettingsGui->Bool(ID_SETTING_ENABLE_RESOLUTION, "Customize", &m_SnapshotsCustomResoluzion, 1);
		m_SettingsGui->Integer(ID_SETTING_RESOLUTION, "Min Width", &m_SnapshotsWidth, 800);
		m_SettingsGui->Integer(ID_SETTING_RESOLUTION, "Min Height", &m_SnapshotsHeight, 600);
		m_SettingsGui->Divider(1);

		m_SettingsGui->Label("Background");
		m_SettingsGui->Color(ID_SETTING_COLOR, "Color", &m_ColorBackground);
		m_SettingsGui->Label("");

		m_SettingsGui->Enable(ID_SETTING_RESOLUTION, m_SnapshotsCustomResoluzion);
	}

	return m_SettingsGui;
}

//----------------------------------------------------------------------------
void albaSnapshotManager::InitializeSettings()
{
	m_SnapshotsCustomResoluzion = 0;
	m_SnapshotsWidth = 800;
	m_SnapshotsHeight = 600;
	int m_Color[3];
	m_Color[0] = m_Color[1] = m_Color[2] = 255;

	//On first run i cannot read configuration
	if (!m_Config->Read("Snapshot_CustomResolution", &m_SnapshotsCustomResoluzion))
		m_Config->Write("Snapshot_CustomResolution", m_SnapshotsCustomResoluzion); // So i will save default value

	if (!m_Config->Read("Snapshot_WidthResolution", &m_SnapshotsWidth))
		m_Config->Write("Snapshot_WidthResolution", m_SnapshotsWidth); // So i will save default value

	if (!m_Config->Read("SnapshotHeightResolution", &m_SnapshotsHeight))
		m_Config->Write("Snapshot_HeightResolution", m_SnapshotsHeight); // So i will save default value

	if (!m_Config->Read("Snapshot_ColorBackgroundR", &m_Color[0]))
		m_Config->Write("Snapshot_ColorBackgroundR", m_Color[0]); // So i will save default value

	if (!m_Config->Read("Snapshot_ColorBackgroundB", &m_Color[1]))
		m_Config->Write("Snapshot_ColorBackgroundB", m_Color[1]); // So i will save default value

	if (!m_Config->Read("Snapshot_ColorBackgroundG", &m_Color[2]))
		m_Config->Write("Snapshot_ColorBackgroundG", m_Color[2]); // So i will save default value
	
	m_ColorBackground = wxColor(m_Color[0], m_Color[1], m_Color[2]);

	m_Config->Flush();
}