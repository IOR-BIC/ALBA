/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorPERScalarInformation.h,v $
Language:  C++
Date:      $Date: 2011-06-13 16:02:54 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni, Gianluigi Crimi
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __medInteractorPERScalarInformation_h
#define __medInteractorPERScalarInformation_h

#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class medInteractorPERScalarInformation : public mafInteractorPER
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
