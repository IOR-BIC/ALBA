/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmiGeneric6DOF.h,v $
  Language:  C++
  Date:      $Date: 2005-10-18 13:45:44 $
  Version:   $Revision: 1.4 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmiGeneric6DOF_h
#define __mmiGeneric6DOF_h

#include "mmi6DOF.h"

/** implements 6DOF move of objects in the scene.
  This class implements a 6DOF move of objects in the scene. The interaction
  modality is a direct manipulation, where movement of the tracker are mapped
  into movements of the object. More details to be written...
*/
class mmiGeneric6DOF : public mmi6DOF
{
public:
  mafTypeMacro(mmiGeneric6DOF,mmi6DOF);

  /**  Process events coming from tracker */
  virtual void OnEvent(mafEventBase *event);

  /** manage move events */
  virtual void OnMove(mafEventInteraction *e);

  /** 
    Enable/Disable differential moving. If enable this interactor moves 
    the object by applying a differential transformation to its original 
    pose, i.e. the original pose is retained and only the delta transform
    is applied to it.
    Default is false. */
  void SetDifferentialMoving(bool flag) {m_DifferentialMoving=flag;}
  bool GetDifferentialMoving() {return m_DifferentialMoving != 0;} ///< @sa SetDifferentialMoving()
  void DifferentialMovingOn() {SetDifferentialMoving(true);} ///< @sa SetDifferentialMoving()
  void DifferentialMovingOff() {SetDifferentialMoving(false);} ///< @sa SetDifferentialMoving()

protected:
  mmiGeneric6DOF();
  virtual ~mmiGeneric6DOF();

  /** Update the transform */
  void Update();

  mafTransform*       m_OutputTransform; 
  double              m_OutputOrientation[3]; ///< orientation of the output matrix
  double              m_OutputPosition[3]; ///< position of the output matrix
  double              m_PivotPosition[3]; ///< position of the selected object at start of interaction

  int                 m_DifferentialMoving;

private:
  mmiGeneric6DOF(const mmiGeneric6DOF&);  // Not implemented.
  void operator=(const mmiGeneric6DOF&);  // Not implemented.
};

#endif 
