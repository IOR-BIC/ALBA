/*=========================================================================

 Program: MAF2
 Module: mafInteractorPERScalarInformation
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mafInteractorPERScalarInformation_h
#define __mafInteractorPERScalarInformation_h

#include "mafDefines.h"
#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class MAF_EXPORT mafInteractorPERScalarInformation : public mafInteractorPER
{
public: 
  mafTypeMacro(mafInteractorPERScalarInformation,mafInteractorPER);

   /** Function that handles events sent from other objects. */
  virtual void OnEvent(mafEventBase *event);

protected:
  /** constructor. */
  mafInteractorPERScalarInformation();
  /** destructor. */
  virtual ~mafInteractorPERScalarInformation();

};
#endif 
