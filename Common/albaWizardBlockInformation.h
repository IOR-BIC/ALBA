/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaWizardBlockInformation
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaWizardBlockInformation_H__
#define __albaWizardBlockInformation_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaWizardBlock.h"
#include <vector>

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: albaWizardSelectionBlock.
  Class for create a multiple choice switch inside wizard graph
*/
class ALBA_EXPORT albaWizardBlockInformation : public albaWizardBlock,albaObserver
{
public:

  enum WIZARD_INFO
  {
    WIZARD_INFO_SHOW_ID = MINID,
  };

  /** Default constructor */
  albaWizardBlockInformation(const char *name);

  /** Default destructor */
  ~albaWizardBlockInformation();
    
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
  virtual void OnEvent(albaEventBase *alba_event);
  
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
