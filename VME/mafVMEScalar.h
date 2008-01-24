/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalar.h,v $
  Language:  C++
  Date:      $Date: 2008-01-24 12:23:06 $
  Version:   $Revision: 1.8 $
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
class mafScalarVector;
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
  It updates also the VTK representation for the scalar data.*/
  void SetTimeStamp(mafTimeStamp t);

  /** Return true is this VME has more than one time stamp, either  for data or matrices */
  virtual bool IsAnimated();

  /** Get the pointer to the array of Scalar's*/
  mafScalarVector *GetScalarVector() {return m_ScalarVector;}

  /** Return the list of time stamps of the data scalar array stored in this VME. */
  virtual void GetDataTimeStamps(std::vector<mafTimeStamp> &kframes);

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrices and VME scalar data*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** Set the time bounds for the time varying VME based on scalar data and matrix vector.*/
  void GetLocalTimeBounds(mafTimeStamp tbounds[2]);

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

  mafScalarVector *m_ScalarVector;

private:
  mafVMEScalar(const mafVMEScalar&); // Not implemented
  void operator=(const mafVMEScalar&); // Not implemented
};
#endif
