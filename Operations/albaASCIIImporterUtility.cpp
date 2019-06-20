/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaASCIIImporterUtility
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaASCIIImporterUtility.h"

#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>

//----------------------------------------------------------------------------
albaASCIIImporterUtility::albaASCIIImporterUtility()
//----------------------------------------------------------------------------
{
  m_NumOfRows = 0;
  m_NumOfCols = 0;
}
//----------------------------------------------------------------------------
albaASCIIImporterUtility::~albaASCIIImporterUtility()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int albaASCIIImporterUtility::ReadFile(const char *fname)
//----------------------------------------------------------------------------
{
  //Read raw motion data
  vcl_ifstream v_raw_matrix(fname, std::ios::in);

	if(v_raw_matrix.is_open() != 0)
	{	
		m_Matrix.read_ascii(v_raw_matrix);
    m_NumOfRows = m_Matrix.rows();
    m_NumOfCols = m_Matrix.cols();
    return ALBA_OK;
	}
 	return ALBA_ERROR;
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtility::ExtractRow(unsigned r, std::vector<double> &row)
//----------------------------------------------------------------------------
{
  for (int c=0; c<m_NumOfCols;c++)
  {
    row.push_back(m_Matrix.get(r,c));
  }
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtility::ExtractRow(unsigned r, double *row)
//----------------------------------------------------------------------------
{
  vnl_vector<double> vec;
  vec = m_Matrix.get_row(r);
  vec.copy_out(row);
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtility::ExtractColumn(unsigned c, std::vector<double> &col)
//----------------------------------------------------------------------------
{
  for (int r=0; r<m_NumOfRows;r++)
  {
    col.push_back(m_Matrix.get(r,c));
  }
}
//----------------------------------------------------------------------------
void albaASCIIImporterUtility::ExtractColumn(unsigned c, double *col)
//----------------------------------------------------------------------------
{
  vnl_vector<double> vec;
  vec = m_Matrix.get_column(c);
  vec.copy_out(col);
}
