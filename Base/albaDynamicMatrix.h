/*=========================================================================

Program: ALBA
Module:  albaDynamicMatrix.h
Authors: Gianluigi Crimi

Copyright (c) BIC
All rights reserved. See Copyright.txt or
http://www.scsitaly.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaDynamicMatrix_h
#define __albaDynamicMatrix_h

#include "albaDefines.h"
#include "albaTextFileReaderHelper.h"

#include <cstddef>
#include <vector>

/**
  albaDynamicMatrix - Dynamic rectangular matrix of double values.
*/
class ALBA_EXPORT albaDynamicMatrix : public albaTextFileReaderHelper
{
public:
  albaDynamicMatrix();
  albaDynamicMatrix(int rows, int columns);
  albaDynamicMatrix(int rows, int columns, double value);

  virtual ~albaDynamicMatrix();

  albaDynamicMatrix(const albaDynamicMatrix &matrix);
  albaDynamicMatrix &operator=(const albaDynamicMatrix &matrix);

  /** Resizes the matrix and preserves the existing values when possible.
    New elements are initialized to zero. */
  void Resize(int rows, int columns);

  /** Resizes the matrix and initializes all elements with value. */
  void Resize(int rows, int columns, double value);

  /** Sets all matrix elements to value. */
  void Fill(double value);

  /** Appends a row initialized with zeroes. */
  void AddRow();

  /** Appends a row initialized with value. */
  void AddRow(double value);

  /** Appends a row containing the specified values.
    If the matrix has no columns, the number of columns is set
    to values.size(). */
  void AddRow(const std::vector<double> &values);
  
  /** Appends a row containing the specified values.
    If the matrix has no columns, the number of columns is set
    to the number of elements in values. */
  void AddRow(double *values);

  /** Returns the number of rows.*/
  int GetRowsNum() const;

  /** Returns the number of columns. */
  int GetColNum() const;

  /** Returns true if the matrix has no usable elements. */
  bool IsEmpty() const;

  /**Returns a reference to the specified element.
    Throws std::out_of_range if the index is invalid. */
  double &At(int row, int column);
  const double &At(int row, int column) const;

  /** Provides access to the specified element.
    Throws std::out_of_range if the index is invalid. */
  double &operator()(int row, int column);
  const double &operator()(int row, int column) const;

  /** Returns a pointer to the contiguous matrix data.
    Returns NULL if the matrix has no stored elements. */
  double *GetData();
  const double *GetData() const;

  bool operator==(const albaDynamicMatrix &matrix) const;
  bool operator!=(const albaDynamicMatrix &matrix) const;

  /** Reads matrix data from a text file.
    Returns ALBA_OK on success, ALBA_ERROR on failure. */
	int ReadFromFile(albaString filename);

  /** Fills the given vector with the values in specified row.*/
  void ExtractRow(int rowNum, std::vector<double> &row);

	/** Fills the given array with the values in specified row.*/
  void ExtractRow(int rowNum, double *row);

	/** Fills the given array with the values in specified column.*/ 
  void ExtractColumn(int colNum, std::vector<double> &col);

	/** Fills the given array with the values in specified column.*/ 
  void ExtractColumn(int colNum, double *col);

private:
  inline int GetIndex(int row, int column)  { return row * m_Columns + column; }
  void CheckIndex(int row, int column) const;

  int m_Rows;
  int m_Columns;
  std::vector<double> m_Data;
};

#endif