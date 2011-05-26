/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: CMatrixTestM2.cpp,v $
Language:  C++
Date:      $Date: 2011-05-26 08:33:31 $
Version:   $Revision: 1.1.2.2 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "CMatrixTestM2.h"

#include "vtkMEDPolyDataDeformation_M2.cpp"

//-------------------------------------------------------------------------
void CMatrixTestM2::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation_M2::CMatrix<double> *M1 = new vtkMEDPolyDataDeformation_M2::CMatrix<double>(2,2);
  delete M1;
  vtkMEDPolyDataDeformation_M2::CMatrix<int> *M2 = new vtkMEDPolyDataDeformation_M2::CMatrix<int>(2,2);
  delete M2;
}
//-------------------------------------------------------------------------
void CMatrixTestM2::TestStaticAllocation()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation_M2::CMatrix<double> M1(2,2);
  vtkMEDPolyDataDeformation_M2::CMatrix<int> M2(2,2);
}
//-------------------------------------------------------------------------
void CMatrixTestM2::TestGetNumberOfColumns()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation_M2::CMatrix<double> *M1 = new vtkMEDPolyDataDeformation_M2::CMatrix<double>(15,22);
  CPPUNIT_ASSERT(M1->GetNumberOfColumns() == 22);
  delete M1;

  vtkMEDPolyDataDeformation_M2::CMatrix<int> *M2 = new vtkMEDPolyDataDeformation_M2::CMatrix<int>(18,7);
  CPPUNIT_ASSERT(M2->GetNumberOfColumns() == 7);
  delete M2;
}
//-------------------------------------------------------------------------
void CMatrixTestM2::TestGetNumberOfRows()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation_M2::CMatrix<double> *M1 = new vtkMEDPolyDataDeformation_M2::CMatrix<double>(15,22);
  CPPUNIT_ASSERT(M1->GetNumberOfRows() == 15);
  delete M1;

  vtkMEDPolyDataDeformation_M2::CMatrix<int> *M2 = new vtkMEDPolyDataDeformation_M2::CMatrix<int>(18,7);
  CPPUNIT_ASSERT(M2->GetNumberOfRows() == 18);
  delete M2;
}
//-------------------------------------------------------------------------
void CMatrixTestM2::TestGetMinSize()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation_M2::CMatrix<double> *M1 = new vtkMEDPolyDataDeformation_M2::CMatrix<double>(15,22);
  CPPUNIT_ASSERT(M1->GetMinSize() == 15);
  delete M1;

  vtkMEDPolyDataDeformation_M2::CMatrix<int> *M2 = new vtkMEDPolyDataDeformation_M2::CMatrix<int>(18,7);
  CPPUNIT_ASSERT(M2->GetMinSize() == 7);
  delete M2;
}
//-------------------------------------------------------------------------
void CMatrixTestM2::TestOperator()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation_M2::CMatrix<double> *M1 = new vtkMEDPolyDataDeformation_M2::CMatrix<double>(15,22);
  (*M1)(0,1) = 5.0;
  CPPUNIT_ASSERT( (*M1)(0,1) == 5.0 );
  delete M1;
}
