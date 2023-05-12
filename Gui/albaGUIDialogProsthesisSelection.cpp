/*=========================================================================
Program:   AssemblerPro
Module:    albaGUIDialogProsthesisSelection.cpp
Language:  C++
Date:      $Date: 2021-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Nicola Vanella
==========================================================================
Copyright (c) BIC-IOR 2021 (https://github.com/IOR-BIC)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaGUIDialogProsthesisSelection.h"
#include "albaLogicWithManagers.h"

#include "albaDecl.h"
#include "albaGUIButton.h"
#include "albaGUIHyperLink.h"
#include "albaGUILab.h"
#include "albaGUIPicButton.h"
#include "albaGUIValidator.h"
#include "albaServiceClient.h"

#include "wx\image.h"
#include "wx\statline.h"
#include "wx\window.h"
//#include "vtkPNGReader.h"

enum PRODUCER_DIALOG_ID
{
	ID_PRODUCER_SELECTION = MINID,
	ID_PRODUCER_TEXT,
	ID_PRODUCER_IMM,
	ID_PRODUCER_LINK,
	ID_PROSTHESIS_SELECTION,
	ID_DIALOG_OK_PRESSED,
	ID_DIALOG_CANCEL_PRESSED,
};

//----------------------------------------------------------------------------
albaGUIDialogProsthesisSelection::albaGUIDialogProsthesisSelection(const wxString& title, long style)
	: albaGUIDialog(title, style)
{
	m_Gui = NULL;

	m_DBManager = GetLogicManager()->GetProsthesesDBManager();

	m_MainBoxSizer = NULL;

	m_IsChanged = false;

	m_ProducerImageSizer = NULL;
	m_ProducerImageButton = NULL;

	m_ProducerComboBox = NULL;
	m_ProsthesisComboBox = NULL;

	m_ProducerName_textCtrl = NULL;
	m_ProducerName_Link = NULL;
	m_ProducerImageFullName = "";

	m_CurrentProducer = NULL;
	m_CurrentProsthesis = NULL;

	m_ProducerName = "Producer";
	m_ProducerWebSite = "";
	m_ProducerImageName = "";
	m_ProducerImageFullName = "";

	m_ProsthesisName = "Prosthesis";
	m_ProsthesisImageName = "";
	m_ProsthesisImageFullName = "";

	CreateDialog();
}
//----------------------------------------------------------------------------
albaGUIDialogProsthesisSelection::~albaGUIDialogProsthesisSelection()
{
}

//----------------------------------------------------------------------------
void albaGUIDialogProsthesisSelection::OnEvent(albaEventBase *alba_event)
{
	switch (alba_event->GetId())
	{
	case ID_PRODUCER_SELECTION:
	{
		std::vector<albaProDBProducer *> DBproducers = m_DBManager->GetProducers();
		SetProducer(DBproducers[m_SelectedProducer]);
	}
	break;

	case ID_PROSTHESIS_SELECTION:
	{
		std::vector<albaProDBProsthesis *> DBprostheses = m_DBManager->GetProstheses();
		SetProsthesis(DBprostheses[m_SelectedProsthesis]);
	}
	break;

	case ID_DIALOG_OK_PRESSED:
	{
		m_IsChanged = true;
		this->Close();
	}
	break;

	case ID_DIALOG_CANCEL_PRESSED:
		m_IsChanged = false;
		this->Close();
		break;

	default:
		albaGUIDialog::OnEvent(alba_event);
	}
}

//----------------------------------------------------------------------------
void albaGUIDialogProsthesisSelection::Show()
{
	CreateDialog();
	ShowModal();
}

//----------------------------------------------------------------------------
void albaGUIDialogProsthesisSelection::CreateDialog()
{
	if (m_Gui == NULL)
	{
		m_Gui = new albaGUI(this);
		//////////////////////////////////////////////////////////////////////////

		m_MainBoxSizer = new wxBoxSizer(wxVERTICAL);

		//
		int panelWidth = 400;

		if (m_CurrentProducer)
		{
			m_ProducerName = m_CurrentProducer->GetName();
			m_ProducerWebSite = m_CurrentProducer->GetWebSite();
			m_ProducerImageName = m_CurrentProducer->GetImgFileName();

			wxString DBDir = m_DBManager->GetDBDir();
			m_ProducerImageFullName = DBDir + m_CurrentProducer->GetImgFileName();
		}

		wxString imagesPath = albaGetConfigDirectory().c_str();
		wxString imgPath = imagesPath + "/Wizard/Producer.bmp"; // Default

		if (wxFileExists(m_ProducerImageFullName))
		{
			imgPath = m_ProducerImageFullName;
		}


		// IMAGE Producer
		m_ProducerImageSizer = new wxBoxSizer(wxVERTICAL);

		wxImage *previewImage = new wxImage(400,140);
		if (wxFileExists(imgPath))
		{
			previewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY);
		}
	
		wxBitmap *previewBitmap = new wxBitmap(*previewImage);
		m_ProducerImageButton = new albaGUIPicButton(this, previewBitmap, -1);

		panelWidth = previewImage->GetWidth();

		m_ProducerImageSizer->Add(m_ProducerImageButton);

		m_MainBoxSizer->Add(m_ProducerImageSizer, 0, wxALL | wxALIGN_CENTER, 0);

		delete previewBitmap;
		delete previewImage;


		//////////////////////////////////////////////////////////////////////////
		// Info Sizer
		wxBoxSizer *infoBoxSizer = new wxBoxSizer(wxVERTICAL);

		wxStaticBoxSizer *producerBoxSizer = new wxStaticBoxSizer(wxHORIZONTAL, this, "Producer");

		if (m_CurrentProducer)
		{
			// TEXT - Producer Name
			m_ProducerName_textCtrl = new wxTextCtrl(this, ID_PRODUCER_TEXT, m_ProducerName, wxPoint(-1, -1), wxSize(300, 20), wxALL | wxEXPAND);
			m_ProducerName_textCtrl->SetEditable(false);
			m_ProducerName_textCtrl->Enable(false);
			m_ProducerName_textCtrl->SetMaxLength(64);
			producerBoxSizer->Add(m_ProducerName_textCtrl, 0, wxALL | wxEXPAND, 0);
			producerBoxSizer->Add(new albaGUILab(this, -1, "   "));

			// LINK - Producer Web Site
			m_ProducerName_Link = new albaGUIHyperLink(this, NULL, m_ProducerWebSite);
			m_ProducerName_Link->SetUrl(m_ProducerWebSite);
			producerBoxSizer->Add(m_ProducerName_Link, 0, wxALL | wxEXPAND, 0);
		}
		else
		{
			// COMBO - Producer Name
			m_ProducerComboBox = new wxComboBox(this, ID_PRODUCER_SELECTION, "", wxPoint(-1, -1), wxSize(panelWidth, 20));
			m_ProducerComboBox->SetEditable(false);
			m_ProducerComboBox->SetValidator(albaGUIValidator(this, ID_PRODUCER_SELECTION, m_ProducerComboBox, &m_SelectedProducer));
			producerBoxSizer->Add(m_ProducerComboBox, 0, wxALL | wxEXPAND, 0);
		}

		infoBoxSizer->Add(producerBoxSizer, 0, wxALL | wxEXPAND, 5);

		//////////////////////////////////////////////////////////////////////////
		m_ProsthesisBoxSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Prosthesis");

		// COMBO - Prosthesis
		m_ProsthesisComboBox = new wxComboBox(this, ID_PROSTHESIS_SELECTION, "", wxPoint(-1, -1), wxSize(panelWidth, 20));
		m_ProsthesisComboBox->SetValidator(albaGUIValidator(this, ID_PROSTHESIS_SELECTION, m_ProsthesisComboBox, &m_SelectedProsthesis));
		m_ProsthesisBoxSizer->Add(m_ProsthesisComboBox, 0, wxALL | wxEXPAND, 0);

		infoBoxSizer->Add(m_ProsthesisBoxSizer, 0, wxALL | wxEXPAND, 5);

		//////////////////////////////////////////////////////////////////////////
		// TEXT - Empty Separator
		infoBoxSizer->Add(new albaGUILab(this, -1, " "), 0, wxALIGN_LEFT, 5);

		m_MainBoxSizer->Add(infoBoxSizer, 0, wxALL, 5);

		// LINE
		m_MainBoxSizer->Add(new wxStaticLine(this, -1, wxPoint(-1, -1), wxSize(430, 1)), 0, wxALL, 5);

		// Buttons
		wxBoxSizer *btnBoxSizer = new wxBoxSizer(wxHORIZONTAL);

		// BUTTON - Cancel
		albaGUIButton *cancelBtn = new albaGUIButton(this, ID_DIALOG_CANCEL_PRESSED, "Cancel", wxPoint(-1, -1));
		cancelBtn->SetListener(this);
		btnBoxSizer->Add(cancelBtn, 0, wxALIGN_RIGHT, 15);

		// BUTTON - Ok
		m_OkBtn = new albaGUIButton(this, ID_DIALOG_OK_PRESSED, "OK", wxPoint(-1, -1));
		m_OkBtn->SetListener(this);
		btnBoxSizer->Add(m_OkBtn, 0, wxALIGN_RIGHT, 15);

		m_MainBoxSizer->Add(btnBoxSizer, 0, wxALL | wxALIGN_RIGHT, 5);

		//////////////////////////////////////////////////////////////////////////

		m_Gui->Fit();

		// Show dialog
		wxSize s = albaGetFrame()->GetSize();
		wxPoint p = albaGetFrame()->GetPosition();

		SetSize(430, 355);

		int posX = p.x + s.GetWidth() * .5 - this->GetSize().GetWidth() * .5;
		int posY = p.y + s.GetHeight() * .5 - this->GetSize().GetHeight() * .5;

		this->SetPosition(wxPoint(posX, posY));

		Add(m_Gui, 1);
		Add(m_MainBoxSizer, 0);
	}

	UpdateDialog();
}

//----------------------------------------------------------------------------
void albaGUIDialogProsthesisSelection::UpdateDialog()
{
	if (m_Gui)
	{
		if (m_ProducerName_textCtrl)
			m_ProducerName_textCtrl->SetValue(m_ProducerName);

		if (m_ProducerName_Link)
		{
			m_ProducerName_Link->SetText(m_ProducerWebSite);
			m_ProducerName_Link->SetUrl(m_ProducerWebSite);
		}

		// Update Producer Image
		if (wxFileExists(m_ProducerImageFullName))
		{
			if (m_ProducerImageButton != NULL)
			{
				m_ProducerImageSizer->Remove(m_ProducerImageButton);
				delete m_ProducerImageButton;
			}

			// Load and show the image
			wxImage *previewImage = new wxImage();

			wxString path, name, ext;
			wxFileName::SplitPath(m_ProducerImageFullName, &path, &name, &ext);
						
			if (ext == "png") // NOT USED
			{
				// Mode 1 - Load png 
// 				wxPNGHandler *pngHandler = new wxPNGHandler();
// 				pngHandler->SetName("PNGHANDLER");
// 				wxImage::AddHandler(pngHandler);
// 				
// 				//previewImage->InsertHandler(pngHandler);
// 				previewImage->LoadFile(m_ProducerImageFullName, wxBITMAP_TYPE_PNG);
// 
// 				wxImage::RemoveHandler("PNGHANDLER");

				// Mode 2 - Load png 
// 				vtkPNGReader *imageReader;
// 				vtkNEW(imageReader);
// 				imageReader->SetFileName(m_ProducerImageFullName);
// 				imageReader->Update();
// 
// 				vtkImageData* imageData = imageReader->GetOutput();
			}
			else /*if (ext == "bmp")*/
			{
				previewImage->LoadFile(m_ProducerImageFullName, wxBITMAP_TYPE_ANY);
			}

			wxBitmap *previewBitmap;
			previewBitmap = new wxBitmap(previewImage);

			m_ProducerImageButton = new albaGUIPicButton(this, previewBitmap, -1);
			m_ProducerImageSizer->Add(m_ProducerImageButton, 0, wxALL | wxALIGN_CENTER, 0);
			
			m_ProducerImageButton->Fit();
			m_ProducerImageSizer->Fit(this);

			delete previewBitmap;
			delete previewImage;
		}

		m_MainBoxSizer->Fit(this);
		m_Gui->Fit();

		m_OkBtn->Enable(m_CurrentProsthesis != NULL);

		m_Gui->Update();
	}
}

//----------------------------------------------------------------------------
void albaGUIDialogProsthesisSelection::SetProducer(albaString producerName)
{
	std::vector<albaProDBProducer *> DBproducers = m_DBManager->GetProducers();
	
	for (int p = 0; p < DBproducers.size(); p++)
	{
		if (DBproducers[p]->GetName() == producerName)
		{
			SetProducer(DBproducers[p]);
			break;
		}
	}
}
//----------------------------------------------------------------------------
void albaGUIDialogProsthesisSelection::SetProducer(albaProDBProducer *producer)
{
	if (producer)
	{
		wxString DBDir = m_DBManager->GetDBDir();
		
		m_CurrentProducer = producer;
		m_CurrentProsthesis = NULL;

		m_ProducerName = m_CurrentProducer->GetName();
		m_ProducerWebSite = m_CurrentProducer->GetWebSite();
		m_ProducerImageName = m_CurrentProducer->GetImgFileName();
		m_ProducerImageFullName = DBDir + m_CurrentProducer->GetImgFileName();

		// Load Producers Info
		std::vector<albaProDBProducer *> DBproducers = m_DBManager->GetProducers();

		m_ProducerComboBox->Clear();
		m_SelectedProducer = 0;

		for (int p = 0; p < DBproducers.size(); p++)
		{
			m_ProducerComboBox->Append(DBproducers[p]->GetName().GetCStr());

			if (DBproducers[p]->GetName() == m_CurrentProducer->GetName())
			{
				m_SelectedProducer = p;
			}
		}

		m_ProducerComboBox->Select(m_SelectedProducer);

		// Load Prosthesis Info
		std::vector<albaProDBProsthesis *> DBprostheses = m_DBManager->GetProstheses();
		m_ProsthesisComboBox->Clear();
		m_SelectedProsthesis = 0;

		int items = 0;

		for (int p = 0; p < DBprostheses.size(); p++)
		{
			if (DBprostheses[p]->GetProducer() == m_CurrentProducer->GetName())
			{
				m_ProsthesisComboBox->Append(DBprostheses[p]->GetName().GetCStr());
				items++;
			}
		}
		
		if (items > 0)
		{
			m_SelectedProsthesis = 0;
			m_CurrentProsthesis = DBprostheses[0];
			m_ProsthesisComboBox->Select(m_SelectedProsthesis);
		}
	}

	UpdateDialog();
}
//----------------------------------------------------------------------------
void albaGUIDialogProsthesisSelection::SetProsthesis(albaProDBProsthesis *prosthesis)
{
	m_CurrentProsthesis = prosthesis;

	UpdateDialog();
}