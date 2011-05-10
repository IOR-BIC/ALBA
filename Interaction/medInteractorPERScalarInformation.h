/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medInteractorPERScalarInformation.h,v $
Language:  C++
Date:      $Date: 2011-05-10 15:08:19 $
Version:   $Revision: 1.1.2.1 $
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

  virtual void OnEvent(mafEventBase *event);

protected:
  medInteractorPERScalarInformation();
  virtual ~medInteractorPERScalarInformation();

private:
  medInteractorPERScalarInformation(const medInteractorPERScalarInformation&);  // Not implemented.
  void operator=(const medInteractorPERScalarInformation&);  // Not implemented.
};
#endif 
