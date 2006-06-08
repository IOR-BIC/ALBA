/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafASCIIImporterUtility.h,v $
Language:  C++
Date:      $Date: 2006-06-08 14:09:10 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mafASCIIImporterUtility_h
#define __mafASCIIImporterUtility_h

#include <vnl/vnl_matrix.h>
#include <vector>

//----------------------------------------------------------------------------
class mafASCIIImporterUtility
//----------------------------------------------------------------------------
{
public:
  mafASCIIImporterUtility();
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
