#ifndef __albaDynamicMatrix_h
#define __albaDynamicMatrix_h

#include "albaDefines.h"

#include <cstddef>
#include <vector>

/**
  albaDynamicMatrix - Dynamic rectangular matrix of double values.
*/
class ALBA_EXPORT albaDynamicMatrix
{
public:
  albaDynamicMatrix();
  albaDynamicMatrix(int rows, int columns);
  albaDynamicMatrix(int rows, int columns, double value);

  virtual ~albaDynamicMatrix();

  albaDynamicMatrix(const albaDynamicMatrix &matrix);
  albaDynamicMatrix &operator=(const albaDynamicMatrix &matrix);

  /**
    Resizes the matrix and preserves the existing values when possible.
    New elements are initialized to zero.
  */
  void Resize(int rows, int columns);

  /**
    Resizes the matrix and initializes all elements with value.
  */
  void Resize(int rows, int columns, double value);

  /**
    Sets all matrix elements to value.
  */
  void Fill(double value);

  /**
    Appends a row initialized with zeroes.
  */
  void AddRow();

  /**
    Appends a row initialized with value.
  */
  void AddRow(double value);

  /**
    Appends a row containing the specified values.
    If the matrix has no columns, the number of columns is set
    to values.size().
  */
  void AddRow(const std::vector<double> &values);

  /**
    Returns the number of rows.
  */
  int GetNumberOfRows() const;

  /**
    Returns the number of columns.
  */
  int GetNumberOfColumns() const;

  /**
    Returns true if the matrix has no usable elements.
  */
  bool IsEmpty() const;

  /**
    Returns a reference to the specified element.
    Throws std::out_of_range if the index is invalid.
  */
  double &At(int row, int column);
  const double &At(int row, int column) const;

  /**
    Provides access to the specified element.
    Throws std::out_of_range if the index is invalid.
  */
  double &operator()(int row, int column);
  const double &operator()(int row, int column) const;

  /**
    Returns a pointer to the contiguous matrix data.
    Returns NULL if the matrix has no stored elements.
  */
  double *GetData();
  const double *GetData() const;

  bool operator==(const albaDynamicMatrix &matrix) const;
  bool operator!=(const albaDynamicMatrix &matrix) const;

private:
  inline int GetIndex(int row, int column)  { return row * m_Columns + column; }
  void CheckIndex(int row, int column) const;

  int m_Rows;
  int m_Columns;
  std::vector<double> m_Data;
};

#endif