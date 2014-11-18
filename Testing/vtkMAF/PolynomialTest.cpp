/*=========================================================================

 Program: MAF2Medical
 Module: PolynomialTest
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include <cppunit/config/SourcePrefix.h>
#include "PolynomialTest.h"

#include "vtkMEDPoissonSurfaceReconstruction.h"
#include "vtkMEDPoissonSurfaceReconstruction.cxx"

//-------------------------------------------------------------------------
void PolynomialTest::TestAllMethods()
//-------------------------------------------------------------------------
{
	// a +bx = 0
	// a = 2
	// b = 4x
	Polynomial<1> p1;	
	double coeffAValue = 2;
	double coeffBValue = 4;

	enum COEFFICIENTS {A = 0, B = 1, C = 2};
	p1.coefficients[A] = coeffAValue;
	p1.coefficients[B] = coeffBValue;

// 	/** copy constructor */
// 	template<int Degree2>
// 		Polynomial(const Polynomial<Degree2>& P);

	Polynomial<1> p2(p1);
	CPPUNIT_ASSERT(p2.coefficients[A] = coeffAValue);
	CPPUNIT_ASSERT(p2.coefficients[B] = coeffBValue);

	// 	/** overload operator () which retrieves  the sum of the product of the coefficients*/
	// 	double operator()(const double& t) const;

	double tmp = p2(A);
	CPPUNIT_ASSERT(tmp == 2); // A

	tmp = p2(B);
	CPPUNIT_ASSERT(tmp == 6);  // A + B

	// 	/** calculate Integral */
	// 	double Integral(const double& tMin,const double& tMax) const;

	CPPUNIT_ASSERT(p1.Integral(0,1) == 4);

	/** operator== overload , checking coefficients */
	// int operator == (const Polynomial& p) const;

	CPPUNIT_ASSERT(p1 == p2);

	// 	/** operator!= overload, checking coefficients */
	// 	int operator != (const Polynomial& p) const;


	CPPUNIT_ASSERT((p1 != p2) == false);

	// 	/** check if all coefficients are zero*/
	// 	int IsZero(void) const;

	CPPUNIT_ASSERT(p1.IsZero() == false);

	// 	/** set  all coefficients as zero*/
	// 	void setZero(void);

	p1.coefficients[A] = 0;
	p1.coefficients[B] = 0;

	CPPUNIT_ASSERT(p1.IsZero() == true);

	p1.coefficients[A] = coeffAValue;
	p1.coefficients[B] = coeffBValue;

	CPPUNIT_ASSERT(p1.IsZero() == false);

// 	/** overload operator, according to the operation over coefficients */
// 	template<int Degree2>
// 		Polynomial& operator  = (const Polynomial<Degree2> &p);

	p1.coefficients[A] = 0;
	p1.coefficients[B] = 0;

	CPPUNIT_ASSERT(p1.IsZero() == true);

	p1 = p2;

	CPPUNIT_ASSERT(p1.coefficients[A] == coeffAValue);
	CPPUNIT_ASSERT(p1.coefficients[B] = coeffBValue);

// 	/** overload operator, according to the operation over coefficients */
// 	Polynomial& operator += (const Polynomial& p);

	Polynomial<1> p3;

	p3 += p2;

	CPPUNIT_ASSERT(p3.coefficients[A] == coeffAValue);
	CPPUNIT_ASSERT(p3.coefficients[B] == coeffBValue);

// 	/** overload operator, according to the operation over coefficients */
// 	Polynomial& operator -= (const Polynomial& p);


	CPPUNIT_ASSERT((p1 -= p2).IsZero() == true);

// 	/** overload operator, according to the operation over coefficients */
// 	Polynomial  operator -  (void) const;

	p1.coefficients[A] = coeffAValue;
	p1.coefficients[B] = coeffBValue;

	p2 = p1;

	p3 = p2 - p1;

	CPPUNIT_ASSERT(!p1.IsZero());
	CPPUNIT_ASSERT(!p2.IsZero());
	CPPUNIT_ASSERT(p3.IsZero());

// 	/** overload operator, according to the operation over coefficients */
// 	Polynomial  operator +  (const Polynomial& p) const;

	p3 = p1 + p2;

	CPPUNIT_ASSERT(p3.coefficients[A] == 2 * coeffAValue);
	CPPUNIT_ASSERT(p3.coefficients[B] == 2 * coeffBValue);

	// 	/** overload operator, according to the operation over coefficients */
// 	Polynomial  operator -  (const Polynomial& p) const;

	CPPUNIT_ASSERT((p2 - p1).IsZero());

// 	/** overload operator, according to the operation over coefficients */
// 	template<int Degree2>
// 		Polynomial<Degree+Degree2>  operator *  (const Polynomial<Degree2>& p) const;

	Polynomial<2> checkProduct;
	checkProduct.coefficients[A] = 4;
	checkProduct.coefficients[B] = 16;
	checkProduct.coefficients[C] = 16;

	Polynomial<2> product; // 4 + 16 * x + 16 * x^2 
	product = p1 * p2;

	CPPUNIT_ASSERT(product == checkProduct);

// 	/** calculate Derivative */
// 	Polynomial<Degree-1> Derivative(void) const;

	Polynomial<1> d = product.Derivative();

	CPPUNIT_ASSERT(d.coefficients[A] == 16);
	CPPUNIT_ASSERT(d.coefficients[B] = 32);
		
// 
// 	/** calculate Integral */
// 	Polynomial<Degree+1> Integral(void) const;
// 

	Polynomial<2> i = d.Integral();
	i.coefficients[A] = 0; // constant coefficient is 0
	i.coefficients[B] = 16;
	i.coefficients[C] = 16;

}

