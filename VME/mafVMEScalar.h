/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-06-08 14:07:45 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEScalar_h
#define __mafVMEScalar_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEGenericAbstract.h"
#include "vnl/vnl_matrix.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEOutputScalar;

/** mafVMEScalar */
class MAF_EXPORT mafVMEScalar : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMEScalar,mafVMEGenericAbstract);

  /** 
  Set data for the give timestamp. This function automatically creates a
  a VMEItem for the data to be stored.
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. */
  virtual int SetData(vnl_matrix<double> &data, mafTimeStamp t);

  /** return the right type of output.*/  
  mafVMEOutputScalar *GetScalarOutput() {return (mafVMEOutputScalar *)GetOutput();}

  /** return the right type of output */  
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  //static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeScalar");};

protected:
  mafVMEScalar();
  virtual ~mafVMEScalar();

private:
  mafVMEScalar(const mafVMEScalar&); // Not implemented
  void operator=(const mafVMEScalar&); // Not implemented
};
#endif
