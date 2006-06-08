/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-06-08 14:07:45 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputScalar_h
#define __mafVMEOutputScalar_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutput.h"
#include "vnl/vnl_matrix.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEGereric;

/** NULL output for VME node with a scalar output data.
  mafVMEOutputScalar is the output produced by a node generating an output
  with a VNL matrix representing scalar/vector/tensor data.
*/
class MAF_EXPORT mafVMEOutputScalar : public mafVMEOutput
{
public:
  mafTypeMacro(mafVMEOutputScalar,mafVMEOutput);

  /**
    Return a VNL matrix corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME Scalar.
    Usually the output is a  "smart copy" of one of the vnl matrix in 
    the DataArray. An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vnl_matrix<double> &GetScalarData();

  /**
    Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();
    
protected:
  mafVMEOutputScalar(); // to be allocated with New()
  virtual ~mafVMEOutputScalar(); // to be deleted with Delete()

  mafString m_NumberOfRows;
  mafString m_NumberOfColumns;

  mmgGui *CreateGui();

private:
  mafVMEOutputScalar(const mafVMEOutputScalar&); // Not implemented
  void operator=(const mafVMEOutputScalar&); // Not implemented
};
#endif
