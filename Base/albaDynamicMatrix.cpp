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
int albaDynamicMatrix::GetRowsNum() const
{
  return m_Rows;
}

//----------------------------------------------------------------------------
int albaDynamicMatrix::GetColNum() const
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

//----------------------------------------------------------------------------
void albaDynamicMatrix::AddRow(double *values)
{
	if (values == NULL)
		throw std::invalid_argument("albaDynamicMatrix: invalid row data");

	m_Data.insert(m_Data.end(), values, values + m_Columns);
	++m_Rows;
}

//----------------------------------------------------------------------------
int albaDynamicMatrix::ReadFromFile(albaString filename)
{
  Resize(0, 0);

  if (ReadInit(filename, true, false, "", NULL) == ALBA_ERROR)
  {
    albaLogMessage("Cannot Open: %s", filename);
    ReadFinalize();
    return ALBA_ERROR;
  }
	bool headerReaded = false;
  int lineLenght;
	unsigned int charsReaded = 0, totCharReaded = 0;
  float tmpValue;

	// Skip header lines
  while (!headerReaded && (lineLenght = GetLine(true)) != 0)
  {
		// if I can read a float value from the line, then I have skipped the header
    if (sscanf(m_Line,"%f%n",&tmpValue,&charsReaded) == 1)
    {
      headerReaded = true;
			m_Columns = 1;
      while (sscanf(m_Line + totCharReaded, "%f%n", &tmpValue, &charsReaded) == 1)
      {
        totCharReaded += charsReaded;
        ++m_Columns;
			}
    }
  }

	double *rowData = new double[m_Columns];

  while ((lineLenght = GetLine(true)) != 0)
  {
    totCharReaded = 0;
    for (int i = 0; i < m_Columns; ++i)
    {
      if (sscanf(m_Line + totCharReaded, "%lf%n", &rowData[i], &charsReaded) != 1)
      {
        albaLogMessage("Wrong Column number on line %d", m_CurrentLine);
        delete[] rowData;
        ReadFinalize();
        return ALBA_ERROR;
      }
      totCharReaded += charsReaded;
    }
    AddRow(rowData);
  }
  
  delete[] rowData;
  ReadFinalize();

	return ALBA_OK;
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::ExtractRow(int rowNum, std::vector<double> &row)
{
	if (rowNum < 0 || rowNum >= m_Rows)
		throw std::out_of_range("albaDynamicMatrix: row index out of range");

	row.assign(m_Data.begin() + rowNum * m_Columns, m_Data.begin() + (rowNum + 1) * m_Columns);
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::ExtractRow(int rowNum, double *row)
{
	if (rowNum < 0 || rowNum >= m_Rows)
		throw std::out_of_range("albaDynamicMatrix: row index out of range");

	if (row == NULL)
		throw std::invalid_argument("albaDynamicMatrix: invalid row data");

	for (int column = 0; column < m_Columns; ++column)
		row[column] = m_Data[rowNum * m_Columns + column];
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::ExtractColumn(int colNum, std::vector<double> &col)
{
	if (colNum < 0 || colNum >= m_Columns)
		throw std::out_of_range("albaDynamicMatrix: column index out of range");

	col.resize(m_Rows);

	for (int row = 0; row < m_Rows; ++row)
		col[row] = m_Data[row * m_Columns + colNum];
}

//----------------------------------------------------------------------------
void albaDynamicMatrix::ExtractColumn(int colNum, double *col)
{
	if (colNum < 0 || colNum >= m_Columns)
		throw std::out_of_range("albaDynamicMatrix: column index out of range");

	if (col == NULL)
		throw std::invalid_argument("albaDynamicMatrix: invalid column data");

	for (int row = 0; row < m_Rows; ++row)
		col[row] = m_Data[row * m_Columns + colNum];
}