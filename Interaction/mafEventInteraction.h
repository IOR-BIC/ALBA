/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafEventInteraction.h,v $
  Language:  C++
  Date:      $Date: 2005-04-28 16:10:11 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafEventInteraction_h
#define __mafEventInteraction_h

#include "mafEventBase.h"
#include "mafMatrix.h"

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
  void Set2DPosition(double pos[2]);

  /** Get screen position, for 2D tracking devices */
  void Get2DPosition(double pos[2]);

  /** set the flag for 2D coords */
  void SetXYFlag(bool val);

  /** get the flag for 2D coords */
  bool GetXYFlag();
  
  /** Set/Get the triggering button */
  void SetButton(int button);
  int GetButton() {return Button;}

  /** Set/Get the optional key argument */
  void SetKey(unsigned char key);
  unsigned char GetKey() {return Key;}

  /** Set/Get the pose matrix, for 3D tracking devices */
  mafMatrix *GetMatrix();
  void SetMatrix(mafMatrix *matrix);

  /** Set/Get the given modifier value*/
  void SetModifier(unsigned long idx,bool value=true);
  bool GetModifier(unsigned long idx);
  void SetModifiers(unsigned long modifiers);
  unsigned long GetModifiers() {return Modifiers;}

  void DeepCopy(mafEventInteraction *e);
  
  mafEventInteraction(mafID id=0,void *sender=NULL,mafMatrix *matrix=NULL,int button=0,unsigned long modifiers=0):
  mafEventBase(id,sender),Button(button),Modifiers(modifiers),Key(0),Matrix(matrix),X(0),Y(0),XYFlag(false) {ReferenceCount=0;}

  mafEventInteraction(mafID id,void *sender,double x,double y,int button=0,unsigned long modifiers=0):
  mafEventBase(id,sender),X(x),Y(y),XYFlag(true),Button(button),Modifiers(modifiers),Key(0),Matrix(NULL) {ReferenceCount=0;}
  virtual ~mafEventInteraction() {}
 
protected:

  int           m_Button;     ///< Optional button which triggered the event
  unsigned long m_Modifiers;  ///< Optional modifiers for the button 
  unsigned char m_Key;        ///< Optional Key 
  double        m_X;          ///< X coordinate, used by mouse device
  double        m_Y;          ///< Y coordinate, used by mouse device
  bool          m_XYFlag;     ///< Used to signal a 2D coordinate is present
  mafMatrix     m_Matrix;    ///< Pose matrix, used by 3D trackers
  

private:
  mafEventInteraction(const mafEventInteraction& c) {}
  void operator=(const mafEventInteraction&) {}
};

#endif /* __mafEventInteraction_h */
 
