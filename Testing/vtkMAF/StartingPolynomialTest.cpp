/*=========================================================================

 Program: MAF2Medical
 Module: StartingPolynomialTest
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
#include "StartingPolynomialTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"
#include "vtkMEDPoissonSurfaceReconstruction.cxx"

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

