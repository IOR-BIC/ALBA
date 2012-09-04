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

#ifndef __medWizardOperaiontionBlock_H__
#define __medWizardOperaiontionBlock_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "medWizardBlock.h"


//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medVect3d.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medWizardOperaiontionBlock : public medWizardBlock
{
public:
  
  /** Default constructor */
  medWizardOperaiontionBlock(const char *name);

  /** Default destructor */
  ~medWizardOperaiontionBlock();

  /** Set The name of the operation required view */
  void SetRequiredView(const char *View);

  /** Set The name of the operation required view */
  wxString GetRequiredView();

  /** Set the path for VME selection */
  void VmeSelect(const char *path);

  /** Set the path of the VMEs that is need to show for the operation.
      The path starts from the selected vme. 
      Multiple calls to this funcion correspond on multiple VME show.*/
  void VmeShow(const char *path);

  /** Set the path of the VMEs that is need to hide after the operation.
      The path starts from the selected vme.
      Multiple calls to this funcion correspond on multiple VME show*/
  void VmeHide(const char *path);

  /** Set name of the Block called after operation. */
  void SetNextBlock(const char *block);

  /** Return the name of the Block witch will be executed after this */
  wxString GetNextBlock();

  /** Execute the block */
  void Execute();

  /** Abort the execution of the block */
  void Abort();

  /** Starts the execution of the block */
  void ExcutionBegin();
  
  /** Ends the execution of the block */
  void ExcutionEnd();

   /** Returns the name of the operation required by this block 
       Return an empty string if no operation is required */
   void SetRequiredOperation(const char *name);

   /** Returns the name of the operation required by this block 
       Return an empty string if no operation is required */
   wxString GetRequiredOperation();

  
private:

  wxString m_Operation;
  wxString m_RequiredView;
  wxString m_VmeSelect;
  std::vector < wxString > m_VmeShow;
  std::vector < wxString > m_VmeHide;
  wxString m_NextBlock;

};
#endif
