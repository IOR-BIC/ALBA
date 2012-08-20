/*=========================================================================

 Program: MAF2Medical
 Module: medOpMatrixVectorMathTest
 Authors: Daniele Cgiunchi
 
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

#include "medOpMatrixVectorMathTest.h"
#include "medOpMatrixVectorMath.h"

#include "mafVMESurface.h"
#include "mafVMESurfaceParametric.h"
#include "mafSmartPointer.h"

#include "vtkDataSet.h"
#include "vtkMath.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
  cppDEL(ovm);
}
/*/-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestSetHomogeneous() 
//-----------------------------------------------------------
{

}*/
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestDivideVectorByHomoCoord() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 2.0, 4.0, 2.0};

  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->DivideVectorByHomoCoord(a);

	double result[4] = {0.0, 1.0, 2.0, 1.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],result[i]);
    CPPUNIT_ASSERT(a[i] == result[i]);
	}
	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestSetVectorToZero() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 2.0, 4.0, 2.0};

  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
  ovm->SetVectorToZero(a);

	double result[4] = {0.0, 0.0, 0.0, 2.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],result[i]);
		CPPUNIT_ASSERT(a[i] == result[i]);
	}

	cppDEL(ovm);

}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestMagnitudeOfVector() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	double result = ovm->MagnitudeOfVector(a);

  CPPUNIT_ASSERT(result = 5.);

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestNormalizeVector1() 
//-----------------------------------------------------------
{
	double a[4] = {2.0, 3.0, 4.0, 1.0};
	double res[4] = {0.,0.,0.,1.};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->NormalizeVector(a,res);

	vtkMath::Normalize(a);

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],res[i]);
		CPPUNIT_ASSERT(a[i] == res[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestNormalizeVector2() 
//-----------------------------------------------------------
{
	double a1[4] = {2.0, 3.0, 4.0, 1.0};
	double a2[4] = {2.0, 3.0, 4.0, 1.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->NormalizeVector(a1);

	vtkMath::Normalize(a2);

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a1[i],a2[i]);
		CPPUNIT_ASSERT(a1[i] == a2[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestInvertVector1() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double res[4] = {0.,0.,0.,1.};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->InvertVector(a,res);

	double aInv[4] = {0.0, -3.0, -4.0, 1.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",-a[i],res[i]);
		CPPUNIT_ASSERT(aInv[i] == res[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestInvertVector2() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->InvertVector(a);
	
	double result[4] = {0.,-3.,-4.,1.};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],result[i]);
		CPPUNIT_ASSERT(a[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestMultiplyVectorByScalar() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double res[4] = {0.0, 0.0, 0.0, 1.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->MultiplyVectorByScalar(3., a, res);

	double result[4] = {0.0, 9.0, 12.0, 1.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",res[i],result[i]);
		CPPUNIT_ASSERT(res[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestDivideVectorByScalar() 
//-----------------------------------------------------------
{
	double a[4] = {2.0, 6.0, 4.0, 1.0};
	double res[4] = {0.0, 0.0, 0.0, 1.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->DivideVectorByScalar(2., a, res);

	double result[4] = {1.0, 3.0, 2.0, 1.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",res[i],result[i]);
		CPPUNIT_ASSERT(res[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestAddVectors() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double b[4] = {2.0, 1.0, 3.0, 1.0};
	double c[4] = {0.0, 0.0, 0.0, 1.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->AddVectors(a,b,c);

	double result[4] = {2.0, 4.0, 7.0, 1.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",c[i],result[i]);
		CPPUNIT_ASSERT(c[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestSubtractVectors() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double b[4] = {2.0, 1.0, 3.0, 1.0};
	double c[4] = {0.0, 0.0, 0.0, 1.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SubtractVectors(a,b,c);

	double result[4] = {-2.0, 2.0, 1.0, 1.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",c[i],result[i]);
		CPPUNIT_ASSERT(c[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestAddMultipleOfVector() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double b[4] = {2.0, 1.0, 3.0, 1.0};
	double c[4] = {0.0, 0.0, 0.0, 1.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->AddMultipleOfVector(a,2.,b,c);

	double result[4] = {4.0, 5.0, 10.0, 1.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",c[i],result[i]);
		CPPUNIT_ASSERT(c[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestSubtractMultipleOfVector() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double b[4] = {2.0, 1.0, 3.0, 1.0};
	double c[4] = {0.0, 0.0, 0.0, 1.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SubtractMultipleOfVector(a,-2.,b,c);

	double result[4] = {4.0, 5.0, 10.0, 1.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",c[i],result[i]);
		CPPUNIT_ASSERT(c[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestDotProduct() 
//-----------------------------------------------------------
{
	double a[3] = {0.0, 3.0, 4.0};
	double b[3] = {1.0, 2.0, 3.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	double res = ovm->DotProduct(a,b);

	double result = vtkMath::Dot(a,b);

	
  //printf("\n%f - %f\n",res, result);
  CPPUNIT_ASSERT(res == result);
	

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestVectorProduct() 
//-----------------------------------------------------------
{
	double a[3] = {0.0, 3.0, 4.0};
	double b[3] = {1.0, 2.0, 3.0};
	double c[3] = {0.0, 0.0, 0.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->VectorProduct(a,b,c);

	double result[3];
	vtkMath::Cross(a,b, result);

	int i=0, size=3;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",c[i], result[i]);
		CPPUNIT_ASSERT(c[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestCopyVector() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double res[4] = {0.0, 0.0, 0.0, 1.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->CopyVector(a,res);

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],res[i]);
		CPPUNIT_ASSERT(a[i] == res[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestEquals() 
//-----------------------------------------------------------
{
	double a[4] = {0.1, 3.1, 4.1, 1.0};
	double res[4] = {0.0, 3.0, 4.0, 1.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	bool result = ovm->Equals(a,res,0.2);

  CPPUNIT_ASSERT(result);
	
	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestCalculateNormalsToU() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double u[4] = {0.0, 0.0, 0.0, 1.0};
	double w[4] = {0.0, 0.0, 0.0, 1.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->CalculateNormalsToU(a,u,w);

	double dotAU = vtkMath::Dot(a,u);
	double dotAW = vtkMath::Dot(a,w);
	double dotUW = vtkMath::Dot(u,w);

	
  CPPUNIT_ASSERT(dotAU == 0. && dotAW == 0. && dotUW == 0.);
	

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestCalculateNormalsToV() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double u[4] = {0.0, 0.0, 0.0, 1.0};
	double w[4] = {0.0, 0.0, 0.0, 1.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->CalculateNormalsToV(u,a,w);

	double dotAU = vtkMath::Dot(a,u);
	double dotAW = vtkMath::Dot(a,w);
	double dotUW = vtkMath::Dot(u,w);


	CPPUNIT_ASSERT(dotAU == 0. && dotAW == 0. && dotUW == 0.);


	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestCalculateNormalsToW() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
	double u[4] = {0.0, 0.0, 0.0, 1.0};
	double w[4] = {0.0, 0.0, 0.0, 1.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->CalculateNormalsToV(u,w,a);

	double dotAU = vtkMath::Dot(a,u);
	double dotAW = vtkMath::Dot(a,w);
	double dotUW = vtkMath::Dot(u,w);

	CPPUNIT_ASSERT(dotAU == 0. && dotAW == 0. && dotUW == 0.);

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestDistance() 
//-----------------------------------------------------------
{
	double a[3] = {0.0, 3.0, 4.0};
	double b[3] = {1.0, 2.0, 3.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	double distance = ovm->Distance(a,b);

	double result = sqrt(vtkMath::Distance2BetweenPoints(a,b));

	CPPUNIT_ASSERT(distance == result);
	

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestDistanceSquared() 
//-----------------------------------------------------------
{
	double a[3] = {0.0, 3.0, 4.0};
	double b[3] = {1.0, 2.0, 3.0};
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	double distance = ovm->DistanceSquared(a,b);

	double result = vtkMath::Distance2BetweenPoints(a,b);

	CPPUNIT_ASSERT(distance == result);

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestPrintVector() 
//-----------------------------------------------------------
{
	double a[4] = {0.0, 3.0, 4.0, 1.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->PrintVector(std::cout, a);

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestDivideMatrixByHomoCoord() 
//-----------------------------------------------------------
{
	double a[16] = {0.0, 2.0, 4.0, 8.0,\
		10.0, 12.0, 14.0, 16.0,\
		18.0, 20.0, 22.0, 24.0,\
		26.0, 28.0, 30.0, 2.0};

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->DivideMatrixByHomoCoord(a);

	double result[16] = {0.0, 1.0, 2.0, 4.0,\
		5.0, 6.0, 7.0, 8.0,\
		9.0, 10.0, 11.0, 12.0,\
		13.0, 14.0, 15.0, 1.0};

	int i=0, size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],result[i]);
		CPPUNIT_ASSERT(a[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestSetMatrixToZero() 
//-----------------------------------------------------------
{
	double a[16] = {0.0, 2.0, 4.0, 8.0,\
		10.0, 12.0, 14.0, 16.0,\
		18.0, 20.0, 22.0, 24.0,\
		26.0, 28.0, 30.0, 2.0};

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->SetMatrixToZero(a);

	double result[16] = {0.0, 0.0, 0.0, 0.0,\
		0.0, 0.0, 0.0, 0.0,\
		0.0, 0.0, 0.0, 0.0,\
		0.0, 0.0, 0.0, 1.0};

	int i=0, size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],result[i]);
		CPPUNIT_ASSERT(a[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestSetMatrixToIdentity() 
//-----------------------------------------------------------
{
	double a[16] = {0.0, 2.0, 4.0, 8.0,\
		10.0, 12.0, 14.0, 16.0,\
		18.0, 20.0, 22.0, 24.0,\
		26.0, 28.0, 30.0, 2.0};

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->SetMatrixToIdentity(a);

	double result[16] = {1.0, 0.0, 0.0, 0.0,\
		0.0, 1.0, 0.0, 0.0,\
		0.0, 0.0, 1.0, 0.0,\
		0.0, 0.0, 0.0, 1.0};

	int i=0, size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],result[i]);
		CPPUNIT_ASSERT(a[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestMultiplyMatrixByScalar() 
//-----------------------------------------------------------
{
	double a[16] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0, 2.0, 2.0, 6.0,\
		0.0, 4.0, 1.0, 1.0};

	double b[16];

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->MultiplyMatrixByScalar(.5, a,b);

	double result[16] = {0.0, 1.0, 2.0, 4.0,\
		1.5, 2.5, 3.0, 4.5,\
		0.5, 1.0, 1.0, 3.0,\
		0.0, 2.0, 0.5, 1.0};

	int i=0, size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",b[i],result[i]);
		CPPUNIT_ASSERT(b[i] == result[i]);
	}

	cppDEL(ovm);
} 
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestDivideMatrixByScalar() 
//-----------------------------------------------------------
{
	double a[16] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0, 2.0, 2.0, 6.0,\
		0.0, 4.0, 1.0, 1.0};

	double b[16];

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->DivideMatrixByScalar(2., a,b);

	double result[16] = {0.0, 1.0, 2.0, 4.0,\
		1.5, 2.5, 3.0, 4.5,\
		0.5, 1.0, 1.0, 3.0,\
		0.0, 2.0, 0.5, 1.0};

	int i=0, size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",b[i],result[i]);
		CPPUNIT_ASSERT(b[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestMultiplyColumnsByScalars() 
//-----------------------------------------------------------
{
	double scalars[4] = {0.,1.,2.,3.};
	double a[16] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0, 2.0, 2.0, 6.0,\
		0.0, 4.0, 1.0, 1.0};

	double b[16];

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->MultiplyColumnsByScalars(scalars,a,b);


	double result[16] = {0.0, 0.0, 0.0, 0.0,\
		3.0, 5.0, 6.0, 9.0,\
		2.0, 4.0, 4.0, 12.0,\
		0.0, 12.0, 3.0, 3.0};

	int i=0, size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",b[i],result[i]);
		CPPUNIT_ASSERT(b[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestMultiplyRowsByScalars() 
//-----------------------------------------------------------
{
	double scalars[4] = {0.,1.,2.,3.};
	double a[16] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0, 2.0, 2.0, 6.0,\
		0.0, 4.0, 1.0, 1.0};

	double b[16];

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->MultiplyRowsByScalars(scalars,a,b);

	double result[16] = {0.0, 2.0, 8.0, 24.0,\
	0.0, 5.0, 12.0, 27.0,\
	0.0, 2.0, 4.0, 18.0,\
	0.0, 4.0, 2.0, 3.0};

	int i=0, size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",b[i],result[i]);
		CPPUNIT_ASSERT(b[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestMultiplyMatrixByVector() 
//-----------------------------------------------------------
{
	double v[4] = {0.0, 3.0, 4.0, 1.0};
	double A[16] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0, 2.0, 2.0, 6.0,\
		0.0, 4.0, 1.0, 1.0};
	double Av[4];
	double result[4];
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->MultiplyMatrixByVector(A,v,Av);

	result[0] = A[0]*v[0] + A[4]*v[1] + A[8]*v[2] + A[12]*v[3] ;
	result[1] = A[1]*v[0] + A[5]*v[1] + A[9]*v[2] + A[13]*v[3] ;
	result[2] = A[2]*v[0] + A[6]*v[1] + A[10]*v[2] + A[14]*v[3] ;
	result[3] = A[3]*v[0] + A[7]*v[1] + A[11]*v[2] + A[15]*v[3] ;

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",result[i],Av[i]);
		CPPUNIT_ASSERT(result[i] == Av[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestMultiplyMatrixByMatrix() 
//-----------------------------------------------------------
{
	double A[9] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0,};

	double B[9] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0};

	double C[9];
	double result[9];

  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->MultiplyMatrixByMatrix(A,B,C);

	result[0] = A[0]*B[0] + A[3]*B[1] + A[6]*B[2] ;
	result[1] = A[1]*B[0] + A[4]*B[1] + A[7]*B[2] ;
	result[2] = A[2]*B[0] + A[5]*B[1] + A[8]*B[2] ;

	result[3] = A[0]*B[3] + A[3]*B[4] + A[6]*B[5] ;
	result[4] = A[1]*B[3] + A[4]*B[4] + A[7]*B[5] ;
	result[5] = A[2]*B[3] + A[5]*B[4] + A[8]*B[5] ;

	result[6] = A[0]*B[6] + A[3]*B[7] + A[6]*B[8] ;
	result[7] = A[1]*B[6] + A[4]*B[7] + A[7]*B[8] ;
	result[8] = A[2]*B[6] + A[5]*B[7] + A[8]*B[8] ;

	int i=0, size=9;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",a[i],result[i]);
		CPPUNIT_ASSERT(C[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestTranspose() 
//-----------------------------------------------------------
{
	double A[16] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0, 2.0, 2.0, 6.0,\
		0.0, 4.0, 1.0, 1.0};
  double T[16];
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	ovm->Transpose(A, T);

	double result[16] = {0.0, 3.0, 1.0, 0.0,\
		2.0, 5.0, 2.0, 4.0,\
		4.0, 6.0, 2.0, 1.0,\
		8.0, 9.0, 6.0, 1.0};

	int i=0, size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",T[i],result[i]);
		CPPUNIT_ASSERT(T[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestGetColumn() 
//-----------------------------------------------------------
{
	double A[16] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0, 2.0, 2.0, 6.0,\
		0.0, 4.0, 1.0, 1.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->SetHomogeneous(true);
	double *column = ovm->GetColumn(A, 0);

	double result[4] = {0.0, 2.0, 4.0, 8.0};

	int i=0, size=4;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",column[i],result[i]);
		CPPUNIT_ASSERT(column[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestPrintMatrix() 
//-----------------------------------------------------------
{
	double A[16] = {0.0, 2.0, 4.0, 8.0,\
		3.0, 5.0, 6.0, 9.0,\
		1.0, 2.0, 2.0, 6.0,\
		0.0, 4.0, 1.0, 1.0};
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->PrintMatrix(std::cout, A);

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestTranspose2DArray() 
//-----------------------------------------------------------
{
	double A[3][3] = { {0.0, 2.0, 4.0},
	                   {1.0, 3.0, 5.0},
	                   {0.1, 0.2, 0.3}};
	double B[3][3];
  medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->Transpose2DArray(A,B); 
	double result[3][3];
	vtkMath::Transpose3x3(A, result);

	
  int i=0,sizeI = 3;
	int j=0,sizeJ = 3;
	for(;i<sizeI;i++)
	{
		j=0;
		for(;j<sizeJ;j++)
		{
			//printf("\n%f - %f\n",B[i][j],result[i][j]);
			CPPUNIT_ASSERT(B[i][j] == result[i][j]);
		}
	}
	

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestCopy2DArrayToMatrix3x3() 
//-----------------------------------------------------------
{
	double array2D[3][3] = { {0.0, 2.0, 4.0},
	{1.0, 3.0, 5.0},
	{0.1, 0.2, 0.3}};

	double A[9];
	
	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->Copy2DArrayToMatrix3x3(array2D, A);

  double result[9] = {0.0, 1.0,0.1,2.0,3.0,0.2,4.0,5.0,0.3};
	
	int i=0,size=9;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",A[i],result[i]);
    CPPUNIT_ASSERT(A[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestCopy2DArrayToMatrix4x4() 
//-----------------------------------------------------------
{
	double array2D[4][4] = { {0.0, 2.0, 4.0, 6.0},
	{1.0, 3.0, 5.0, 7.0},
	{0.1, 0.2, 0.3, 0.4}, 
	{0.01, 0.02, 0.03, 0.04}};

	double A[16];

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->Copy2DArrayToMatrix4x4(array2D, A);

	double result[16] = {0.0,1.0,0.1,0.01,2.0,3.0,0.2,0.02,4.0,5.0,0.3,0.03,6.0,7.0,0.4,0.04};

	int i=0,size=16;
	for(;i<size;i++)
	{
		//printf("\n%f - %f\n",A[i],result[i]);
		CPPUNIT_ASSERT(A[i] == result[i]);
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestCopyMatrixTo2DArray3x3() 
//-----------------------------------------------------------
{
  double A[9] = {0.0, 1.0,0.1,2.0,3.0,0.2,4.0,5.0,0.3};
	double B[3][3];

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->CopyMatrixTo2DArray3x3(A, B);

	double result[3][3] = { {0.0, 2.0, 4.0},
	{1.0, 3.0, 5.0},
	{0.1, 0.2, 0.3}};

	
	int i=0,sizeI=3;
	int j=0,sizeJ=3;
	for(;i<sizeI;i++)
	{
		j = 0;
		for(;j<sizeJ;j++)
		{
			//printf("\n%f - %f\n",B[i][j],result[i][j]);
		  CPPUNIT_ASSERT(B[i][j] == result[i][j]);
		}
	}

	cppDEL(ovm);
}
//-----------------------------------------------------------
void medOpMatrixVectorMathTest::TestCopyMatrixTo2DArray4x4() 
//-----------------------------------------------------------
{
	double A[16] = {0.0,1.0,0.1,0.01,2.0,3.0,0.2,0.02,4.0,5.0,0.3,0.03,6.0,7.0,0.4,0.04};
	double B[4][4];

	medOpMatrixVectorMath *ovm=new medOpMatrixVectorMath();
	ovm->CopyMatrixTo2DArray4x4(A, B);

	double result[4][4] = { {0.0, 2.0, 4.0, 6.0},
	{1.0, 3.0, 5.0, 7.0},
	{0.1, 0.2, 0.3, 0.4}, 
	{0.01, 0.02, 0.03, 0.04}};


	int i=0,sizeI=4;
	int j=0,sizeJ=4;
	for(;i<sizeI;i++)
	{
		j = 0;
		for(;j<sizeJ;j++)
		{
			//printf("\n%f - %f\n",B[i][j],result[i][j]);
			CPPUNIT_ASSERT(B[i][j] == result[i][j]);
		}
	}

	cppDEL(ovm);
}
