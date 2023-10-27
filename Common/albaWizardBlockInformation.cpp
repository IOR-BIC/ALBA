/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardSelectionBlock
 Authors: Gianluigi Crimi
 
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
#include "albaWizardBlockInformation.h"
#include "albaGUIDialog.h"
#include "albaGUI.h"
#include <wx/image.h>
#include <wx/statline.h>
#include "albaGUIPicButton.h"



//----------------------------------------------------------------------------
albaWizardBlockInformation::albaWizardBlockInformation(const char *name):albaWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting image on top by default
  m_HorizontalImage=true;
  m_ImagesPath=(albaGetApplicationDirectory() + "\\WizardImages\\").char_str();
}

//----------------------------------------------------------------------------
albaWizardBlockInformation::~albaWizardBlockInformation()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaWizardBlockInformation::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}


//----------------------------------------------------------------------------
void albaWizardBlockInformation::SetDescription( const char *description )
//----------------------------------------------------------------------------
{
  //set the description showed to the user
  m_Description=description;
}



//----------------------------------------------------------------------------
void albaWizardBlockInformation::ExcutionBegin()
//----------------------------------------------------------------------------
{
  albaWizardBlock::ExcutionBegin();

  //Create event for communication
  albaEvent e=albaEvent(this,WIZARD_INFORMATION_BOX_SHOW_GET);

  //Ask upward to the wizard manager if information box shows is enabled
  albaEventMacro(e);

  //read value
  m_ShowBoxes=e.GetBool();
 
  //Show information box only if is enabled in options
  if (m_ShowBoxes)
  {
    albaWizardBlock::ExcutionBegin();
    albaGUIPicButton *previewImageButton;

    wxBoxSizer *mainVertSizer = new wxBoxSizer(wxVERTICAL);
    albaGUIDialog *dialog=new albaGUIDialog(m_Title,albaRESIZABLE);
	wxColour col(232, 249, 253);
	dialog->SetBackgroundColour(col);
    wxBoxSizer * mainInfoSizer;
  
    //Setting the frames order base
    if (m_HorizontalImage)
      mainInfoSizer = new wxBoxSizer(wxVERTICAL);
    else
      mainInfoSizer = new wxBoxSizer(wxHORIZONTAL);
  
    //check if there is an image to show
    if (m_ImageFilename != "")
    {
      wxImage *previewImage;
      wxBitmap *previewBitmap;
    
      //load and show the image
      previewImage=new wxImage();
      wxString imgPath=m_ImagesPath;
      imgPath=imgPath+m_ImageFilename;
      previewImage->LoadFile(imgPath.char_str(), wxBITMAP_TYPE_ANY );

      previewBitmap=new wxBitmap(*previewImage);
      previewImageButton=new albaGUIPicButton(dialog,previewBitmap,-1);
      mainInfoSizer->Add(previewImageButton, 0,wxALL | wxALIGN_CENTER, 5);
      delete previewBitmap;
    }
  
  
    //Creating the static text area
    wxStaticText* guiLabel = new wxStaticText(dialog, -1, m_Description,wxPoint(-1,-1),wxSize(-1,-1),wxALIGN_LEFT);
    wxFont fixedFont= guiLabel->GetFont();

    //setting font to fixed size to avoid wx non-sense 
    fixedFont.SetFamily(wxFONTFAMILY_DECORATIVE);
    //fixedFont.SetFaceName("Courier");
    guiLabel->SetFont(fixedFont);
    
    const char *charPointer=m_Description.char_str();
    int xsize=0,ysize=0,currentX;
    int x,y;
  

    x=guiLabel->GetCharWidth()-1;
    y=guiLabel->GetCharHeight();
    
    //Calculating space required by text area
    while (*charPointer!='\0')
    {
      currentX=0;
      while (*charPointer!='\n' && *charPointer!='\0')
      {
        charPointer++;
        currentX+=x;
      }
      xsize=MAX(xsize,currentX);
      ysize+=y;
      charPointer++;
    }

    //setting the min size for the area
    guiLabel->SetMinSize(wxSize(xsize,ysize));

  
    //merging sizers into dialog
    wxStaticBoxSizer * modelSizer = new wxStaticBoxSizer(wxVERTICAL,dialog,m_BoxLabel);
    modelSizer->Add(guiLabel,0,wxALL|wxEXPAND,5);
    mainInfoSizer->Add(modelSizer,0,wxALL|wxEXPAND,5);
  
    mainVertSizer->Add(mainInfoSizer,0,wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 15);
    
    
    mainVertSizer->Add(new wxStaticLine(dialog,-1),0,wxEXPAND | wxALL, 7);
    

    wxBoxSizer *buttonSizer =  new wxBoxSizer( wxHORIZONTAL );

    //Creating button stuff to disable information box 
    //and ok button
    albaGUI *checkGUI=new albaGUI(this);
    checkGUI->Bool(WIZARD_INFO_SHOW_ID,"Show information boxes",&m_ShowBoxes,true);
    checkGUI->Reparent(dialog);
    buttonSizer->Add(checkGUI,0,wxLEFT,5);

    wxButton *okButton = new wxButton(dialog,wxID_OK,"Ok");
    buttonSizer->Add(okButton,0,wxALIGN_CENTER,5);
    

    mainVertSizer->Add(buttonSizer,0,wxALL,5);
    dialog->Add(mainVertSizer,0,wxALL);
	
	dialog->Fit();
    //show dialog
	wxSize s = albaGetFrame()->GetSize();
	wxPoint p = albaGetFrame()->GetPosition();
	int posX = p.x + s.GetWidth() * .5 - dialog->GetSize().GetWidth() * .5 ;
	int posY = p.y + s.GetHeight() * .5 - dialog->GetSize().GetHeight() * .5;
	dialog->SetPosition(wxPoint(posX, posY));
    dialog->ShowModal();
  }
}

//----------------------------------------------------------------------------
void albaWizardBlockInformation::SetImage( const char *imageFilename )
//----------------------------------------------------------------------------
{
  //
  m_ImageFilename=imageFilename;
}


//----------------------------------------------------------------------------
void albaWizardBlockInformation::SetBoxLabel( const char *label )
//----------------------------------------------------------------------------
{
  //Set the label of the box
  m_BoxLabel=label;
}

//----------------------------------------------------------------------------
void albaWizardBlockInformation::SetImagePositionToTop()
//----------------------------------------------------------------------------
{
  m_HorizontalImage=true;
}

//----------------------------------------------------------------------------
void albaWizardBlockInformation::SetImagePositionToLeft()
//----------------------------------------------------------------------------
{
  m_HorizontalImage=false;
}


//----------------------------------------------------------------------------
void albaWizardBlockInformation::OnEvent( albaEventBase *alba_event )
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case WIZARD_INFO_SHOW_ID:
      {
        //Forward up event to update persistent settings
        albaEventMacro(albaEvent(this,WIZARD_INFORMATION_BOX_SHOW_SET,(bool)m_ShowBoxes));
      }
      break;
    default:
      //All event will be forwarded up
      albaEventMacro(*e);
      break;
    } // end switch case
  }
}


