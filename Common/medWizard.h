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

#ifndef __medWizard_H__
#define __medWizard_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "medWizard.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

class medWizardBlock;


/**
  Class Name: medVect3d.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medWizard
{
public:

  /** Default constructor */
  medWizard();

  /** Default destructor */
  ~medWizard();

  /** Execute the wizard */
  void Execute();

  /** Add a new Block to the wizard */
  void AddBlock(medWizardBlock *block);
  
private:
  
  std::vector <medWizardBlock *> m_Blocks;
  
};
#endif
