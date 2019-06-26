/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: StartingPolynomialTest
 Authors: Roberto Mucci
 
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

#include <cppunit/config/SourcePrefix.h>
#include "StartingPolynomialTest.h"

#include "vtkALBAPoissonSurfaceReconstruction.h"

//-------------------------------------------------------------------------
void StartingPolynomialTest::TestAllMethods()
//-------------------------------------------------------------------------
{

/** scale operation*/
// StartingPolynomial<Degree> StartingPolynomial<Degree>::scale(const double& s) const

	StartingPolynomial<1> sp1;
  sp1.start = 50;
  StartingPolynomial<1> s1 = sp1.scale(2);
  CPPUNIT_ASSERT(s1.start == sp1.start*2);


/** shift operation*/
// StartingPolynomial<Degree> StartingPolynomial<Degree>::shift(const double& s) const

  StartingPolynomial<1> sp2(sp1);
  StartingPolynomial<1> s2 = sp2.shift(10);
  CPPUNIT_ASSERT(s2.start == sp2.start+10);
}

