/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafASCIIImporterUtility.cpp,v $
Language:  C++
Date:      $Date: 2006-06-08 14:09:10 $
Version:   $Revision: 1.2 $
Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafASCIIImporterUtility.h"

#include <vcl_fstream.h>
#include <vnl/vnl_vector.h>

//----------------------------------------------------------------------------
mafASCIIImporterUtility::mafASCIIImporterUtility()
//----------------------------------------------------------------------------
{
  m_NumOfRows = 0;
  m_NumOfCols = 0;
}
//----------------------------------------------------------------------------
mafASCIIImporterUtility::~mafASCIIImporterUtility()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
int mafASCIIImporterUtility::ReadFile(const char *fname)
//----------------------------------------------------------------------------
{
  //Read raw motion data
  vcl_ifstream v_raw_matrix(fname, std::ios::in);

	if(v_raw_matrix.is_open() != 0)
	{	
		m_Matrix.read_ascii(v_raw_matrix);
    m_NumOfRows = m_Matrix.rows();
    m_NumOfCols = m_Matrix.cols();
    return MAF_OK;
	}
 	return MAF_ERROR;
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtility::ExtractRow(unsigned r, std::vector<double> &row)
//----------------------------------------------------------------------------
{
  for (int c=0; c<m_NumOfCols;c++)
  {
    row.push_back(m_Matrix.get(r,c));
  }
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtility::ExtractRow(unsigned r, double *row)
//----------------------------------------------------------------------------
{
  vnl_vector<double> vec;
  vec = m_Matrix.get_row(r);
  vec.copy_out(row);
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtility::ExtractColumn(unsigned c, std::vector<double> &col)
//----------------------------------------------------------------------------
{
  for (int r=0; r<m_NumOfRows;r++)
  {
    col.push_back(m_Matrix.get(r,c));
  }
}
//----------------------------------------------------------------------------
void mafASCIIImporterUtility::ExtractColumn(unsigned c, double *col)
//----------------------------------------------------------------------------
{
  vnl_vector<double> vec;
  vec = m_Matrix.get_column(c);
  vec.copy_out(col);
}
