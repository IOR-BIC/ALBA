/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaAvatar2D
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaAvatar2D_h
#define __albaAvatar2D_h

#include "albaInteractor.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaDeviceButtonsPadMouse;

/**
  Avatars are entities moving in the virtual world according to user's 
  movements, and thus representing their presence in the virtual world.
  Currently Avatar2D are empty classes used only to store the renderer.
 */
class ALBA_EXPORT albaAvatar2D : public albaInteractor
{
public:
  albaTypeMacro(albaAvatar2D,albaInteractor);

  /**  Process events coming from tracker */
  virtual void OnEvent(albaEventBase *event);

  /** return the Mouse device associated to this avatar */
  albaDeviceButtonsPadMouse *GetMouse();

  /** set the mouse to be used for this avatar2D */
  void SetMouse(albaDeviceButtonsPadMouse *mouse);

protected:
  albaAvatar2D();
  virtual ~albaAvatar2D();

private:
  albaAvatar2D(const albaAvatar2D&) {}  // Not implemented.
  void operator=(const albaAvatar2D&) {}  // Not implemented.

  /**
  Test friend */
  friend class albaAvatar2DTest;

};

#endif 
