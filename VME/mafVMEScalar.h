/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalar.h,v $
  Language:  C++
  Date:      $Date: 2007-12-11 11:23:37 $
  Version:   $Revision: 1.7 $
  Authors:   Paolo Quadrani
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

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEOutputScalar;
class vtkDataSet;

/** mafVMEScalar */
class MAF_EXPORT mafVMEScalar : public mafVMEGenericAbstract
{
public:
  mafTypeMacro(mafVMEScalar,mafVMEGenericAbstract);

  /** Set the data for the given timestamp. 
  This function automatically creates a VMEItem for the data to be stored.
  Return MAF_OK if succeeded, MAF_ERROR if they kind of data is not accepted by
  this type of VME. */
  virtual int SetData(double data, mafTimeStamp t);

  /** Set the time for this VME.
  It updates also the vtk representation for the scalar data.*/
  void SetTimeStamp(mafTimeStamp t);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another mafVMEScalar into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another mafVMEScalar. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output.*/  
  mafVMEOutputScalar *GetScalarOutput() {return (mafVMEOutputScalar *)GetOutput();}

  /** return the right type of output */  
  virtual mafVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipeScalar");};

protected:
  mafVMEScalar();
  virtual ~mafVMEScalar();

  void InternalPreUpdate();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

private:
  mafVMEScalar(const mafVMEScalar&); // Not implemented
  void operator=(const mafVMEScalar&); // Not implemented
};
#endif
