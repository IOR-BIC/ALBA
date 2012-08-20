/*=========================================================================

 Program: MAF2Medical
 Module: medLogicWithManagers
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medLogicWithManagers_H__
#define __medLogicWithManagers_H__

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "medCommonDefines.h"
#include "mafLogicWithManagers.h"

//----------------------------------------------------------------------------
// forward reference
//----------------------------------------------------------------------------

/**
  Class Name: medLogicWithManagers.
  Class for handle the high level logic of a medical application.
*/
class MED_COMMON_EXPORT medLogicWithManagers: public mafLogicWithManagers
{
public:
  /** constructor. */
	medLogicWithManagers();
  /** destructor. */
	virtual     ~medLogicWithManagers(); 

  /** Process events coming from other objects */ 
	virtual void OnEvent(mafEventBase *maf_event);

protected:
  /** Show contextual menu for views when right mouse click arrives.*/
  void ViewContextualMenu(bool vme_menu);

};
#endif
