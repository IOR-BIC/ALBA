/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorPERScalarInformation.h,v $
Language:  C++
Date:      $Date: 2012-04-06 08:39:52 $
Version:   $Revision: 1.1.2.3 $
Authors:   Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medInteractorPERScalarInformation_h
#define __medInteractorPERScalarInformation_h

#include "medInteractionDefines.h"
#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class MED_INTERACTION_EXPORT medInteractorPERScalarInformation : public mafInteractorPER
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
