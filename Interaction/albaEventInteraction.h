/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaEventInteraction
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaEventInteraction_h
#define __albaEventInteraction_h

#include "albaEventBase.h"
#include "albaMatrix.h"
#include "albaSmartPointer.h"

template class ALBA_EXPORT albaAutoPointer<albaMatrix>;

/** Event class to transport a triggering button and a position from pointing devices.
  Evant issue by 2D and 3D tracking devices in correspondace to
  MoveEvent, ButtonDownEvent and ButtonUpevent. 2D devices set the 
  X and Y coordinates, while 3D devices set the pose matrix. The event
  can also store an optional button argument, used to store the button 
  indexs when events for button down and button up are issued. Also an
  optional word of modifiers can be used.
  @sa albaEventBase albaDeviceButtonsPadTracker MoveEvent ButtonDownEvent ButtonUpEvent
*/
class ALBA_EXPORT albaEventInteraction : public albaEventBase
{
public:
  /** RTTI macro*/
  albaTypeMacro(albaEventInteraction,albaEventBase);

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
  
  /** Set the triggering button */
  void SetButton(int button);
  /** Get the triggering button */
  int GetButton() {return m_Button;}

  /** Set the optional key argument */
  void SetKey(unsigned char key);
  /** Get the optional key argument */
  unsigned char GetKey() {return m_Key;}

  /** Get the pose matrix, for 3D tracking devices */
  albaMatrix *GetMatrix();
  /** Set the pose matrix, for 3D tracking devices */
  void SetMatrix(albaMatrix *matrix);

  /** Set the given modifier value*/
  void SetModifier(unsigned long idx,bool value=true);
  /** Get the given modifier value*/
  bool GetModifier(unsigned long idx);
  /** Set the given modifiers value*/
  void SetModifiers(unsigned long modifiers);
  /** Get the given modifiers value*/
  unsigned long GetModifiers() {return m_Modifiers;}

  /** Deep Copy of the object*/
  virtual void DeepCopy(const albaEventBase *event);
  
  /** overloaded constructor */
  albaEventInteraction(void *sender=NULL,albaID id=-1,int button=0,unsigned long modifiers=0):
  albaEventBase(sender,id),m_Button(button),m_Modifiers(modifiers),m_Key(0),m_X(0),m_Y(0),m_XYFlag(false) {}

  /** overloaded constructor */
  albaEventInteraction(void *sender,albaID id,albaMatrix *matrix,int button=0,unsigned long modifiers=0):
  albaEventBase(sender,id),m_Button(button),m_Modifiers(modifiers),m_Key(0),m_Matrix(matrix),m_X(0),m_Y(0),m_XYFlag(false) {}

  /** overloaded constructor */
  albaEventInteraction(void *sender,albaID id,double x,double y,int button=0,unsigned long modifiers=0):
  albaEventBase(sender,id),m_X(x),m_Y(y),m_XYFlag(true),m_Button(button),m_Modifiers(modifiers),m_Key(0) {}
  virtual ~albaEventInteraction() {}
 
protected:

  int           m_Button;     ///< Optional button which triggered the event
  unsigned long m_Modifiers;  ///< Optional modifiers for the button 
  unsigned char m_Key;        ///< Optional Key 
  double        m_X;          ///< X coordinate, used by mouse device
  double        m_Y;          ///< Y coordinate, used by mouse device
  bool          m_XYFlag;     ///< Used to signal a 2D coordinate is present

  albaAutoPointer<albaMatrix>  m_Matrix;    ///< Pose matrix, used by 3D trackers
};

#endif /* __albaEventInteraction_h */
 
