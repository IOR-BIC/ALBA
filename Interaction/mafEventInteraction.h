/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventInteraction.h,v $
  Language:  C++
  Date:      $Date: 2005-06-21 07:57:08 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafEventInteraction_h
#define __mafEventInteraction_h

#include "mafEventBase.h"
#include "mafMatrix.h"
#include "mafSmartPointer.h"

/** Event class to transport a triggering button.
  Evant issue by 2D and 3D tracking devices in correspondace to
  MoveEvent, ButtonDownEvent and ButtonUpevent. 2D devices set the 
  X and Y coordinates, while 3D devices set the pose matrix. The event
  can also store an optional button argument, used to store the button 
  indexs when events for button down and button up are issued. Also an
  optional word of modifiers can be used.
  @sa mafEventBase mmdTracker MoveEvent ButtonDownEvent ButtonUpEvent
*/
class mafEventInteraction : public mafEventBase
{
public:
  mafTypeMacro(mafEventInteraction,mafEventBase);

  /** Set screen position, for 2D tracking devices */
  void Set2DPosition(double x,double y);
  /** Set screen position, for 2D tracking devices */
  void Set2DPosition(const double pos[2]);

  /** Get screen position, for 2D tracking devices */
  void Get2DPosition(double pos[2]);

  /** set the flag for 2D coords */
  void SetXYFlag(bool val);

  /** get the flag for 2D coords */
  bool GetXYFlag();
  
  /** Set/Get the triggering button */
  void SetButton(int button);
  int GetButton() {return m_Button;}

  /** Set/Get the optional key argument */
  void SetKey(unsigned char key);
  unsigned char GetKey() {return m_Key;}

  /** Set/Get the pose matrix, for 3D tracking devices */
  mafMatrix *GetMatrix();
  void SetMatrix(mafMatrix *matrix);

  /** Set/Get the given modifier value*/
  void SetModifier(unsigned long idx,bool value=true);
  bool GetModifier(unsigned long idx);
  void SetModifiers(unsigned long modifiers);
  unsigned long GetModifiers() {return m_Modifiers;}

  virtual void DeepCopy(mafEventBase *event);
  
  mafEventInteraction(void *sender=NULL,mafID id=-1,int button=0,unsigned long modifiers=0):
  mafEventBase(sender,id),m_Button(button),m_Modifiers(modifiers),m_Key(0),m_X(0),m_Y(0),m_XYFlag(false) {}

  mafEventInteraction(void *sender,mafID id,mafMatrix *matrix,int button=0,unsigned long modifiers=0):
  mafEventBase(sender,id),m_Button(button),m_Modifiers(modifiers),m_Key(0),m_Matrix(matrix),m_X(0),m_Y(0),m_XYFlag(false) {}

  mafEventInteraction(void *sender,mafID id,double x,double y,int button=0,unsigned long modifiers=0):
  mafEventBase(sender,id),m_X(x),m_Y(y),m_XYFlag(true),m_Button(button),m_Modifiers(modifiers),m_Key(0) {}
  virtual ~mafEventInteraction() {}
 
protected:

  int           m_Button;     ///< Optional button which triggered the event
  unsigned long m_Modifiers;  ///< Optional modifiers for the button 
  unsigned char m_Key;        ///< Optional Key 
  double        m_X;          ///< X coordinate, used by mouse device
  double        m_Y;          ///< Y coordinate, used by mouse device
  bool          m_XYFlag;     ///< Used to signal a 2D coordinate is present

  mafAutoPointer<mafMatrix>  m_Matrix;    ///< Pose matrix, used by 3D trackers
};

#endif /* __mafEventInteraction_h */
 
