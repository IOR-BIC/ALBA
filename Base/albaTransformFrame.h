/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaTransformFrame
 Authors: Marco Petrone, Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaTransformFrame_h
#define __albaTransformFrame_h

#include "albaTransformBase.h"

class vtkMatrix4x4;

/** albaTransformFrame - class for multi frame homogeneous transformations.
  albaTransformFrame provides functionalities for converting homogeneous transformations,
  between different frames.
  The idea of this class is to represent by itself a transformation, obtained by changing
  the an input transformation for its original coordinate system to a target coordinate 
  system. Coordinate systems are by default the world coords system. Reference systems can 
  be expressed as a albaTransformBase.

  @sa albaTransformBase albaTransform
 
  @todo
  - Modifica GetMTime per considerare anche l'MTime di InputFrame e TargetFrame
 */
class ALBA_EXPORT albaTransformFrame : public albaTransformBase
{
 public:
   albaTransformFrame();
  ~albaTransformFrame();
	
  albaTypeMacro(albaTransformFrame,albaTransformBase);

  //virtual void Print(std::ostream& os, const int tabs=0) const;

  /** set the matrix to be transformed */
  void SetInput(albaTransformBase *frame);
  void SetInput(albaMatrix *frame);
  albaTransformBase *GetInput() {return m_Input;}

  /**
  Set/Get the input reference system, i.e. the reference system of the 
  input matrix.*/
  void SetInputFrame(albaMatrix *frame);
  void SetInputFrame(albaTransformBase *frame);
  albaTransformBase *GetInputFrame() {return m_InputFrame;}

  /**
  Set/Get the output reference system, i.e. the reference system of the output
  matrix or the target reference system for point transformation.*/
  void SetTargetFrame(albaMatrix *frame);
  void SetTargetFrame(albaTransformBase *frame);
  albaTransformBase *GetTargetFrame() {return m_TargetFrame;}

  /** 
    Return current modification time, taking inro consideration also
    Input, InputFrame and TargetFrame. */
  virtual vtkMTimeType GetMTime();

protected:
  void InternalUpdate();

  albaTransformBase  *m_Input;
  albaTransformBase  *m_InputFrame;
  albaTransformBase  *m_TargetFrame;

private:
  albaTransformFrame(const albaTransformFrame&);  // Not implemented.
  void operator=(const albaTransformFrame&);  // Not implemented.
};

#endif
