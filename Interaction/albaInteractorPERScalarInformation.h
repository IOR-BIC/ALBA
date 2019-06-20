/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorPERScalarInformation
 Authors: Matteo Giacomoni, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaInteractorPERScalarInformation_h
#define __albaInteractorPERScalarInformation_h

#include "albaDefines.h"
#include "albaInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class ALBA_EXPORT albaInteractorPERScalarInformation : public albaInteractorPER
{
public: 
  albaTypeMacro(albaInteractorPERScalarInformation,albaInteractorPER);

   /** Function that handles events sent from other objects. */
  virtual void OnEvent(albaEventBase *event);

protected:
  /** constructor. */
  albaInteractorPERScalarInformation();
  /** destructor. */
  virtual ~albaInteractorPERScalarInformation();

};
#endif 
