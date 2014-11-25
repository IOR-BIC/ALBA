/*=========================================================================

 Program: MAF2Medical
 Module: PPolynomialTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "PPolynomialTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"
#include "vtkMEDPoissonSurfaceReconstruction.cxx"

//-------------------------------------------------------------------------
void PPolynomialTest::TestAllMethods()
//-------------------------------------------------------------------------
{

  // Create Polynomial used inside PPolynomial ////////
  Polynomial<1> p1;	
  double coeffAValue = 2;
  double coeffBValue = 4;

  enum COEFFICIENTS {A = 0, B = 1, C = 2};
  p1.coefficients[A] = coeffAValue;
  p1.coefficients[B] = coeffBValue;

  StartingPolynomial<1> sp1;
  sp1.start = 0;
  sp1.p = p1;
  ///////////////////////////////////////////////////


	PPolynomial<1> PPolynomialTestItem;	
  
  PPolynomialTestItem.set(1);
  CPPUNIT_ASSERT(PPolynomialTestItem.size() == 24);

  PPolynomialTestItem.reset(2);
  CPPUNIT_ASSERT(PPolynomialTestItem.size() == 48);

  // copy constructor test
  PPolynomial<1> PPolynomialTestItem2(PPolynomialTestItem);
  CPPUNIT_ASSERT(PPolynomialTestItem2.size() == 48);
  

  //integral test
  PPolynomialTestItem.set(&sp1, 1);
  CPPUNIT_ASSERT(PPolynomialTestItem.integral(0,1) == 4);

  //scale test
  PPolynomial<1> p3;
  p3 = PPolynomialTestItem.scale(2);
  CPPUNIT_ASSERT(p3.polys->p.coefficients[B] == 2);

  //shift test
  p3 = PPolynomialTestItem.shift(6);
  CPPUNIT_ASSERT(p3.polys->start == 6);
  CPPUNIT_ASSERT(p3.polys->p.coefficients[A] == -22.0);
  CPPUNIT_ASSERT(p3.polys->p.coefficients[B] == 4);
}

