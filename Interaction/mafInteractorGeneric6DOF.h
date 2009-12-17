/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafInteractorGeneric6DOF.h,v $
  Language:  C++
  Date:      $Date: 2009-12-17 11:46:39 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mafInteractorGeneric6DOF_h
#define __mafInteractorGeneric6DOF_h

#include "mafInteractor6DOF.h"

/** implements 6DOF move of objects in the scene.
  This class implements a 6DOF move of objects in the scene. The interaction
  modality is a direct manipulation, where movement of the tracker are mapped
  into movements of the object. More details to be written...
*/
class mafInteractorGeneric6DOF : public mafInteractor6DOF
{
public:
  mafTypeMacro(mafInteractorGeneric6DOF,mafInteractor6DOF);

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
  mafInteractorGeneric6DOF();
  virtual ~mafInteractorGeneric6DOF();

  /** Update the transform */
  void Update();

  mafTransform*       m_OutputTransform; 
  double              m_OutputOrientation[3]; ///< orientation of the output matrix
  double              m_OutputPosition[3]; ///< position of the output matrix
  double              m_PivotPosition[3]; ///< position of the selected object at start of interaction

  int                 m_DifferentialMoving;

private:
  mafInteractorGeneric6DOF(const mafInteractorGeneric6DOF&);  // Not implemented.
  void operator=(const mafInteractorGeneric6DOF&);  // Not implemented.
};

#endif 
