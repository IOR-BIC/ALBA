/*=========================================================================

 Program: MAF2
 Module:  mafSnapshotManager
 Authors: Nicola Vanella
 
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

#include "mafDecl.h"
#include "mafSnapshotManager.h"

#include "mafLogicWithManagers.h"
#include "mafRWIBase.h"
#include "mafVMEGroup.h"

#include "mafView.h"
#include "mafViewCompound.h"
#include "mafViewManager.h"
#include "mafVME.h"
#include "mafTagItem.h"
#include "vtkUnsignedCharArray.h"
#include "mafVMEImage.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "mafTagArray.h"
#include "mafGUIImageViewer.h"
#include "mafDeviceButtonsPadMouse.h"

//----------------------------------------------------------------------------
mafSnapshotManager::mafSnapshotManager()
{
	m_SnapshotsGroup = NULL;
	m_ImageViewer = NULL;

	m_GroupName = "Snapshots";

	m_ImageViewer = new mafGUIImageViewer(this);
	m_ImageViewer->SetTitle("Snapshot Manager");
	m_ImageViewer->EnableDeleteButton(true);
	m_ImageViewer->EnableSaveButton(true);
	m_ImageViewer->SetListener(this);
}

//----------------------------------------------------------------------------
mafSnapshotManager::~mafSnapshotManager()
{
	cppDEL(m_ImageViewer);
}

//----------------------------------------------------------------------------
void mafSnapshotManager::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
void mafSnapshotManager::CreateSnapshot(mafVME *root, mafView *selectedView)
{
	CreateSnapshotGroup(root);
		
	//////////////////////////////////////////////////////////////////////////

	mafViewCompound *viewCompound = mafViewCompound::SafeDownCast(selectedView);
	
	wxImage img;

	if (viewCompound)
	{
		wxColor color = viewCompound->GetBackgroundColor();
		viewCompound->SetBackgroundColor(wxColor(255, 255, 255));

		wxBitmap btmComp;
		viewCompound->GetImage(btmComp);
		img = btmComp.ConvertToImage();

		viewCompound->SetBackgroundColor(color);
	}
	else if (selectedView)
	{
		wxColor color = selectedView->GetBackgroundColor();
		selectedView->SetBackgroundColor(wxColor(255, 255, 255));

		wxBitmap* image = selectedView->GetRWI()->GetImage();
		img = image->ConvertToImage();
		cppDEL(image);

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
	mafVMEImage *image;
	mafNEW(image);

	image->SetData(vtkimg, 0);
	image->SetName(imageName);
	image->SetTimeStamp(0);
	image->ReparentTo(m_SnapshotsGroup);

	mafDEL(image);
	//////////////////////////////////////////////////////////////////////////
	vtkDEL(buffer);
	vtkDEL(vtkimg);
}

//----------------------------------------------------------------------------
void mafSnapshotManager::CreateSnapshotGroup(mafVME *root)
{
	// Find or Create Snapshots group
	if (m_SnapshotsGroup == NULL || m_SnapshotsGroup != (mafVMEGroup *)root->FindInTreeByName(m_GroupName))
	{
		m_SnapshotsGroup = (mafVMEGroup *)root->FindInTreeByName(m_GroupName);

		if (m_SnapshotsGroup == NULL)
		{
			mafNEW(m_SnapshotsGroup);
			m_SnapshotsGroup->SetName(m_GroupName);
			m_SnapshotsGroup->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
			m_SnapshotsGroup->ReparentTo(root);
			m_SnapshotsGroup->Delete();
		}
	}
}

//----------------------------------------------------------------------------
void mafSnapshotManager::ShowSnapshotPreview()
{
	if (m_ImageViewer && m_SnapshotsGroup)
		m_ImageViewer->ShowImageDialog(m_SnapshotsGroup);
}

//----------------------------------------------------------------------------
int mafSnapshotManager::GetNSnapshots()
{
	if (m_SnapshotsGroup)
		return m_SnapshotsGroup->GetNumberOfChildren();
	else
		return 0;
}

//----------------------------------------------------------------------------
void mafSnapshotManager::SetMouse(mafDeviceButtonsPadMouse *mouse)
{
	m_ImageViewer->SetMouse(mouse);
}

