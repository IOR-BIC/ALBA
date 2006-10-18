/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEScalar.h,v $
  Language:  C++
  Date:      $Date: 2006-10-18 13:09:02 $
  Version:   $Revision: 1.2 $
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
  virtual int SetData(vnl_matrix<double> &data, mafTimeStamp t);

  /** Sets which scalar ID has to be used as X coordinate for the VTK Representation.*/
  void SetScalarIdForXCoordinate(int id);
  
  /** Sets which scalar ID has to be used as Y coordinate for the VTK Representation.*/
  void SetScalarIdForYCoordinate(int id);
  
  /** Sets which scalar ID has to be used as Z coordinate for the VTK Representation.*/
  void SetScalarIdForZCoordinate(int id);

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

  int m_XID; ///< Scalar id or timestamp (if -1) associated with the X coordinate of the VTK representation;
  int m_YID; ///< Scalar id or timestamp (if -1) associated with the Y coordinate of the VTK representation;
  int m_ZID; ///< Scalar id or timestamp (if -1) associated with the Z coordinate of the VTK representation;

private:
  mafVMEScalar(const mafVMEScalar&); // Not implemented
  void operator=(const mafVMEScalar&); // Not implemented
};
#endif
