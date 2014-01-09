/*=========================================================================

 Program: MAF2Medical
 Module: medWizardSelectionBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
#include "medWizardBlockInformation.h"
#include "mafGUIDialog.h"
#include "mafGUI.h"
#include <wx/image.h>
#include <wx/statline.h>
#include "mafGUIPicButton.h"


#define max(a,b)  (((a) > (b)) ? (a) : (b))



//----------------------------------------------------------------------------
medWizardBlockInformation::medWizardBlockInformation(const char *name):medWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting image on top by default
  m_HorizontalImage=true;
  m_ImagesPath=(mafGetApplicationDirectory() + "\\WizardImages\\").c_str();
}

//----------------------------------------------------------------------------
medWizardBlockInformation::~medWizardBlockInformation()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medWizardBlockInformation::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}


//----------------------------------------------------------------------------
void medWizardBlockInformation::SetDescription( const char *description )
//----------------------------------------------------------------------------
{
  //set the description showed to the user
  m_Description=description;
}



//----------------------------------------------------------------------------
void medWizardBlockInformation::ExcutionBegin()
//----------------------------------------------------------------------------
{
  medWizardBlock::ExcutionBegin();

  //Create event for communication
  mafEvent e=mafEvent(this,WIZARD_INFORMATION_BOX_SHOW_GET);

  //Ask upward to the wizard manager if information box shows is enabled
  mafEventMacro(e);

  //read value
  m_ShowBoxes=e.GetBool();
 
  //Show information box only if is enabled in options
  if (m_ShowBoxes)
  {
    medWizardBlock::ExcutionBegin();
    mafGUIPicButton *previewImageButton;

    wxBoxSizer *mainVertSizer = new wxBoxSizer(wxVERTICAL);
    mafGUIDialog *dialog=new mafGUIDialog(m_Title,mafRESIZABLE);
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
      previewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY );

      previewBitmap=new wxBitmap(*previewImage);
      previewImageButton=new mafGUIPicButton(dialog,previewBitmap,-1);
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
    
    const char *charPointer=m_Description.c_str();
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
      xsize=max(xsize,currentX);
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
    mafGUI *checkGUI=new mafGUI(this);
    checkGUI->Bool(WIZARD_INFO_SHOW_ID,"Show information boxes",&m_ShowBoxes,true);
    checkGUI->Reparent(dialog);
    buttonSizer->Add(checkGUI,0,wxLEFT,5);

    wxButton *okButton = new wxButton(dialog,wxID_OK,"Ok");
    buttonSizer->Add(okButton,0,wxALIGN_CENTER,5);
    

    mainVertSizer->Add(buttonSizer,0,wxALL,5);
    dialog->Add(mainVertSizer,0,wxALL);
	
	dialog->Fit();
    //show dialog
	wxSize s = mafGetFrame()->GetSize();
	wxPoint p = mafGetFrame()->GetPosition();
	int posX = p.x + s.GetWidth() * .5 - dialog->GetSize().GetWidth() * .5 ;
	int posY = p.y + s.GetHeight() * .5 - dialog->GetSize().GetHeight() * .5;
	dialog->SetPosition(wxPoint(posX, posY));
    dialog->ShowModal();
  }
}

//----------------------------------------------------------------------------
void medWizardBlockInformation::SetImage( const char *imageFilename )
//----------------------------------------------------------------------------
{
  //
  m_ImageFilename=imageFilename;
}


//----------------------------------------------------------------------------
void medWizardBlockInformation::SetBoxLabel( const char *label )
//----------------------------------------------------------------------------
{
  //Set the label of the box
  m_BoxLabel=label;
}

//----------------------------------------------------------------------------
void medWizardBlockInformation::SetImagePositionToTop()
//----------------------------------------------------------------------------
{
  m_HorizontalImage=true;
}

//----------------------------------------------------------------------------
void medWizardBlockInformation::SetImagePositionToLeft()
//----------------------------------------------------------------------------
{
  m_HorizontalImage=false;
}


//----------------------------------------------------------------------------
void medWizardBlockInformation::OnEvent( mafEventBase *maf_event )
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case WIZARD_INFO_SHOW_ID:
      {
        //Forward up event to update persistent settings
        mafEventMacro(mafEvent(this,WIZARD_INFORMATION_BOX_SHOW_SET,(bool)m_ShowBoxes));
      }
      break;
    default:
      //All event will be forwarded up
      mafEventMacro(*e);
      break;
    } // end switch case
  }
}


