/*=========================================================================

 Program: MAF2
 Module: mafWizardBlockInformation
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafWizardBlockInformation_H__
#define __mafWizardBlockInformation_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: mafWizardSelectionBlock.
  Class for create a multiple choice switch inside wizard graph
*/
class MAF_EXPORT mafWizardBlockInformation : public mafWizardBlock,mafObserver
{
public:

  enum WIZARD_INFO
  {
    WIZARD_INFO_SHOW_ID = MINID,
  };

  /** Default constructor */
  mafWizardBlockInformation(const char *name);

  /** Default destructor */
  ~mafWizardBlockInformation();
    
  /** Set The title of the information window */
  void SetWindowTitle(const char *Title);

  /** Get The title of the information window */
  wxString GetWindowTitle() {return m_Title;};

  /** Set The description */
  void SetDescription(const char *description);

  /** Get The description */
  wxString GetDescription() {return m_Description;};

  /** Set name of the window image
      if no image is setted no image will be showed
      The image will be stored in <APP_DIR>\WizardImages\*/
  void SetImage(const char *imageFilename);

  /** return current image filename */
  wxString GetImage() {return m_ImageFilename;};
  
  /** Set the description box Label*/
  void SetBoxLabel(const char *label);

  /** Get the description box Label*/
  wxString GetBoxLabel(){return m_BoxLabel;};
    
  /** Set Image Position To Top */
  void SetImagePositionToTop();

  /** return true if image Position is set to top */
  bool GetImagePositionToTop() { return m_HorizontalImage;};

  /** Set Image Position To Left */
  void SetImagePositionToLeft();

  /** return true if image Position is set to left */
  bool GetImagePositionToLeft() { return !m_HorizontalImage;};

  /** Set the images path (default path is <ApplicationDirectory>\WizardImages\) */
  void SetImagePath(const char *path) {m_ImagesPath=path;};

  /** Get the images path*/
  wxString GetImagePath() { return m_ImagesPath;};

protected:

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  /** Event management */
  virtual void OnEvent(mafEventBase *maf_event);
  
private:

  bool m_HorizontalImage;
  int m_ShowBoxes;
  wxString m_Title;
  wxString m_Description;
  wxString m_BoxLabel;
  wxString m_ImageFilename;
  wxString m_ImagesPath;
};
#endif
