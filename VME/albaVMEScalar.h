/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEScalar
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEScalar_h
#define __albaVMEScalar_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEGenericAbstract.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMEOutputScalar;
class albaScalarVector;
class vtkDataSet;

/** albaVMEScalar */
class ALBA_EXPORT albaVMEScalar : public albaVMEGenericAbstract
{
public:
  albaTypeMacro(albaVMEScalar,albaVMEGenericAbstract);

  /** Set the data for the given timestamp. 
  This function automatically creates a VMEItem for the data to be stored.
  Return ALBA_OK if succeeded, ALBA_ERROR if they kind of data is not accepted by
  this type of VME. */
  virtual int SetData(double data, albaTimeStamp t);

  /** Set the time for this VME.
  It updates also the VTK representation for the scalar data.*/
  void SetTimeStamp(albaTimeStamp t);

  /** Return true is this VME has more than one time stamp, either  for data or matrices */
  virtual bool IsAnimated();

  /** Get the pointer to the array of Scalar's*/
  albaScalarVector *GetScalarVector() {return m_ScalarVector;}

  /** Return the list of time stamps of the data scalar array stored in this VME. */
  virtual void GetDataTimeStamps(std::vector<albaTimeStamp> &kframes);

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrices and VME scalar data*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** Set the time bounds for the time varying VME based on scalar data and matrix vector.*/
  void GetLocalTimeBounds(albaTimeStamp tbounds[2]);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another albaVMEScalar into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another albaVMEScalar. */
  virtual bool Equals(albaVME *vme);

  /** return the right type of output.*/  
  albaVMEOutputScalar *GetScalarOutput() {return (albaVMEOutputScalar *)GetOutput();}

  /** return the right type of output */  
  virtual albaVMEOutput *GetOutput();

  /** return icon */
  static char** GetIcon();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeScalar");};

protected:
  albaVMEScalar();
  virtual ~albaVMEScalar();

  void InternalPreUpdate();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  albaScalarVector *m_ScalarVector;

private:
  albaVMEScalar(const albaVMEScalar&); // Not implemented
  void operator=(const albaVMEScalar&); // Not implemented
};
#endif
