/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataPipeInterpolatorScalarMatrix.h,v $
  Language:  C++
  Date:      $Date: 2011-11-15 09:01:01 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafScalarMatrixInterpolator_h
#define __mafScalarMatrixInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataPipeInterpolator.h"
#include <vnl/vnl_matrix.h>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVMEItemScalarMatrix;

template class MAF_EXPORT vnl_matrix<double>;

/**
  Class Name: mafScalarMatrixInterpolator.
  Data interpolator specialized for mafVMEScalarMatrix (for VNL matrix data).
  This interpolator is specialized for scalar data. By default selects the 
  right VMEItem, extracts the inner VNL matrix and set it as input of the 
  DataPipe.

  @sa mafVMEScalarMatrix
  
  @todo
  -
*/
class MAF_EXPORT mafDataPipeInterpolatorScalarMatrix : public mafDataPipeInterpolator
{
public:
  /** type macro for RTTI and instance creation*/
  mafTypeMacro(mafDataPipeInterpolatorScalarMatrix,mafDataPipeInterpolator);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(mafVME *vme);

  /**
    Get the MTime: this is the bit of magic that makes everything work.*/
  //virtual unsigned long GetMTime();

  /** process events coming from vtkMAFDataPipe bridge component */
  void OnEvent(mafEventBase *e);

   /**  Get the output of the interpolator item*/
  mafVMEItemScalarMatrix *GetCurrentItem() {return (mafVMEItemScalarMatrix *)m_CurrentItem;}

  /** return the vnl_matrix data generated as output to this data pipe */
  virtual vnl_matrix<double> &GetScalarData();

protected:
  /** constructor */
  mafDataPipeInterpolatorScalarMatrix();
  /** destructor */
  virtual ~mafDataPipeInterpolatorScalarMatrix();

  /** Set m_ScalarData to current item data*/
  virtual void PreExecute();

  /** Execute possible procedural code: Empty function */
  virtual void Execute() {}

  vnl_matrix<double> m_ScalarData;

private:
  /** copy constructor not implemented */
  mafDataPipeInterpolatorScalarMatrix(const mafDataPipeInterpolatorScalarMatrix&); 
  /** assignment operator not implemeted */
  void operator=(const mafDataPipeInterpolatorScalarMatrix&); 
};

#endif /* __mafScalarInterpolator_h */
