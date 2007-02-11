/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafVMEAdvancedProberTest.cpp,v $
Language:  C++
Date:      $Date: 2007-02-11 16:24:15 $
Version:   $Revision: 1.1 $
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

#include "vtkDataSet.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"


void mafVMEAdvancedProberTest::Test()
{
	mafVMEStorage *storage = mafVMEStorage::New();
	storage->GetRoot()->SetName("root");
	storage->GetRoot()->Initialize();

	mmoVTKImporter *importer=new mmoVTKImporter("importer");;
	mafString filename_volume=MED_DATA_ROOT;
	filename_volume<<"/VTK_Volumes/Volume.vtk";
	importer->TestModeOn();
	importer->SetFileName(filename_volume);
	importer->SetInput(storage->GetRoot());
	importer->ImportVTK();
	mafVMEVolumeGray *volume=mafVMEVolumeGray::SafeDownCast(importer->GetOutput());
	volume->GetOutput()->GetVTKData()->Update();
	volume->SetParent(storage->GetRoot());
	volume->Update();

	mafString filename_polyline=MED_DATA_ROOT;
	filename_polyline<<"/Polyline/Polyline.vtk";
	importer->SetFileName(filename_polyline);
	importer->ImportVTK();
	mafVMEPolyline *polyline=mafVMEPolyline::SafeDownCast(importer->GetOutput());
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
	advProber->SetVolumeLink(mafNode::SafeDownCast(volume));
	advProber->SetPolylineLink(mafNode::SafeDownCast(spline));
	advProber->GetOutput()->GetVTKData()->Update();
	advProber->SetParent(storage->GetRoot());
	advProber->Modified();

	double srin[2],srout[2];
	volume->GetOutput()->GetVTKData()->GetScalarRange(srin);
	//advProber->GetOutput()->GetVTKData()->GetScalarRange(srout);
	advProber->GetMaterial()->GetMaterialTexture()->GetScalarRange(srout);



	mafDEL(advProber);
	mafDEL(spline);
	mafDEL(importer);
	CPPUNIT_ASSERT(true);
}