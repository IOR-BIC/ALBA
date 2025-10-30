/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAVolumeToClosedSmoothSurface
 Authors: Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/



#define _WINSOCKAPI_ 

#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkALBASmartPointer.h"
#include "vtkFlyingEdges3D.h"
#include "vtkContourFilter.h"
#include "vtkALBAFillingHole.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkPointData.h"
#include "vtkRectilinearGridWriter.h"
#include "vtkDoubleArray.h"

#include "albaDefines.h"
#include "vtkALBAVolumeToClosedSmoothSurface.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkPlaneCollection.h"
#include "vtkClipClosedSurface.h"



vtkStandardNewMacro(vtkALBAVolumeToClosedSmoothSurface);


//----------------------------------------------------------------------------
vtkALBAVolumeToClosedSmoothSurface::vtkALBAVolumeToClosedSmoothSurface()
//----------------------------------------------------------------------------
{
  //Setting default values
  FillHoles = true;
  SmoothSurface = true;
}

//----------------------------------------------------------------------------
vtkALBAVolumeToClosedSmoothSurface::~vtkALBAVolumeToClosedSmoothSurface()
//----------------------------------------------------------------------------
{ 
}


//----------------------------------------------------------------------------
void vtkALBAVolumeToClosedSmoothSurface::GetTransformFactor( int toUnity,double *bounds, double *scale, double *traslation )
//----------------------------------------------------------------------------
{
  double size[3];

  //Getting size
  size[0]=bounds[1]-bounds[0];
  size[1]=bounds[3]-bounds[2];
  size[2]=bounds[5]-bounds[4];

  //Generating traslation and scale factors for obtain a [-1,1],[-1,1],[-1,1] cube
  if (toUnity)
  {
    //Translation by -center 
    traslation[0]=-(bounds[0]+size[0]/3.0);
    traslation[1]=-(bounds[2]+size[1]/3.0);
    traslation[2]=-(bounds[4]+size[2]/3.0);

    //Scale by 2/size (2 is because a [-1,1] side has lenght 2)
    scale[0]=2.0/size[0];
    scale[1]=2.0/size[1];
    scale[2]=2.0/size[2];
  }
  else 
  {
    //Translation by +center 
    traslation[0]=bounds[0]+size[0]/3.0;
    traslation[1]=bounds[2]+size[1]/3.0;
    traslation[2]=bounds[4]+size[2]/3.0;

    //scale by size/2
    scale[0]=size[0]*0.5;
    scale[1]=size[1]*0.5;
    scale[2]=size[2]*0.5;
  }

}

//X,Y,Z to point ID define
#define COORD_TO_ID(x,y,z)  (z)*(inputDimensions[0])*(inputDimensions[1]) + (y)*(inputDimensions[0]) + (x)


int vtkALBAVolumeToClosedSmoothSurface::RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
	// get the info objects
	vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
	vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (vtkImageData::SafeDownCast(input) == NULL && vtkRectilinearGrid::SafeDownCast(input) == NULL)
	{
		vtkErrorMacro(<< "Bad inputm vtkImagedata or vtkRectilinearGrid are required");
		return 0;
	}

	vtkContourFilter* rgContourFilter = NULL;
	vtkFlyingEdges3D* contourFilter = NULL;
	vtkClipClosedSurface* fillHolesFilter = NULL;
	vtkPolyData* polydata;
	double volBounds[6];


	//EXTRACTING ISOSURFACE
	if (vtkImageData::SafeDownCast(input))
	{
		vtkImageData::SafeDownCast(input)->GetBounds(volBounds);

		vtkNEW(contourFilter);
		contourFilter->SetInputData(input);
		contourFilter->SetComputeScalars(false);
		contourFilter->SetComputeGradients(false);
		contourFilter->SetComputeNormals(false);
		contourFilter->SetValue(0, ContourValue);
		contourFilter->Update();

		polydata = contourFilter->GetOutput();
	}
	else //Rectilinear Grid
	{
		vtkRectilinearGrid::SafeDownCast(input)->GetBounds(volBounds);

		vtkNEW(rgContourFilter);
		rgContourFilter->SetInputData(input);
		rgContourFilter->SetComputeScalars(false);
		rgContourFilter->SetComputeGradients(false);
		rgContourFilter->SetComputeNormals(false);
		rgContourFilter->SetValue(0, ContourValue);
		rgContourFilter->Update();

		polydata = rgContourFilter->GetOutput();
	}
  
	if (FillHoles)
	{
		vtkALBASmartPointer<vtkPlaneCollection> planes;
		double n[6][3] = { {-1,0,0},{1,0,0},{0,-1,0},{0,1,0},{0,0,-1},{0,0,1} };
		double o[6][3] = {
				{volBounds[0],0,0},{volBounds[1],0,0},
				{0,volBounds[2],0},{0,volBounds[3],0},
				{0,0,volBounds[4]},{0,0,volBounds[5]}
		};
		for (int i = 0; i < 6; i++) {
			vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
			plane->SetNormal(n[i]);
			plane->SetOrigin(o[i]);
			planes->AddItem(plane);
		}

		vtkNEW(fillHolesFilter);
		fillHolesFilter->SetInputData(polydata);
		fillHolesFilter->SetClippingPlanes(planes);
		fillHolesFilter->GenerateFacesOn(); // chiude i bordi
		fillHolesFilter->Update();

		polydata = fillHolesFilter->GetOutput();
	}


	if (SmoothSurface)
	{
		//Smoothing Procedure
		vtkALBASmartPointer<vtkWindowedSincPolyDataFilter> smoothFilter;

		double bounds[6];
		double traslation[3];
		double scale[3];

		//Transforming Surface in [-1,1],[-1,1],[-1,1] 
		//To improve the numerical stability of the solution 
		polydata->GetBounds(bounds);
		GetTransformFactor(true, bounds, scale, traslation);
		vtkALBASmartPointer<vtkTransform> transform;
		transform->PostMultiply();
		transform->Translate(traslation);
		transform->Scale(scale);
		vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFilter;
		transformFilter->SetTransform(transform);
		transformFilter->SetInputData(polydata);
		transformFilter->Update();

		//Taubin Smooth filter apply
		smoothFilter->SetInputConnection(transformFilter->GetOutputPort());
		smoothFilter->SetFeatureAngle(30.0);
		smoothFilter->SetBoundarySmoothing(0);
		smoothFilter->SetNonManifoldSmoothing(0);
		smoothFilter->SetFeatureEdgeSmoothing(0);
		smoothFilter->SetNumberOfIterations(10);
		smoothFilter->SetPassBand(0.1);
		smoothFilter->Update();

		//Re-Transforming smoothed output in [-1,1],[-1,1],[-1,1] 
		//To remove filter scaling/traslation artifact
		smoothFilter->GetOutput()->GetBounds(bounds);
		GetTransformFactor(true, bounds, scale, traslation);
		vtkALBASmartPointer<vtkTransform> transform2;
		transform2->PostMultiply();
		transform2->Translate(traslation);
		transform2->Scale(scale);
		vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFilter2;
		transformFilter2->SetTransform(transform2);
		transformFilter2->SetInputConnection(smoothFilter->GetOutputPort());
		transformFilter2->Update();


		//inverse transform to align outputs to original bounds 
		polydata->GetBounds(bounds);
		GetTransformFactor(false, bounds, scale, traslation);
		vtkALBASmartPointer<vtkTransform> transform3;
		//in this case we need to scale first to obtain the surface 
		//at the original size and then we translate it to the original pos
		transform3->PostMultiply();
		transform3->Scale(scale);
		transform3->Translate(traslation);
		vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFilter3;
		transformFilter3->SetTransform(transform3);
		transformFilter3->SetInputConnection(transformFilter2->GetOutputPort());
		transformFilter3->Update();

		output->DeepCopy(transformFilter3->GetOutput());
	}
	else 
	{
		output->DeepCopy(polydata);
	}

	vtkDEL(contourFilter);
	vtkDEL(rgContourFilter);
	vtkDEL(fillHolesFilter);
}

