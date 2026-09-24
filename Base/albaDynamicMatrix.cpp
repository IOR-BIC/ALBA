#include "albaDefines.h"

#include "albaDynamicMatrix.h"

#include <algorithm>
#include <stdexcept>

//----------------------------------------------------------------------------
albaDynamicMatrix::albaDynamicMatrix(): m_Rows(0), m_Columns(0)
{
}

//----------------------------------------------------------------------------
albaDynamicMatrix::albaDynamicMatrix(int rows, int columns): m_Rows(rows), m_Columns(columns), m_Data(rows * columns, 0.0)
{
}

//----------------------------------------------------------------------------
albaDynamicMatrix::albaDynamicMatrix(int rows, int columns, double value): m_Rows(rows), m_Columns(columns), m_Data(rows * columns, value)
{
}

//----------------------------------------------------------------------------
albaDynamicMatrix::~albaDynamicMatrix()
{
}

//----------------------------------------------------------------------------
albaDynamicMatrix::albaDynamicMatrix(const albaDynamicMatrix &matrix): m_Rows(matrix.m_Rows), m_Columns(matrix.m_Columns), m_Data(matrix.m_Data)
{
}

//----------------------------------------------------------------------------
albaDynamicMatrix &albaDynamicMatrix::operator=(const albaDynamicMatrix &matrix)
{
  if (this != &matrix)
  {
    m_Rows = matrix.m_Rows;
    m_Columns = matrix.m_Columns;
    m_Data = matrix.m_Data;
  }

  return *this;
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::Resize(int rows, int columns)
{
  Resize(rows, columns, 0.0);
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::Resize(int rows, int columns, double value)
{
  std::vector<double> newData(rows * columns, value);

  int rowsToCopy = std::min(m_Rows, rows);
  int columnsToCopy = std::min(m_Columns, columns);

  for (int row = 0; row < rowsToCopy; ++row)
  {
    for (int column = 0; column < columnsToCopy; ++column)
      newData[row * columns + column] = m_Data[row * m_Columns + column];
  }

  m_Rows = rows;
  m_Columns = columns;
  m_Data.swap(newData);
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::Fill(double value)
{
  std::fill(m_Data.begin(), m_Data.end(), value);
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::AddRow()
{
  AddRow(0.0);
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::AddRow(double value)
{
  m_Data.insert(m_Data.end(), m_Columns, value);
  ++m_Rows;
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::AddRow(const std::vector<double> &values)
{
  if (m_Columns == 0)
  {
    if (m_Rows > 0 && !values.empty())
      m_Data.insert(m_Data.end(), m_Rows * values.size(), 0.0);

    m_Columns = values.size();
  }

  if (values.size() != m_Columns)
    throw std::invalid_argument("albaDynamicMatrix: invalid row size");

  m_Data.insert(m_Data.end(), values.begin(), values.end());
  ++m_Rows;
}

//----------------------------------------------------------------------------
int albaDynamicMatrix::GetNumberOfRows() const
{
  return m_Rows;
}

//----------------------------------------------------------------------------
int albaDynamicMatrix::GetNumberOfColumns() const
{
  return m_Columns;
}

//----------------------------------------------------------------------------
bool albaDynamicMatrix::IsEmpty() const
{
  return m_Rows == 0 || m_Columns == 0;
}

//----------------------------------------------------------------------------
double &albaDynamicMatrix::At( int row, int column)
{
  CheckIndex(row, column);
  return m_Data[GetIndex(row, column)];
}

//----------------------------------------------------------------------------
const double &albaDynamicMatrix::At( int row, int column) const
{
  CheckIndex(row, column);
  return m_Data[GetIndex(row, column)];
}

//----------------------------------------------------------------------------
double &albaDynamicMatrix::operator()( int row, int column)
{
  return At(row, column);
}

//----------------------------------------------------------------------------
const double &albaDynamicMatrix::operator()(int row, int column) const
{
  return At(row, column);
}

//----------------------------------------------------------------------------
double *albaDynamicMatrix::GetData()
{
  return m_Data.empty() ? NULL : &m_Data[0];
}

//----------------------------------------------------------------------------
const double *albaDynamicMatrix::GetData() const
{
  return m_Data.empty() ? NULL : &m_Data[0];
}

//----------------------------------------------------------------------------
bool albaDynamicMatrix::operator==(const albaDynamicMatrix &matrix) const
{
  return m_Rows == matrix.m_Rows &&
    m_Columns == matrix.m_Columns &&
    m_Data == matrix.m_Data;
}

//----------------------------------------------------------------------------
bool albaDynamicMatrix::operator!=(const albaDynamicMatrix &matrix) const
{
  return !(*this == matrix);
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::CheckIndex(int row, int column) const
{
  if (row >= m_Rows || column >= m_Columns)
    throw std::out_of_range("albaDynamicMatrix: index out of range");
}