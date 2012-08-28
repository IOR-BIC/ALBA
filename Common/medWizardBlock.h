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

#ifndef __medWizardBlock_H__
#define __medWizardBlock_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "mafEvent.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medVect3d.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medWizardBlock : public mafObject
{
public:

  /** Default constructor */
  medWizardBlock();

  /** Default destructor */
  ~medWizardBlock();

  /** Set the name of the block 
      "START","END", "ABORT" are reserved. */
  void SetName(char *name);

  /** Get the name of the block */
  wxString GetName();

  /** Return the name of the Block witch will be executed after this */
  virtual wxString GetNextBlock();

  /** Abort the execution of the block */
  virtual void Abort();

  /** Return true if the user has aborted the operation */
  int isAborted();

  /** Starts the execution of the block */
  virtual void ExcutionBegin();

  /** Ends the execution of the block */
  virtual void ExcutionEnd();

  /** Returns the name of the operation required by this block 
      Return an empty string if no operation is required */
  virtual wxString RequiredOperation();

private:
    
  wxString m_Name;
  wxString m_BlockType;
  int m_Aborted;
  int m_Running;
};
#endif
