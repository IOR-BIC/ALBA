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

  /** Return the name of the Block witch will be executed after this */
  virtual wxString GetNextBlock();

  /** Execute the block */
  virtual void Execute();

  /** Abort the execution of the block */
  virtual void Abort();

  /** Return true if the user has aborted the operation */
  int isAborted();

  /** Manage Events */
  virtual void OnEvent(mafEventBase *maf_event);



private:
    
  wxString m_Name;
  wxString m_BlockType;
  int m_Aborted;

};
#endif
