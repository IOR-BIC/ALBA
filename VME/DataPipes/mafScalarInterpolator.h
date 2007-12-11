/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafScalarInterpolator.h,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:23:24 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafScalarInterpolator_h
#define __mafScalarInterpolator_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDataInterpolator.h"

//----------------------------------------------------------------------------
// forward declarations
//----------------------------------------------------------------------------
class mafVMEItemScalar;

/** data interpolator specialized for mafVMEScalar (for double data).
  This interpolator is specialized for scalar data. By default selects the 
  right VMEItem, extracts the inner double data and set it as input of the 
  DataPipe.

  @sa mafVMEScalar
  
  @todo
  -
*/
class MAF_EXPORT mafScalarInterpolator : public mafDataInterpolator
{
public:
  mafTypeMacro(mafScalarInterpolator,mafDataInterpolator);

  /** This DataPipe accepts only VME's with internal DataArray. */
  virtual bool Accept(mafVME *vme);

  /**
    Get the MTime: this is the bit of magic that makes everything work.*/
  //virtual unsigned long GetMTime();

  /** process events coming from vtkMAFDataPipe bridge component */
  void OnEvent(mafEventBase *e);

   /**  Get the output of the interpolator item*/
  mafVMEItemScalar *GetCurrentItem() {return (mafVMEItemScalar *)m_CurrentItem;}

  /** return the double scalar data generated as output to this data pipe */
  virtual double GetScalarData();

protected:
  mafScalarInterpolator();
  virtual ~mafScalarInterpolator();

  virtual void PreExecute();
  virtual void Execute() {}

  double m_ScalarData;

private:
  mafScalarInterpolator(const mafScalarInterpolator&); // Not implemented
  void operator=(const mafScalarInterpolator&); // Not implemented  
};
#endif /* __mafScalarInterpolator_h */
