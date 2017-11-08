/*=========================================================================

 Program: MAF2
 Module:  mafGUIImageViewer
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

#include "mafGUIImageViewer.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafGUIButton.h"
#include "mafGUIDialog.h"
#include "mafGUILab.h"
#include "mafGUIPicButton.h"
#include "mafGUIValidator.h"
#include "mafGUIWizardPageNew.h"
#include "mafVME.h"
#include "mafVMEGroup.h"
#include "mafVMEImage.h"
#include "vtkBMPWriter.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkJPEGWriter.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkProbeFilter.h"
#include "vtkStructuredPoints.h"
#include "vtkUnsignedCharArray.h"
#include "wx\image.h"
#include "wx\mac\carbon\bitmap.h"

#define DIALOG_W 800
#define DIALOG_H 480

//----------------------------------------------------------------------------
mafGUIImageViewer::mafGUIImageViewer(mafObserver *Listener, const mafString &label)
{
	m_Listener = Listener;

	m_ImagesGroup = NULL;

	m_Dialog = NULL;
	m_ImageBoxSizer = NULL;
	m_PrevBtn = NULL;
	m_NextBtn = NULL;
	m_CheckBtn = NULL;

	m_ImageCheck = 0;
	m_ImageSelection = 0;
	m_ImagesList.clear();

	m_ImagesPath = mafGetAppDataDirectory().c_str();
	m_ImagesPath += "\\Preview\\";

	m_TitleDialog = "Image Viewer";

	m_EnableDeleteButton = false;
	m_EnableSaveImageButton= false;
	m_EnableImageCheckButton = false;
}

//----------------------------------------------------------------------------
mafGUIImageViewer::~mafGUIImageViewer()
{
	m_ImagesList.clear();
}

//----------------------------------------------------------------------------
void mafGUIImageViewer::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case ID_IMAGE_PREV:
		{
			if (m_EnableImageCheckButton) mafEventMacro(*e);
			UpdateSelectionDialog(m_ImageSelection - 1);
		}
		break;
		case ID_IMAGE_NEXT:
		{
			if (m_EnableImageCheckButton) mafEventMacro(*e);
			UpdateSelectionDialog(m_ImageSelection + 1);

		}
		break;
		case ID_IMAGE_SAVE:
		{
			SaveImageAs();
		}
		break;

// 		case ID_IMAGE_DELETE:
// 		{
// 			//DeleteImage();
// 		}
		break;

		case ID_IMAGE:
			break;

		default:
			mafEventMacro(*e);
			break;
		}
	}
}

//----------------------------------------------------------------------------
void mafGUIImageViewer::ShowImageDialog(mafVMEGroup *group, bool reloadList)
{
	wxString imageName = "";

	m_ImagesGroup = group;

	if (reloadList) Reset();

	if (m_ImagesGroup)
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
	wxString title = m_TitleDialog + " - " + imageName + " " + tmp;

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

		// PREV
		m_PrevBtn = new mafGUIButton(m_Dialog, ID_IMAGE_PREV, "Prev", wxPoint(-1, -1));
		m_PrevBtn->SetListener(this);
		buttonBoxSizer->Add(m_PrevBtn, 0, wxALIGN_CENTER, 0);

		// NEXT
		m_NextBtn = new mafGUIButton(m_Dialog, ID_IMAGE_NEXT, "Next", wxPoint(-1, -1));
		m_NextBtn->SetListener(this);
		buttonBoxSizer->Add(m_NextBtn, 0, wxALIGN_CENTER, 0);

		if (m_EnableDeleteButton) // DELETE IMAGE Button
		{
			mafGUILab *lab = new mafGUILab(m_Dialog, -1, "  ");
			buttonBoxSizer->Add(lab, 0, wxALIGN_CENTER, 0);

			mafGUIButton *delBtn = new mafGUIButton(m_Dialog, ID_IMAGE_DELETE, "Delete Image", wxPoint(-1, -1));
			delBtn->SetListener(this);
			buttonBoxSizer->Add(delBtn, 0, wxALIGN_CENTER, 0);
		}

		if (m_EnableSaveImageButton) // SAVE AS.. Button
		{
			mafGUILab *lab2 = new mafGUILab(m_Dialog, -1, "  ");
			buttonBoxSizer->Add(lab2, 0, wxALIGN_CENTER, 0);

			mafGUIButton *saveBtn = new mafGUIButton(m_Dialog, ID_IMAGE_SAVE, "Save Image As...", wxPoint(-1, -1));
			saveBtn->SetListener(this);
			buttonBoxSizer->Add(saveBtn, 0, wxALIGN_CENTER, 0);
		}

		if (m_EnableImageCheckButton)
		{
			mafGUILab *lab = new mafGUILab(m_Dialog, -1, "  ");
			buttonBoxSizer->Add(lab, 0, wxALIGN_CENTER, 0);

			m_CheckBtn = new wxCheckBox(m_Dialog, ID_IMAGE_SELECT, "Select", wxPoint(-1, -1));
			m_CheckBtn->SetValidator(mafGUIValidator(this, ID_IMAGE_SELECT, m_CheckBtn, &m_ImageCheck));
			buttonBoxSizer->Add(m_CheckBtn, 0, wxALIGN_CENTER, 0);
		}

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

	if(m_CheckBtn)			
		m_CheckBtn->SetValue(m_ImageCheck);

	m_PrevBtn->Enable(false);
	m_Dialog->ShowModal();
}

//----------------------------------------------------------------------------
wxString mafGUIImageViewer::GetSelectedImageName()
{
	if (m_ImagesList.size() > 0 && m_ImageSelection >= 0)
		return m_ImagesList[m_ImageSelection];

	return "";
}

//----------------------------------------------------------------------------
void mafGUIImageViewer::Reset()
{
	m_ImageSelection = 0;
	m_ImagesList.clear();
}

//----------------------------------------------------------------------------
void mafGUIImageViewer::FillImageList()
{
	wxString imageName = "";
	int nImages = m_ImagesGroup->GetNumberOfChildren(false);

	if (!wxDirExists(m_ImagesPath.c_str()))
	{
		wxMkdir(m_ImagesPath.c_str());
	}

	for (int i = 0; i < nImages; i++)
	{
		mafVMEImage *image = (mafVMEImage*)m_ImagesGroup->GetChild(i);

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
void mafGUIImageViewer::UpdateSelectionDialog(int selection)
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

		wxString title = m_TitleDialog + " - " + imageName + " " + tmp;
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

		if (m_CheckBtn)
			m_CheckBtn->SetValue(m_ImageCheck);

		m_Dialog->Fit();
		m_Dialog->Update();

		delete previewBitmap;
		delete previewImage;
	}
}

//----------------------------------------------------------------------------
int mafGUIImageViewer::SaveVMEImage(mafVMEImage *image, wxString imageFileName, bool scale)
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
void mafGUIImageViewer::SaveImageAs()
{
	if (m_ImageSelection >= 0 && m_ImageSelection < m_ImagesList.size())
	{
		mafString fileNameFullPath = mafGetDocumentsDirectory().c_str();
		fileNameFullPath.Append("\\NewImage.jpg");

		mafString wildc = "JPEG (*.jpg)|*.jpg; |Bitmap (*.bmp)|*.bmp";
		wxString newFileName = mafGetSaveFile(fileNameFullPath.GetCStr(), wildc, "Save Image as").c_str();

		if (m_ImagesGroup)
		{
			wxString imageName = m_ImagesList[m_ImageSelection];
			mafVMEImage *image = (mafVMEImage*)m_ImagesGroup->FindInTreeByName(imageName);

			if (SaveVMEImage(image, newFileName) == OP_RUN_OK)
				wxMessageBox("Image Saved!");
		}
	}
}

//----------------------------------------------------------------------------
void mafGUIImageViewer::AddImage(wxBitmap &bitmap, wxString name = "")
{
	if (m_ImagesGroup == NULL)
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
		} while (m_ImagesGroup->FindInTreeByName(tmp) != NULL);

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
	image->ReparentTo(m_ImagesGroup);

	mafDEL(image);
	//////////////////////////////////////////////////////////////////////////
	vtkDEL(buffer);
	vtkDEL(vtkimg);
}
//----------------------------------------------------------------------------
void mafGUIImageViewer::DeleteSelectedImage()
{
	if (m_ImagesGroup && m_ImagesList.size() > 0)
	{
		wxString imageName = m_ImagesList[m_ImageSelection];
		mafVME *node = m_ImagesGroup->FindInTreeByName(imageName);

		m_ImagesList.erase(m_ImagesList.begin() + m_ImageSelection);

		m_ImagesGroup->RemoveChild(node);

		UpdateSelectionDialog(m_ImageSelection - 1);
	}
}
//----------------------------------------------------------------------------
void mafGUIImageViewer::DeleteImage(int index)
{
	if (index >= 0 && index < m_ImagesList.size())
	{
		m_ImageSelection = index;
		DeleteSelectedImage();
	}
}

