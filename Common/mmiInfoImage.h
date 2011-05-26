/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiInfoImage.h,v $
  Language:  C++
  Date:      $Date: 2011-05-26 07:51:33 $
  Version:   $Revision: 1.1.8.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mmiInfoImage_h
#define __mmiInfoImage_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class MED_EXPORT mmiInfoImage : public mafInteractorPER
{
public: 
  mafTypeMacro(mmiInfoImage,mafInteractorPER);

  virtual void OnEvent(mafEventBase *event);

protected:
  mmiInfoImage();
  virtual ~mmiInfoImage();

private:
  mmiInfoImage(const mmiInfoImage&);  // Not implemented.
  void operator=(const mmiInfoImage&);  // Not implemented.
};
#endif 
