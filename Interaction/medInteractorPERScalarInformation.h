/*=========================================================================

 Program: MAF2
 Module: medInteractorPERScalarInformation
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __medInteractorPERScalarInformation_h
#define __medInteractorPERScalarInformation_h

#include "mafDefines.h"
#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class MAF_EXPORT medInteractorPERScalarInformation : public mafInteractorPER
{
public: 
  mafTypeMacro(medInteractorPERScalarInformation,mafInteractorPER);

   /** Function that handles events sent from other objects. */
  virtual void OnEvent(mafEventBase *event);

protected:
  /** constructor. */
  medInteractorPERScalarInformation();
  /** destructor. */
  virtual ~medInteractorPERScalarInformation();

};
#endif 
