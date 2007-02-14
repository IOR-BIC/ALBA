/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEAdvancedProberTest.cpp,v $
Language:  C++
Date:      $Date: 2007-02-14 13:56:34 $
Version:   $Revision: 1.3 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2002/2004 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEAdvancedProberTest.h"
#include "mafVMEAdvancedProber.h"
#include "mafVMEVolumeGray.h"
#include "mafVMEPolylineSpline.h"
#include "mafVMEPolyline.h"
#include "mafVMERoot.h"
#include "mafVMEStorage.h"
#include "mmoVTKImporter.h"
#include "mafString.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"


void mafVMEAdvancedProberTest::Test()
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mmoVTKImporter *importer_volume=new mmoVTKImporter("importer");
	mafString filename_volume=MED_DATA_ROOT;
	filename_volume<<"/VTK_Volumes/Volume.vtk";
	importer_volume->TestModeOn();
	importer_volume->SetFileName(filename_volume);
	importer_volume->SetInput(storage->GetRoot());
	importer_volume->ImportVTK();
	mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(importer_volume->GetOutput());
	volume->GetOutput()->GetVTKData()->Update();
	volume->SetParent(storage->GetRoot());
	volume->Update();

	mmoVTKImporter *importer_polyline=new mmoVTKImporter("importer");
	mafString filename_polyline=MED_DATA_ROOT;
	filename_polyline<<"/Polyline/Polyline.vtk";
	importer_polyline->TestModeOn();
	importer_polyline->SetInput(storage->GetRoot());
	importer_polyline->SetFileName(filename_polyline);
	importer_polyline->ImportVTK();
	mafVMEPolyline *polyline=mafVMEPolyline::SafeDownCast(importer_polyline->GetOutput());
	polyline->GetOutput()->GetVTKData()->Update();
	polyline->SetParent(storage->GetRoot());
	polyline->Update();

	mafVMEPolylineSpline *spline;
	mafNEW(spline);
	spline->SetPolylineLink(mafNode::SafeDownCast(polyline));
	spline->GetOutput()->GetVTKData()->Update();
	spline->SetParent(storage->GetRoot());
	spline->Modified();

	mafVMEAdvancedProber *advProber;
	mafNEW(advProber);
	
  if(volume->GetMaterial()->m_TableRange[1] == -1)
		volume->GetMaterial()->m_TableRange[1] = 1;

	//volume->GetMaterial()->UpdateProp();
	advProber->SetVolumeLink(mafNode::SafeDownCast(volume));
	advProber->SetPolylineLink(mafNode::SafeDownCast(spline));
	advProber->GetOutput()->GetVTKData()->Update();
	advProber->SetParent(storage->GetRoot());
	advProber->Modified();

	double srin[2],srout[2];
	volume->GetOutput()->GetVTKData()->GetScalarRange(srin);
	advProber->GetImage()->GetScalarRange(srout);
	CPPUNIT_ASSERT(srin[0]<=srout[0] && srin[1]>=srout[1]);

	double b1[6],b2[6];
	volume->GetOutput()->GetVTKData()->GetBounds(b1);
	advProber->GetOutput()->GetVTKData()->GetBounds(b2);

	advProber->SetParent(NULL);
	spline->SetParent(NULL);
	volume->SetParent(NULL);
	polyline->SetParent(NULL);

	mafDEL(importer_volume);
	mafDEL(importer_polyline);

	mafDEL(advProber);
	mafDEL(spline);

	mafDEL(storage);

}