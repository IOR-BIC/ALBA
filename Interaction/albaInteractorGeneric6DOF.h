/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaInteractorGeneric6DOF
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaInteractorGeneric6DOF_h
#define __albaInteractorGeneric6DOF_h

#include "albaInteractor6DOF.h"

/** implements 6DOF move of objects in the scene.
  This class implements a 6DOF move of objects in the scene. The interaction
  modality is a direct manipulation, where movement of the tracker are mapped
  into movements of the object. More details to be written...
*/
class ALBA_EXPORT albaInteractorGeneric6DOF : public albaInteractor6DOF
{
public:
  albaTypeMacro(albaInteractorGeneric6DOF,albaInteractor6DOF);

  /**  Process events coming from tracker */
  virtual void OnEvent(albaEventBase *event);

  /** manage move events */
  virtual void OnMove(albaEventInteraction *e);

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
  albaInteractorGeneric6DOF();
  virtual ~albaInteractorGeneric6DOF();

  /** Update the transform */
  void Update();

  albaTransform*       m_OutputTransform; 
  double              m_OutputOrientation[3]; ///< orientation of the output matrix
  double              m_OutputPosition[3]; ///< position of the output matrix
  double              m_PivotPosition[3]; ///< position of the selected object at start of interaction

  int                 m_DifferentialMoving;

private:
  albaInteractorGeneric6DOF(const albaInteractorGeneric6DOF&);  // Not implemented.
  void operator=(const albaInteractorGeneric6DOF&);  // Not implemented.
};

#endif 
