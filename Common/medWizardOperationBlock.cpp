/*=========================================================================

 Program: MAF2Medical
 Module: medVect3d
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
#include "medWizardOperationBlock.h"
#include <math.h>

//----------------------------------------------------------------------------
mafCxxTypeMacro(medWizardOperaiontionBlock);
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
medWizardOperaiontionBlock::medWizardOperaiontionBlock()
//----------------------------------------------------------------------------
{

}

//----------------------------------------------------------------------------
medWizardOperaiontionBlock::~medWizardOperaiontionBlock()
//----------------------------------------------------------------------------
{
  m_VmeShow.clear();
  m_VmeHide.clear();
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::RequiredView( char *View )
//----------------------------------------------------------------------------
{
  m_RequiredView = View;
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeSelect( char *path )
//----------------------------------------------------------------------------
{
  m_VmeSelect=path;
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeShow( char *path )
//----------------------------------------------------------------------------
{
  wxString wxPath;
  wxPath=path;
  m_VmeShow.push_back(wxPath);
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::VmeHide( char *path )
//----------------------------------------------------------------------------
{
  wxString wxPath;
  wxPath=path;
  m_VmeHide.push_back(wxPath);
}

//----------------------------------------------------------------------------
void medWizardOperaiontionBlock::SetNextBlock( char *block )
//----------------------------------------------------------------------------
{
  m_NextBlock=block;
}

//----------------------------------------------------------------------------
wxString medWizardOperaiontionBlock::GetNextBlock()
//----------------------------------------------------------------------------
{
  return m_NextBlock;
}

void medWizardOperaiontionBlock::Execute()
{
  //Ask Wizard for View

  //Select the input VME for the operation

  //Show the required VMEs

  //Run Operation

  //Hide the required VMEs
 
}

void medWizardOperaiontionBlock::Abort()
{
    //Abort The Operation
}
