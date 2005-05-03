/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafAvatar2D.h,v $
  Language:  C++
  Date:      $Date: 2005-05-03 05:58:10 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafAvatar2D_h
#define __mafAvatar2D_h

#include "mafInteractor.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mmdMouse;

/**
  Avatars are entities moving in the virtual world according to user's 
  movements, and thus representing their presence in the virtual world.
  Currently Avatar2D are empty classes used only to store the renderer...
 */
class mafAvatar2D : public mafInteractor
{
public:
  mafTypeMacro(mafAvatar2D,mafInteractor);

  /**  
    Hide the 3D cursor, i.e. remove actor from renderer's list. This is
    usually used to compute bounding box without considering avatars */
  //void Hide();

  /**  
    Show the 3D cursor. This function is conditional, in the 
    sense the old visibility state is retained when showing back. */
  //void Show();

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

  /** return the Mouse device associated to this avatar */
  mmdMouse *GetMouse();

  /** set the mouse to be used for this avatar2D */
  void SetMouse(mmdMouse *mouse);

protected:
  mafAvatar2D();
  virtual ~mafAvatar2D();

  /** redefined to add the Cursor actor into the selected renderer */
  //virtual int InternalInitialize();

  /** redefined to remove Cursor from renderer */
  //virtual void InternalShutdown();

private:
  mafAvatar2D(const mafAvatar2D&);  // Not implemented.
  void operator=(const mafAvatar2D&);  // Not implemented.
};

#endif 
