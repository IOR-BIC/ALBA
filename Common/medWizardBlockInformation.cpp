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
#include "mafGUIPicButton.h"


#define max(a,b)  (((a) > (b)) ? (a) : (b))



//----------------------------------------------------------------------------
medWizardBlockInformation::medWizardBlockInformation(const char *name):medWizardBlock(name)
//----------------------------------------------------------------------------
{
  //setting image on top by default
  m_HorizontalImage=true;
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
  mafGUIPicButton *previewImageButton;

  mafGUIDialog *dialog=new mafGUIDialog(m_Title,mafOK|mafRESIZABLE);
  wxBoxSizer * mainSizer;
  
  //Setting the frames order base
  if (m_HorizontalImage)
    mainSizer = new wxBoxSizer(wxVERTICAL);
  else
    mainSizer = new wxBoxSizer(wxHORIZONTAL);
  
  //check if there is an image to show
  if (m_ImageFilename != "")
  {
    wxImage *previewImage;
    wxBitmap *previewBitmap;
    
    //load and show the image
    previewImage=new wxImage();
    wxString imgPath=(mafGetApplicationDirectory() + "\\WizardImages\\").c_str();
    imgPath=imgPath+m_ImageFilename;
    previewImage->LoadFile(imgPath.c_str(), wxBITMAP_TYPE_ANY );

    previewBitmap=new wxBitmap(*previewImage);
    previewImageButton=new mafGUIPicButton(dialog,previewBitmap,-1);
    mainSizer->Add(previewImageButton, 0,wxALL | wxALIGN_CENTER, 5);
    delete previewBitmap;
  }
  
  
  //Creating the static text area
  wxStaticText* guiLabel = new wxStaticText(dialog, -1, m_Description);
  wxFont fixedFont= guiLabel->GetFont();

  //setting font to fixed size to avoid wx non-sense 
  fixedFont.SetFamily(wxFONTFAMILY_MODERN);
  fixedFont.SetFaceName("Courier");
  guiLabel->SetFont(fixedFont);

  const char *charPointer=m_Description.c_str();
  int xsize=0,ysize=0,currentX;
  int x,y;
  
  x=guiLabel->GetCharWidth();
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
  mainSizer->Add(modelSizer,0,wxALL|wxEXPAND,5);
  
  dialog->Add(mainSizer,0,wxALL|wxEXPAND, 15);
  
  //show dialog
  dialog->ShowModal();
}

//----------------------------------------------------------------------------
void medWizardBlockInformation::SetImage( const char *imageFilename )
//----------------------------------------------------------------------------
{
  m_ImageFilename=imageFilename;
}

//----------------------------------------------------------------------------
void medWizardBlockInformation::SetNextBlock( const char *block )
//----------------------------------------------------------------------------
{
  //set the name of the block called after block execution
  m_NextBlock=block;
}

void medWizardBlockInformation::SetBoxLabel( const char *label )
{
  m_BoxLabel=label;
}

void medWizardBlockInformation::SetImagePositionToTop()
{
  m_HorizontalImage=true;
}

void medWizardBlockInformation::SetImagePositionToLeft()
{
  m_HorizontalImage=false;
}




