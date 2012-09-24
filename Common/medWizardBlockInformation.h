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

#ifndef __medWizardBlockInformation_H__
#define __medWizardBlockInformation_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "medWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medWizardSelectionBlock.
  Class for create a multiple choice switch inside wizard graph
*/
class MED_COMMON_EXPORT medWizardBlockInformation : public medWizardBlock
{
public:

  /** Default constructor */
  medWizardBlockInformation(const char *name);

  /** Default destructor */
  ~medWizardBlockInformation();
    
  /** Set The title of the selection window */
  void SetWindowTitle(const char *Title);

  /** Set The title of the selection window */
  void SetDescription(const char *description);

  /** Set name of the window image
      if no image is setted no image will be showed*/
  void SetImage(const char *imageFilename);

  /** Set name of the Block called after operation. */
  void SetNextBlock(const char *block);

  /** Set the description box Label*/
  void SetBoxLabel(const char *label);

  /** Set Image Position To Top */
  void SetImagePositionToTop();

  /** Set Image Position To Left */
  void SetImagePositionToLeft();


  /** Starts the execution of the block */
  virtual void ExcutionBegin();


protected:


private:

  bool m_HorizontalImage;
  wxString m_Title;
  wxString m_Description;
  wxString m_BoxLabel;
  wxString m_ImageFilename;
};
#endif
