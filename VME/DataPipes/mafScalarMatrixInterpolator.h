/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafScalarMatrixInterpolator.h,v $
  Language:  C++
  Date:      $Date: 2009-05-07 14:48:43 $
  Version:   $Revision: 1.2.2.1 $
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
#include "mafDataInterpolator.h"
#include <vnl/vnl_matrix.h>

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVMEItemScalarMatrix;

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
class MAF_EXPORT mafScalarMatrixInterpolator : public mafDataInterpolator
{
public:
  /** type macro for RTTI and instance creation*/
  mafTypeMacro(mafScalarMatrixInterpolator,mafDataInterpolator);

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
  mafScalarMatrixInterpolator();
  /** destructor */
  virtual ~mafScalarMatrixInterpolator();

  /** Set m_ScalarData to current item data*/
  virtual void PreExecute();

  /** Execute possible procedural code: Empty function */
  virtual void Execute() {}

  vnl_matrix<double> m_ScalarData;

private:
  /** copy constructor not implemented */
  mafScalarMatrixInterpolator(const mafScalarMatrixInterpolator&); 
  /** assignment operator not implemeted */
  void operator=(const mafScalarMatrixInterpolator&); 
};

#endif /* __mafScalarInterpolator_h */
