/*=========================================================================

 Program: MAF2
 Module: mafASCIIImporterUtility
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafASCIIImporterUtility_h
#define __mafASCIIImporterUtility_h

#include "mafDefines.h"
#include <vnl/vnl_matrix.h>
#include <vector>

/**
  Class Name: mafASCIIImporterUtility.
  Tool based on VNL for extract information from matrix.
*/
class MAF_EXPORT mafASCIIImporterUtility
{
public:
  /** constructor.*/
  mafASCIIImporterUtility();
  /** destructor.*/
  ~mafASCIIImporterUtility();
  
  // Description:
  // Read the matrix from a filename by means of the VNL library
  int ReadFile(const char *fname);

  /** Return the number of rows of the scalar matrix.*/
  unsigned GetNumberOfRows() {return m_NumOfRows;};

  /** Return the number of columns of the scalar matrix.*/
  unsigned GetNumberOfCols() {return m_NumOfCols;};

  /** Return the number of elements of the scalar matrix.*/
  unsigned GetNumberOfScalars() {return m_NumOfCols * m_NumOfRows;};

  /** Return the scalar value at row = r and column = c.*/
  double GetScalar(unsigned r, unsigned c) {return m_Matrix.get(r,c);};

  /** Return the whole scalar matrix.*/
  vnl_matrix<double> &GetMatrix() {return m_Matrix;};

  /** Fill the given vector with the values in specified row.*/
  void ExtractRow(unsigned r, std::vector<double> &row);

  /** Fill the given array with the values in specified row.*/
  void ExtractRow(unsigned r, double *row);

  /** Fill the given vector with the values in specified column.*/
  void ExtractColumn(unsigned c, std::vector<double> &col);

  /** Fill the given array with the values in specified column.*/
  void ExtractColumn(unsigned c, double *col);

protected:
  vnl_matrix<double> m_Matrix;
  unsigned m_NumOfRows;
  unsigned m_NumOfCols;
};
#endif
