/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEOutputScalarMatrix
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEOutputScalarMatrix_h
#define __albaVMEOutputScalarMatrix_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVMEOutput.h"
#include "vnl/vnl_matrix.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVMEGeneric;
#ifdef ALBA_USE_VTK
class vtkPolyData;
#endif //ALBA_USE_VTK

#ifdef ALBA_EXPORTS
#include "albaDllMacros.h"
EXPORT_VNL_MATRIX(ALBA_EXPORT, double);
EXPORT_VNL_VECTOR(ALBA_EXPORT, double);
#endif

/** NULL output for VME node with a scalar output data.
  albaVMEOutputScalarMatrix is the output produced by a node generating an output
  with a VNL matrix representing scalar/vector/tensor data.
*/
class ALBA_EXPORT albaVMEOutputScalarMatrix : public albaVMEOutput
{
public:
  albaTypeMacro(albaVMEOutputScalarMatrix,albaVMEOutput);

  /**
    Return a VNL matrix corresponding to the current time. This is
    the output of the DataPipe currently attached to the VME Scalar.
    Usually the output is a  "smart copy" of one of the vnl matrix in 
    the DataArray. An event is rised when the output data changes to allow attached classes to 
    update their input.*/
  virtual vnl_matrix<double> &GetScalarData();

#ifdef ALBA_USE_VTK
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
  albaVMEOutputScalarMatrix(); // to be allocated with New()
  virtual ~albaVMEOutputScalarMatrix(); // to be deleted with Delete()

  albaString m_NumberOfRows;
  albaString m_NumberOfColumns;

#ifdef ALBA_USE_VTK
  vtkPolyData *m_Polydata;
#endif

  albaGUI *CreateGui();

private:
  albaVMEOutputScalarMatrix(const albaVMEOutputScalarMatrix&); // Not implemented
  void operator=(const albaVMEOutputScalarMatrix&); // Not implemented
};
#endif
