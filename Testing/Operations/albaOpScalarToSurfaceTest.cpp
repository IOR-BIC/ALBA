/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpScalarToSurfaceTest
 Authors: Eleonora Mambrini
 
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
#include "albaOpScalarToSurfaceTest.h"

#include "albaOpScalarToSurface.h"
#include "albaVMEScalar.h"
#include "albaVMEGroup.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEImage.h"
#include "albaVMESurfaceParametric.h"
#include "albaVMEGizmo.h"
#include "albaVMERefSys.h"
#include "albaVMESlicer.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "albaVMEPolyline.h"
#include "albaVMESurface.h"

/*#include "vtkALBASmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"
#include "vtkCleanPolyData.h"
#include "vtkCell.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkDecimatePro.h"
#include "vtkTriangleFilter.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkStripper.h"
#include "vtkPolyDataNormals.h"*/

//----------------------------------------------------------------------------
void albaOpScalarToSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void albaOpScalarToSurfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  albaOpScalarToSurface op;
}
//----------------------------------------------------------------------------
void albaOpScalarToSurfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  albaOpScalarToSurface *op = new albaOpScalarToSurface();
  albaDEL(op);
}

//----------------------------------------------------------------------------
void albaOpScalarToSurfaceTest::TestAccept()
//----------------------------------------------------------------------------
{
  albaSmartPointer<albaVMEScalar> scalar;

  albaVMEScalar *animatedScalar;
  albaNEW(animatedScalar);

  for(int i=0;i<10;i++)
  {
    double scalarValue = i + 0.5;
    animatedScalar->SetData(scalarValue, i+0.0);
  }

  albaOpScalarToSurface *op = new albaOpScalarToSurface();

  CPPUNIT_ASSERT( !op->Accept(scalar) );
  CPPUNIT_ASSERT( op->Accept(animatedScalar) );

  albaSmartPointer<albaVMEVolumeGray> volume;
  albaSmartPointer<albaVMEGroup> group;
  albaSmartPointer<albaVMESlicer> slicer;
  albaSmartPointer<albaVMEPolyline> poly;
  albaSmartPointer<albaVMEImage> image;
  albaSmartPointer<albaVMERefSys> refSys;
  albaSmartPointer<albaVMESurface> surface;
  albaSmartPointer<albaVMEGizmo> gizmo;
  albaSmartPointer<albaVMELandmark> landmark;
  albaSmartPointer<albaVMELandmarkCloud> landmarkCloud;

  //Only surface is accepted
  CPPUNIT_ASSERT( !(op->Accept(landmark) && op->Accept(landmarkCloud) && op->Accept(gizmo) && op->Accept(surface) && op->Accept(volume) && op->Accept(group) && op->Accept(slicer) && op->Accept(poly) && op->Accept(image) && op->Accept(refSys)) );

  albaDEL(animatedScalar);
  albaDEL(op);
}

//----------------------------------------------------------------------------
void albaOpScalarToSurfaceTest::TestSurfaceBuilder()
//----------------------------------------------------------------------------
{
  albaOpScalarToSurface *op = new albaOpScalarToSurface();
  op->TestModeOn();

  albaVMEScalar *scalar;
  albaNEW(scalar);

  for(int i=0;i<10;i++)
  {
    double scalarValue = i + 0.5;
    scalar->SetData(scalarValue, i+0.5);
  }

  scalar->GetOutput()->GetVTKData();

  op->SetInput(scalar);
  op->OpRun();
  op->OpDo();

  CPPUNIT_ASSERT(albaVMESurface::SafeDownCast(op->GetOutput()));

  albaDEL(scalar);
  albaDEL(op);
}