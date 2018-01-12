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
#include "mafDeviceButtonsPadMouse.h"
#include "mafGUI.h"
#include "mafGUIButton.h"
#include "mafGUIDialog.h"
#include "mafGUILab.h"
#include "mafGUIValidator.h"
#include "mafGUIWizardPageNew.h"
#include "mafRWI.h"
#include "mafVME.h"
#include "mafVMEGroup.h"
#include "mafVMEImage.h"

#include "vtkActor.h"
#include "vtkBMPWriter.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkImageFlip.h"
#include "vtkJPEGWriter.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPlaneSource.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataSource.h"
#include "vtkProbeFilter.h"
#include "vtkRenderer.h"
#include "vtkStructuredPoints.h"
#include "vtkTexture.h"
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
	m_PrevBtn = NULL;
	m_NextBtn = NULL;
	m_CheckBtn = NULL;

	m_ImageCheck = 0;
	m_ImageSelection = 0;
	m_ImagesList.clear();

	m_TitleDialog = "Image Viewer";

	m_EnableDeleteButton = false;
	m_EnableSaveImageButton= false;
	m_EnableImageCheckButton = false;
	
	m_RwiSizer = NULL;
	m_Rwi = NULL;

	m_PlaneSource = NULL;
	m_DataMapper = NULL;
	m_Texture = NULL;
	m_Actor = NULL;
}

//----------------------------------------------------------------------------
mafGUIImageViewer::~mafGUIImageViewer()
{
	m_ImagesList.clear();

	if (m_Rwi)
	{
		m_Rwi->m_RenFront->RemoveActor(m_Actor);
		cppDEL(m_Rwi);
	}

	delete m_NextBtn;
	delete m_PrevBtn;
	delete m_CheckBtn;

	vtkDEL(m_Texture);
	vtkDEL(m_PlaneSource);
	vtkDEL(m_DataMapper);
	vtkDEL(m_Actor);

	m_Dialog->DestroyChildren();
	cppDEL(m_Dialog);
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
		case ID_IMAGE_PRINT:
		{
			if (m_ImageSelection >= 0 && m_ImageSelection < m_ImagesList.size())
			{
				wxString imageName = m_ImagesList[m_ImageSelection];
				mafVMEImage *image = (mafVMEImage*)m_ImagesGroup->FindInTreeByName(imageName);
				GetLogicManager()->PrintImage(image);
			}
		}
		break;
		case ID_IMAGE_SAVE:
		{
			SaveImageAs();
		}
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
void mafGUIImageViewer::ShowImageDialog(mafVMEGroup *group, int selection)
{
	m_ImagesGroup = group;

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
	wxString imageName = m_ImagesList[0];

	char tmp[10];
	sprintf(tmp, "[1/%d]", m_ImagesList.size());
	wxString title = m_TitleDialog + " - " + imageName + " " + tmp;

	//////////////////////////////////////////////////////////////////////////

	if (m_Dialog == NULL)
	{
		m_Dialog = new mafGUIDialog(title, mafCLOSEWINDOW);

		// Images
		//////////////////////////////////////////////////////////////////////////
		vtkNEW(m_Texture);
		vtkNEW(m_PlaneSource);
		vtkNEW(m_DataMapper);
		vtkNEW(m_Actor);

		m_Texture->InterpolateOff();
		m_DataMapper->SetInput(m_PlaneSource->GetOutput());
		m_Actor->SetMapper(m_DataMapper);
		m_Actor->SetTexture(m_Texture);
		
		m_RwiSizer = new wxBoxSizer(wxHORIZONTAL);

		m_Rwi = new mafRWI(m_Dialog, ONE_LAYER);
		m_Rwi->SetListener(this);
		m_Rwi->SetSize(0, 0, DIALOG_W, DIALOG_H);
		m_Rwi->Show(true);
		m_Rwi->CameraSet(CAMERA_CT);
		m_Rwi->CameraUpdate();

		m_Rwi->m_RwiBase->SetMouse(m_Mouse);
		m_Rwi->m_RenFront->AddActor(m_Actor);

		m_RwiSizer->Add(m_Rwi->m_RwiBase, 1, wxEXPAND);

		//////////////////////////////////////////////////////////////////////////

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

		mafGUILab *printLab = new mafGUILab(m_Dialog, -1, "  ");
		buttonBoxSizer->Add(printLab, 0, wxALIGN_CENTER, 0);

		mafGUIButton *printBtn = new mafGUIButton(m_Dialog, ID_IMAGE_PRINT, "Print...", wxPoint(-1, -1));
		printBtn->SetListener(this);
		buttonBoxSizer->Add(printBtn, 0, wxALIGN_CENTER, 0);


		if (m_EnableImageCheckButton)
		{
			mafGUILab *lab = new mafGUILab(m_Dialog, -1, "  ");
			buttonBoxSizer->Add(lab, 0, wxALIGN_CENTER, 0);

			m_CheckBtn = new wxCheckBox(m_Dialog, ID_IMAGE_SELECT, "Select", wxPoint(-1, -1));
			m_CheckBtn->SetValidator(mafGUIValidator(this, ID_IMAGE_SELECT, m_CheckBtn, &m_ImageCheck));
			buttonBoxSizer->Add(m_CheckBtn, 0, wxALIGN_CENTER, 0);
		}

		m_Dialog->Add(m_RwiSizer, 0, wxALL);
		m_Dialog->Add(buttonBoxSizer, 0, wxALL, 5);
		m_Dialog->Fit();

		// Show dialog
		wxSize s = mafGetFrame()->GetSize();
		wxPoint p = mafGetFrame()->GetPosition();
		int posX = p.x + s.GetWidth() * .5 - m_Dialog->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - m_Dialog->GetSize().GetHeight() * .5;

		m_Dialog->SetPosition(wxPoint(posX, posY));
	}

	if (m_CheckBtn)
		m_CheckBtn->SetValue(m_ImageCheck);

		UpdateSelectionDialog(selection);

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
	m_ImagesList.clear();

	for (int i = 0; i < nImages; i++)
	{
		mafVMEImage *image = (mafVMEImage*)m_ImagesGroup->GetChild(i);
		m_ImagesList.push_back(image->GetName());
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

	if (nImages == 0)
	{
		m_PrevBtn->Enable(false);
		m_NextBtn->Enable(false);
		return;
	}

	//if (m_ImageSelection == selection) return;
	if (selection > nImages) return;

	m_ImageSelection = selection;
	wxString imageName = m_ImagesList[m_ImageSelection];

	//////////////////////////////////////////////////////////////////////////
	if (m_Dialog)
	{
		char tmp[10];
		sprintf(tmp, "[%d/%d]", selection + 1, nImages);

		wxString title = m_TitleDialog + " - " + imageName + " " + tmp;
		m_Dialog->SetTitle(title);

		// Load and show the image		
		mafVMEImage*vmeImage = mafVMEImage::SafeDownCast(m_ImagesGroup->FindInTreeByName(imageName));
		if (vmeImage)
		{
			m_Texture->SetInput((vtkImageData*)vmeImage->GetOutput()->GetVTKData());
			m_Texture->Modified();

			double b[6];
			vmeImage->GetOutput()->GetBounds(b);

			m_PlaneSource->SetOrigin(0, 0, 0);
			m_PlaneSource->SetPoint1(b[1], 0, 0);
			m_PlaneSource->SetPoint2(0, b[3], 0);

			m_Rwi->CameraReset(vmeImage);
			m_Rwi->CameraUpdate();

			m_Rwi->Show(true);
		}

		if (m_CheckBtn)
			m_CheckBtn->SetValue(m_ImageCheck);

		m_Dialog->Fit();
		m_Dialog->Update();
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

		if (m_ImagesGroup->GetNumberOfChildren() == 0)
		{
			m_Rwi->Show(false);
			m_Dialog->SetTitle(m_TitleDialog);
		}
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

