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

//----------------------------------------------------------------------------
albaSnapshotManager::albaSnapshotManager()
{
	m_SnapshotsGroup = NULL;
	m_ImageViewer = NULL;

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
		selectedView->SetBackgroundColor(wxColor(255, 255, 255));

		wxBitmap btmComp;
		selectedView->GetImage(btmComp);
		img = btmComp.ConvertToImage();

		selectedView->SetBackgroundColor(color);
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
	vtkimg->SetNumberOfScalarComponents(NumberOfComponents);
	vtkimg->SetScalarTypeToUnsignedChar();
	vtkimg->SetDimensions(img.GetWidth(), img.GetHeight(), 1);
	vtkimg->SetUpdateExtentToWholeExtent();
	assert(vtkimg->GetPointData());
	vtkimg->GetPointData()->SetScalars(buffer);

	//////////////////////////////////////////////////////////////////////////	
	albaVMEImage *image;
	albaNEW(image);

	image->SetData(vtkimg, 0);
	image->SetName(imageName);
	image->SetTimeStamp(0);
	image->ReparentTo(m_SnapshotsGroup);

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
		m_ImageViewer->ShowImageDialog(m_SnapshotsGroup);
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
