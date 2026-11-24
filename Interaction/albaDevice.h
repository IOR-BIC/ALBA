/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDevice
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaDevice_h
#define __albaDevice_h

#include "albaAgent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaView;
class vtkRenderer;

/** Base class of the input devices: a device sends its events on the MCH_INPUT channel
  to its observers (the PER for the mouse) and tells on which view and renderer it is interacting. */
class ALBA_EXPORT albaDevice : public albaAgent
{
public:
  albaTypeMacro(albaDevice,albaAgent);

  /** Return the view the device is interacting with, NULL if none */
  virtual albaView *GetView() {return NULL;}

  /** Return the renderer the device is interacting with, NULL if none */
  virtual vtkRenderer *GetRenderer() {return NULL;}

protected:
  albaDevice() {}
  virtual ~albaDevice() {}

private:
  albaDevice(const albaDevice&);  // Not implemented.
  void operator=(const albaDevice&);  // Not implemented.
};

#endif 