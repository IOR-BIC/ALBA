/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEOutputScalarMatrix.h,v $
  Language:  C++
  Date:      $Date: 2009-03-30 10:11:38 $
  Version:   $Revision: 1.3.2.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEOutputScalarMatrix_h
#define __mafVMEOutputScalarMatrix_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVMEOutput.h"
#include "vnl/vnl_matrix.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class mafVMEGeneric;
#ifdef MAF_USE_VTK
class vtkPolyData;
#endif //MAF_USE_VTK

/** NULL output for VME node with a scalar output data.
  mafVMEOutputScalarMatrix is the output produced by a node generating an output
  with a VNL matrix representing scalar/vector/tensor data.
*/
class MAF_EXPORT mafVMEOutputScalarMatrix : public mafVMEOutput
{
public:
  mafTypeMacro(mafVMEOutputScalarMatrix,mafVMEOutput);

  /**
    Return a VNL matrix corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME Scalar.
    Usually the output is a  "smart copy" of one of the vnl matrix in 
    the DataArray. An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vnl_matrix<double> &GetScalarData();

#ifdef MAF_USE_VTK
  /**
  Return a VTK dataset with the choose scalar information corresponding to the current time. This is
  the output of the DataPipe currently attached to the VME.
  Usually the output is a  "smart copy" of one of the dataset in 
  the DataArray. In some cases it can be NULL, e.g. in case the number
  of stored Items is 0. Also special VME could not support VTK dataset output.
  An event rise when the output data changes to allow attached classes to 
  update their input.*/
  virtual vtkDataSet *GetVTKData();

  /** Function to update VTK representation of vnl matrix representing the scalar data.*/
  void UpdateVTKRepresentation();
#endif

  /** Update all the output data structures (data, bounds, matrix and abs matrix).*/
  virtual void Update();

  /** Retrieve Number Of Rows  written in a string*/
  const char *GetNumberOfRows(){return m_NumberOfRows.GetCStr();}

  /** Retrieve Number Of Columns  written in a string*/
  const char *GetNumberOfColumns(){return m_NumberOfColumns.GetCStr();}
    
protected:
  mafVMEOutputScalarMatrix(); // to be allocated with New()
  virtual ~mafVMEOutputScalarMatrix(); // to be deleted with Delete()

  mafString m_NumberOfRows;
  mafString m_NumberOfColumns;

#ifdef MAF_USE_VTK
  vtkPolyData *m_Polydata;
#endif

  mafGUI *CreateGui();

private:
  mafVMEOutputScalarMatrix(const mafVMEOutputScalarMatrix&); // Not implemented
  void operator=(const mafVMEOutputScalarMatrix&); // Not implemented
};
#endif
