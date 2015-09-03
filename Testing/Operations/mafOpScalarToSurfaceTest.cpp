/*=========================================================================

 Program: MAF2
 Module: mafOpScalarToSurfaceTest
 Authors: Eleonora Mambrini
 
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
#include "mafOpScalarToSurfaceTest.h"

#include "mafOpScalarToSurface.h"
#include "mafVMEScalar.h"
#include "mafVMEGroup.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEImage.h"
#include "mafVMESurfaceParametric.h"
#include "mafVMEGizmo.h"
#include "mafVMERefSys.h"
#include "mafVMESlicer.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafVMEPolyline.h"
#include "mafVMESurface.h"

/*#include "vtkMAFSmartPointer.h"
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
void mafOpScalarToSurfaceTest::TestFixture()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void mafOpScalarToSurfaceTest::TestStaticAllocation()
//----------------------------------------------------------------------------
{
  mafOpScalarToSurface op;
}
//----------------------------------------------------------------------------
void mafOpScalarToSurfaceTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  mafOpScalarToSurface *op = new mafOpScalarToSurface();
  mafDEL(op);
}

//----------------------------------------------------------------------------
void mafOpScalarToSurfaceTest::TestAccept()
//----------------------------------------------------------------------------
{
  mafSmartPointer<mafVMEScalar> scalar;

  mafVMEScalar *animatedScalar;
  mafNEW(animatedScalar);

  for(int i=0;i<10;i++)
  {
    double scalarValue = i + 0.5;
    animatedScalar->SetData(scalarValue, i+0.0);
  }

  mafOpScalarToSurface *op = new mafOpScalarToSurface();

  CPPUNIT_ASSERT( !op->Accept(scalar) );
  CPPUNIT_ASSERT( op->Accept(animatedScalar) );

  mafSmartPointer<mafVMEVolumeGray> volume;
  mafSmartPointer<mafVMEGroup> group;
  mafSmartPointer<mafVMESlicer> slicer;
  mafSmartPointer<mafVMEPolyline> poly;
  mafSmartPointer<mafVMEImage> image;
  mafSmartPointer<mafVMERefSys> refSys;
  mafSmartPointer<mafVMESurface> surface;
  mafSmartPointer<mafVMEGizmo> gizmo;
  mafSmartPointer<mafVMELandmark> landmark;
  mafSmartPointer<mafVMELandmarkCloud> landmarkCloud;

  //Only surface is accepted
  CPPUNIT_ASSERT( !(op->Accept(landmark) && op->Accept(landmarkCloud) && op->Accept(gizmo) && op->Accept(surface) && op->Accept(volume) && op->Accept(group) && op->Accept(slicer) && op->Accept(poly) && op->Accept(image) && op->Accept(refSys)) );

  mafDEL(animatedScalar);
  mafDEL(op);
}

//----------------------------------------------------------------------------
void mafOpScalarToSurfaceTest::TestSurfaceBuilder()
//----------------------------------------------------------------------------
{
  mafOpScalarToSurface *op = new mafOpScalarToSurface();
  op->TestModeOn();

  mafVMEScalar *scalar;
  mafNEW(scalar);

  for(int i=0;i<10;i++)
  {
    double scalarValue = i + 0.5;
    scalar->SetData(scalarValue, i+0.5);
  }

  scalar->GetOutput()->GetVTKData();

  op->SetInput(scalar);
  op->OpRun();
  op->OpDo();

  CPPUNIT_ASSERT(mafVMESurface::SafeDownCast(op->GetOutput()));

  mafDEL(scalar);
  mafDEL(op);
}