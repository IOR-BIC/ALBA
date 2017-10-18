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

#include "mafGUIButton.h"
#include "mafGUICheckListBox.h"
#include "mafGUIPicButton.h"
#include "mafGUIValidator.h"
#include "mafLogicWithManagers.h"
#include "mafRWIBase.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVMEGroup.h"
#include "mafVMEImage.h"
#include "mafView.h"
#include "mafViewCompound.h"
#include "mafViewManager.h"

#include "vtkBMPWriter.h"
#include "vtkDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkJPEGWriter.h"
#include "vtkMAFSmartPointer.h"
#include "vtkProbeFilter.h"
#include "vtkStructuredPoints.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPointData.h"
#include "mafRWI.h"
#include "mafGUIDialog.h"

#define DIALOG_W 800
#define DIALOG_H 480

//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------
enum
{
	ID_IMAGE = MINID,
	ID_IMAGE_PREV,
	ID_IMAGE_NEXT,
	ID_IMAGE_SELECT,
	ID_IMAGE_SAVE,
	ID_IMAGE_DELETE,
};

//----------------------------------------------------------------------------
mafSnapshotManager::mafSnapshotManager()
{
	m_SnapshotsGroup = NULL;

	m_Dialog = NULL;
	m_ImageBoxSizer = NULL;
	m_PrevBtn = NULL;
	m_NextBtn = NULL;

	m_ImageSelection = 0;
	m_ImagesList.clear();

	m_ImagesPath = mafGetAppDataDirectory().c_str();
	m_ImagesPath += "\\Preview\\";
}

//----------------------------------------------------------------------------
mafSnapshotManager::~mafSnapshotManager()
{
	m_ImagesList.clear();	
}

//----------------------------------------------------------------------------
void mafSnapshotManager::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case ID_IMAGE_PREV:
		{
			UpdateSelectionDialog(m_ImageSelection - 1);
		}
		break;
		case ID_IMAGE_NEXT:
		{
			UpdateSelectionDialog(m_ImageSelection + 1);
		}
		break;
		case ID_IMAGE_SAVE:
		{
			SaveImageAs();
		}
			break;

		case ID_IMAGE_DELETE:
		{
			DeleteImage();
		}
			break;

		case ID_IMAGE:
			break;

		default:
			break;
		}
	}
}

//----------------------------------------------------------------------------
void mafSnapshotManager::CreateSnapshot(mafVME *root, mafView *selectedView)
{
	// Find or Create snapshots group
	
	if (m_SnapshotsGroup==NULL || m_SnapshotsGroup != (mafVMEGroup *)root->FindInTreeByName("Snapshots"))
	{
		m_SnapshotsGroup = (mafVMEGroup *)root->FindInTreeByName("Snapshots");

		if (m_SnapshotsGroup == NULL)
		{
			mafNEW(m_SnapshotsGroup);
			m_SnapshotsGroup->SetName("Snapshots");
			m_SnapshotsGroup->GetTagArray()->SetTag(mafTagItem("VISIBLE_IN_THE_TREE", 0.0));
			m_SnapshotsGroup->ReparentTo(root);
			m_SnapshotsGroup->Delete();
		}
		m_ImagesList.clear();	
	}

	
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
void mafSnapshotManager::AddSnapshot(wxBitmap &bitmap, wxString name = "")
{
	if (m_SnapshotsGroup == NULL)
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////

	wxImage img;

	if (&bitmap)
	{
		img = &bitmap.ConvertToImage();
	}

	// Generate Snapshot name
	wxString imageName = name;

	if (name == "") 
	{
		imageName = "Snapshot";

		char tmp[20];
		int count = 1;
		do
		{
			sprintf(tmp, "%s_%d", imageName, count);
			count++;
		} while (m_SnapshotsGroup->FindInTreeByName(tmp) != NULL);

		imageName = tmp;
	}

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
void mafSnapshotManager::ShowSnapshotPreview(mafVME *node)
{
	wxString imageName = "";

	m_SnapshotsGroup = (mafVMEGroup *)node->GetRoot()->FindInTreeByName("Snapshots");

	if (m_SnapshotsGroup)
	{
		FillImageList();
	}
	else
	{
		m_ImagesList.clear();
		m_ImageSelection = 0;
		return;
	}

	if (m_ImagesList.size() <= 0) return;
	//////////////////////////////////////////////////////////////////////////

	imageName = m_ImagesList[0];
	wxString imgPath = m_ImagesPath + imageName + ".bmp";

	char tmp[10];
	sprintf(tmp, "[1/%d]", m_ImagesList.size());
	wxString title = "Snapshots Manager - " + imageName + " " + tmp;

	//////////////////////////////////////////////////////////////////////////

	if (m_Dialog == NULL)
	{
		m_Dialog = new mafGUIDialog(title, mafCLOSEWINDOW);
		m_ImageBoxSizer = new wxBoxSizer(wxVERTICAL);
		m_ImageBoxSizer->SetMinSize(DIALOG_W, DIALOG_H);
		
		// Images
		if (wxFileExists(imgPath))
		{
			// Load and show the image
			wxImage *previewImage;
			previewImage = new wxImage();
			previewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

			wxBitmap *previewBitmap;
			previewBitmap = new wxBitmap(*previewImage);
			m_PreviewImageButton = new mafGUIPicButton(m_Dialog, previewBitmap, ID_IMAGE);

			m_ImageBoxSizer->Add(m_PreviewImageButton, 0, wxALL | wxALIGN_CENTER, 0);

			delete previewBitmap;
			delete previewImage;
		}

		// Creating buttons
		wxBoxSizer *buttonBoxSizer = new wxBoxSizer(wxHORIZONTAL);

		m_PrevBtn = new mafGUIButton(m_Dialog, ID_IMAGE_PREV, "Prev", wxPoint(-1, -1));
		m_PrevBtn->SetListener(this);
		buttonBoxSizer->Add(m_PrevBtn, 0, wxALIGN_CENTER, 0);

		m_NextBtn = new mafGUIButton(m_Dialog, ID_IMAGE_NEXT, "Next", wxPoint(-1, -1));
		m_NextBtn->SetListener(this);
		buttonBoxSizer->Add(m_NextBtn, 0, wxALIGN_CENTER, 0);

		mafGUILab *lab = new mafGUILab(m_Dialog, -1, "  ");
		buttonBoxSizer->Add(lab, 0, wxALIGN_CENTER, 0);

		mafGUIButton *delBtn = new mafGUIButton(m_Dialog, ID_IMAGE_DELETE, "Delete Image", wxPoint(-1, -1));
		delBtn->SetListener(this);
		buttonBoxSizer->Add(delBtn, 0, wxALIGN_CENTER, 0);

		mafGUILab *lab2 = new mafGUILab(m_Dialog, -1, "  ");
		buttonBoxSizer->Add(lab2, 0, wxALIGN_CENTER, 0);

		mafGUIButton *saveBtn = new mafGUIButton(m_Dialog, ID_IMAGE_SAVE, "Save Image As...", wxPoint(-1, -1));
		saveBtn->SetListener(this);
		buttonBoxSizer->Add(saveBtn, 0, wxALIGN_CENTER, 0);

		m_Dialog->Add(m_ImageBoxSizer, 0, wxALL);
		m_Dialog->Add(buttonBoxSizer, 0, wxALL, 5);
		m_Dialog->Fit();

		// Show dialog
		wxSize s = mafGetFrame()->GetSize();
		wxPoint p = mafGetFrame()->GetPosition();
		int posX = p.x + s.GetWidth() * .5 - m_Dialog->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - m_Dialog->GetSize().GetHeight() * .5;

		m_Dialog->SetPosition(wxPoint(posX, posY));
	}
	else
	{
		UpdateSelectionDialog(0);
	}
	
	m_PrevBtn->Enable(false);
	m_Dialog->ShowModal();
}
//----------------------------------------------------------------------------
void mafSnapshotManager::FillImageList()
{
	wxString imageName = "";
	int nImages = m_SnapshotsGroup->GetNumberOfChildren(false);

	if (!wxDirExists(m_ImagesPath.c_str()))
	{
		wxMkdir(m_ImagesPath.c_str());
	}

	for (int i = 0; i < nImages; i++)
	{
		mafVMEImage *image = (mafVMEImage*)m_SnapshotsGroup->GetChild(i);

		imageName = image->GetName();

		bool newImage = true;

		for (int i = 0; i < m_ImagesList.size(); i++)
		{
			if (m_ImagesList[i] == imageName)
			{
				newImage = false;
				break;
			}
		}

		if (newImage)
		{
			m_ImagesList.push_back(imageName);

			wxString imageFileName = m_ImagesPath + imageName + ".bmp";

			SaveVMEImage(image, imageFileName, true);
		}
	}
}

//----------------------------------------------------------------------------
void mafSnapshotManager::UpdateSelectionDialog(int selection)
{
	int nImages = m_ImagesList.size();

	m_PrevBtn->Enable(nImages > 0);
	m_NextBtn->Enable(nImages > 0);

	if (selection <= 0)
	{
		selection = 0;
		m_PrevBtn->Enable(false);
	}

	if (selection >= nImages - 1)
	{
		selection = nImages - 1;
		m_NextBtn->Enable(false);
	}

	if (nImages == 0) return;
	//if (m_ImageSelection == selection) return;
	if (m_ImageSelection > nImages) return;

	m_ImageSelection = selection;

	wxString imageName = m_ImagesList[m_ImageSelection];
	wxString imgPath = m_ImagesPath + imageName + ".bmp";

	//////////////////////////////////////////////////////////////////////////
	if (wxFileExists(imgPath) && m_Dialog)
	{
		char tmp[10];
		sprintf(tmp, "[%d/%d]", selection + 1, nImages);

		wxString title = "Snapshots Manager - " + imageName + " " + tmp;
		m_Dialog->SetTitle(title);

		// Load and show the image
		wxImage *previewImage;
		previewImage = new wxImage();
		previewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);

		wxBitmap *previewBitmap;
		previewBitmap = new wxBitmap(*previewImage);

		mafGUIPicButton *previewImageButton;
		previewImageButton = new mafGUIPicButton(m_Dialog, previewBitmap, ID_IMAGE);

		m_ImageBoxSizer->Remove(m_PreviewImageButton);
		m_ImageBoxSizer->Clear();

		if (m_PreviewImageButton)
			delete m_PreviewImageButton;

		m_PreviewImageButton = previewImageButton;

		m_ImageBoxSizer->Add(m_PreviewImageButton, 0, wxALL | wxALIGN_CENTER, 0);

		m_PreviewImageButton->Enable(false);

		m_Dialog->Fit();
		m_Dialog->Update();
				
		delete previewBitmap;
		delete previewImage;
	}
}

//----------------------------------------------------------------------------
int mafSnapshotManager::SaveVMEImage(mafVMEImage *image, wxString imageFileName, bool scale)
{
	int result = OP_RUN_CANCEL;

	if (image && imageFileName != "")
	{
		wxString pathName, fileName, extension;
		wxSplitPath(imageFileName, &pathName, &fileName, &extension);

		// Flip y axis
		vtkMAFSmartPointer<vtkImageFlip> imageFlipFilter;
		imageFlipFilter->SetFilteredAxis(1);
		imageFlipFilter->SetInput((vtkImageData*)image->GetOutput()->GetVTKData());
		imageFlipFilter->Update();

		vtkImageData *imageData = imageFlipFilter->GetOutput();
		vtkMAFSmartPointer<vtkProbeFilter> probeFilter;

		if (scale)
		{
			// Scale image
			double scaling = 1.0;
			double origin[3] = { 0,0,0 };

			double b[6];

			image->GetOutput()->GetBounds(b);

			double w = b[1];
			double h = b[3];

			scaling = MAX(1, MAX(w / DIALOG_W, h / DIALOG_H));

			origin[0] = -((DIALOG_W - (w / scaling)) / 2)*scaling;
			origin[1] = -((DIALOG_H - (h / scaling)) / 2)*scaling;

			vtkStructuredPoints *SP = vtkStructuredPoints::New();
			SP->SetOrigin(origin[0], origin[1], origin[2]);
			SP->SetDimensions(DIALOG_W, DIALOG_H, 1);
			SP->SetSpacing(scaling, scaling, 1);

			probeFilter->SetInput(SP);
			probeFilter->SetSource(imageFlipFilter->GetOutput());
			probeFilter->Update();

			imageData = (vtkImageData*)probeFilter->GetOutput();

			SP->Delete();
		}

		//////////////////////////////////////////////////////////////////////////

		if (extension == "bmp")
		{
			vtkBMPWriter *imageBMPWriter;
			vtkNEW(imageBMPWriter);

			// Save image
			imageBMPWriter->SetInput(imageData);
			imageBMPWriter->SetFileName(imageFileName);
			imageBMPWriter->Write();

			vtkDEL(imageBMPWriter);

			result = OP_RUN_OK;
		}
		else if (extension == "jpg")
		{
			vtkJPEGWriter *imageWriter;
			vtkNEW(imageWriter);

			// Save image
			imageWriter->SetInput(imageData);
			imageWriter->SetFileName(imageFileName);
			// 			int q = imageWriter->GetQuality()
			// 			imageWriter->SetQuality(95);
			imageWriter->Write();

			vtkDEL(imageWriter);

			result = OP_RUN_OK;
		}
	}

	return result;
}
//----------------------------------------------------------------------------
void mafSnapshotManager::SaveImageAs()
{
	if (m_ImagesList.size() > m_ImageSelection)
	{
		mafString fileNameFullPath = mafGetDocumentsDirectory().c_str();
		fileNameFullPath.Append("\\NewImage.jpg");

		mafString wildc = "JPEG (*.jpg)|*.jpg; |Bitmap (*.bmp)|*.bmp";
		wxString newFileName = mafGetSaveFile(fileNameFullPath.GetCStr(), wildc, "Save Image as").c_str();

		if (m_SnapshotsGroup)
		{
			wxString imageName = m_ImagesList[m_ImageSelection];
			mafVMEImage *image = (mafVMEImage*)m_SnapshotsGroup->FindInTreeByName(imageName);

			if (SaveVMEImage(image, newFileName) == OP_RUN_OK)
				wxMessageBox("Image Saved!");
		}
	}
}
//----------------------------------------------------------------------------
void mafSnapshotManager::DeleteImage()
{
	if (m_SnapshotsGroup && m_ImagesList.size() > 0)
	{
		wxString imageName = m_ImagesList[m_ImageSelection];
		mafVME *node = m_SnapshotsGroup->FindInTreeByName(imageName);

		m_ImagesList.erase(m_ImagesList.begin() + m_ImageSelection);

		m_SnapshotsGroup->RemoveChild(node);
		//node->ReparentTo(NULL);
		
		UpdateSelectionDialog(m_ImageSelection - 1);
	}
}