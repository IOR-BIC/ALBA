/*=========================================================================

 Program: MAF2Medical
 Module: mmiInfoImage
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __mmiInfoImage_h
#define __mmiInfoImage_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafInteractorPER.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------

/** Class implementing image info reporting on status bar when not interacting.
*/
class MAF_EXPORT mmiInfoImage : public mafInteractorPER
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
