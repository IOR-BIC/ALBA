/*=========================================================================

 Program: MAF2Medical
 Module: medWizardBlockInformation
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
class MED_COMMON_EXPORT medWizardBlockInformation : public medWizardBlock,mafObserver
{
public:

  enum WIZARD_INFO
  {
    WIZARD_INFO_SHOW_ID = MINID,
  };

  /** Default constructor */
  medWizardBlockInformation(const char *name);

  /** Default destructor */
  ~medWizardBlockInformation();
    
  /** Set The title of the selection window */
  void SetWindowTitle(const char *Title);

  /** Set The title of the selection window */
  void SetDescription(const char *description);

  /** Set name of the window image
      if no image is setted no image will be showed
      The image will be stored in <APP_DIR>\WizardImages\*/
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

  /** Event management */
  virtual void OnEvent(mafEventBase *maf_event);
  
private:

  bool m_HorizontalImage;
  int m_ShowBoxes;
  wxString m_Title;
  wxString m_Description;
  wxString m_BoxLabel;
  wxString m_ImageFilename;
};
#endif
