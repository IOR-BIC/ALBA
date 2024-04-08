/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataPipeInterpolatorScalarMatrix
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaScalarMatrixInterpolator_h
#define __albaScalarMatrixInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDataPipeInterpolator.h"
#include <vnl/vnl_matrix.h>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class albaVMEItemScalarMatrix;

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_VNL_MATRIX(ALBA_EXPORT, double);
#endif

/**
  Class Name: albaScalarMatrixInterpolator.
  Data interpolator specialized for albaVMEScalarMatrix (for VNL matrix data).
  This interpolator is specialized for scalar data. By default selects the 
  right VMEItem, extracts the inner VNL matrix and set it as input of the 
  DataPipe.

  @sa albaVMEScalarMatrix
  
  @todo
  -
*/
class ALBA_EXPORT albaDataPipeInterpolatorScalarMatrix : public albaDataPipeInterpolator
{
public:
  /** type macro for RTTI and instance creation*/
  albaTypeMacro(albaDataPipeInterpolatorScalarMatrix,albaDataPipeInterpolator);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(albaVME *vme);

  /**
    Get the MTime: this is the bit of magic that makes everything work.*/
  //virtual vtkMTimeType GetMTime();

  /** process events coming from vtkALBADataPipe bridge component */
  void OnEvent(albaEventBase *e);

   /**  Get the output of the interpolator item*/
  albaVMEItemScalarMatrix *GetCurrentItem() {return (albaVMEItemScalarMatrix *)m_CurrentItem;}

  /** return the vnl_matrix data generated as output to this data pipe */
  virtual vnl_matrix<double> &GetScalarData();

protected:
  /** constructor */
  albaDataPipeInterpolatorScalarMatrix();
  /** destructor */
  virtual ~albaDataPipeInterpolatorScalarMatrix();

  /** Set m_ScalarData to current item data*/
  virtual void PreExecute();

  /** Execute possible procedural code: Empty function */
  virtual void Execute() {}

  vnl_matrix<double> m_ScalarData;

private:
  /** copy constructor not implemented */
  albaDataPipeInterpolatorScalarMatrix(const albaDataPipeInterpolatorScalarMatrix&); 
  /** assignment operator not implemeted */
  void operator=(const albaDataPipeInterpolatorScalarMatrix&); 
};

#endif /* __albaScalarInterpolator_h */
