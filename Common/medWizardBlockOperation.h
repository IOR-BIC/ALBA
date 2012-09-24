/*=========================================================================

 Program: MAF2Medical
 Module: medWizardOperaiontionBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medWizardBlockOperation_H__
#define __medWizardBlockOperation_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "medWizardBlock.h"


//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medWizardOperaiontionBlock.
  Class for operation block inside the wizard 
  This classes manage all the operation required to run an operation
  *Open the view Required by the op (if necesary)
  *Selects the input VME
  *Show other useful VME
  *Run the operation
  *Hide some VME after operation termination
*/
class MED_COMMON_EXPORT medWizardBlockOperation : public medWizardBlock
{
public:
  
  /** Default constructor */
  medWizardBlockOperation(const char *name);

  /** Default destructor */
  ~medWizardBlockOperation();

  /** Set The name of the operation required view */
  void SetRequiredView(const char *View);

  /** Set The name of the operation required view */
  wxString GetRequiredView();

  /** Set the path for VME selection */
  void VmeSelect(const char *path);

  /** Set the path of the VMEs that is need to show for the operation.
      The path starts from the selected vme. 
      Multiple calls to this funcion correspond on multiple VME show.*/
  void VmeShowAdd(const char *path);

  /** Set the path of the VMEs that is need to hide after the operation.
      The path starts from the selected vme.
      Multiple calls to this funcion correspond on multiple VME show*/
  void VmeHideAdd(const char *path);


  /** Execute the block */
  void Execute();

  /** Starts the execution of the block */
  void ExcutionBegin();
  
  /** Ends the execution of the block */
  void ExcutionEnd();

   /** Returns the name of the operation required by this block 
       Return an empty string if no operation is required
       There are three wizard specificic operation:
       SAVE Save the msf
       SAVE_AS Save the msf with name
       OPEN an msf
       PAUSE open an operation with only one "next step" button*/
   void SetRequiredOperation(const char *name);

   /** Returns the name of the operation required by this block 
       Return an empty string if no operation is required */
   wxString GetRequiredOperation();

   /** If Set  Auto Show Selected VME to false the selected vme will not 
       auto showed before operation start*/
   void SetAutoShowSelectedVME(bool autoShow);

   /** Get Auto Show Selected VME status */
   bool GetAutoShowSelectedVME();
  
private:

  wxString m_Operation;
  wxString m_RequiredView;
  wxString m_VmeSelect;
  bool m_AutoShowSelectedVME;
  std::vector < wxString > m_VmeShow;
  std::vector < wxString > m_VmeHide;
};
#endif
