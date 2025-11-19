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
#include "vtkALBAContourVolumeMapper.h"
#include "vtkALBAFillingHole.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkTransform.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkPointData.h"
#include "vtkRectilinearGridWriter.h"
#include "vtkDoubleArray.h"

#include "albaDefines.h"
#include "vtkALBAVolumeToClosedSmoothSurface.h"



vtkCxxRevisionMacro(vtkALBAVolumeToClosedSmoothSurface, "$Revision: 1.1.2.6 $");
vtkStandardNewMacro(vtkALBAVolumeToClosedSmoothSurface);


//----------------------------------------------------------------------------
vtkALBAVolumeToClosedSmoothSurface::vtkALBAVolumeToClosedSmoothSurface()
//----------------------------------------------------------------------------
{
  //Setting default values
  BorderVolumeID=NULL;
  BorderVolumeRG=NULL;
  FillHoles = true;
  SmoothSurface = true;
}

//----------------------------------------------------------------------------
vtkALBAVolumeToClosedSmoothSurface::~vtkALBAVolumeToClosedSmoothSurface()
//----------------------------------------------------------------------------
{ 
    //Deleting pre allocated structures
    vtkDEL(BorderVolumeID);
    vtkDEL(BorderVolumeRG);
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

//----------------------------------------------------------------------------
vtkPolyData * vtkALBAVolumeToClosedSmoothSurface::GetOutput( int level /*= 0*/, vtkPolyData *data /*= NULL*/ )
//----------------------------------------------------------------------------
{
  vtkPolyData *polydata=Superclass::GetOutput(level,data);

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
    GetTransformFactor(true,bounds,scale,traslation);
    vtkALBASmartPointer<vtkTransform> transform;
    transform->PostMultiply();
    transform->Translate(traslation);
    transform->Scale(scale);
    vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFilter;
    transformFilter->SetTransform(transform);
    transformFilter->SetInput(polydata);
    transformFilter->Update();

    //Taubin Smooth filter apply
    smoothFilter->SetInput(transformFilter->GetOutput());
    smoothFilter->SetFeatureAngle(30.0);
    smoothFilter->SetBoundarySmoothing(0);
    smoothFilter->SetNonManifoldSmoothing(0);
    smoothFilter->SetFeatureEdgeSmoothing(0);
    smoothFilter->SetNumberOfIterations(10);
    smoothFilter->SetPassBand(0.1);
    smoothFilter->Update();
    smoothFilter->GetOutput()->Update();

    //Re-Transforming smoothed output in [-1,1],[-1,1],[-1,1] 
    //To remove filter scaling/traslation artifact
    smoothFilter->GetOutput()->GetBounds(bounds);
    GetTransformFactor(true,bounds,scale,traslation);
    vtkALBASmartPointer<vtkTransform> transform2;
    transform2->PostMultiply();
    transform2->Translate(traslation);
    transform2->Scale(scale);
    vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFilter2;
    transformFilter2->SetTransform(transform2);
    transformFilter2->SetInput(smoothFilter->GetOutput());
    transformFilter2->Update();


    //inverse transform to align outputs to original bounds 
    polydata->GetBounds(bounds);
    GetTransformFactor(false,bounds,scale,traslation);
    vtkALBASmartPointer<vtkTransform> transform3;
    //in this case we need to scale first to obtain the surface 
    //at the original size and then we translate it to the original pos
    transform3->PostMultiply();
    transform3->Scale(scale);
    transform3->Translate(traslation);
    vtkALBASmartPointer<vtkTransformPolyDataFilter> transformFilter3;
    transformFilter3->SetTransform(transform3);
    transformFilter3->SetInput(transformFilter2->GetOutput());
    transformFilter3->Update();

    polydata->DeepCopy(transformFilter3->GetOutput());
  }
  if (FillHoles)
  {
    //When Fill holes is enabled the border on the volume,
    //this generate a border outiside the volume in the output surface
    //Here we remove that border
    double pointCoords[3];

    //this epsilon is needed to show the surface in first and last slice 
    //this is need because there are some approximation problems
    int nPoints;
    vtkPoints *newPoints;
    

    //get number of points
    nPoints=polydata->GetNumberOfPoints();
    
    vtkNEW(newPoints);
    //new point array whitout border
    newPoints->SetNumberOfPoints(nPoints);

    double limits[6];

    
    limits[0]=InputBounds[0]+VoxelShift[0];
    limits[1]=InputBounds[1]-VoxelShift[1];
    limits[2]=InputBounds[2]+VoxelShift[2];
    limits[3]=InputBounds[3]-VoxelShift[3];
    limits[4]=InputBounds[4]+VoxelShift[4];
    limits[5]=InputBounds[5]-VoxelShift[5];

    for (int i=0;i<nPoints;i++)
    {
      //X coordinate
      polydata->GetPoint(i,pointCoords);
      if (pointCoords[0]<limits[0])
        pointCoords[0]=InputBounds[0]-VoxelShift[0];
      if (pointCoords[0]>limits[1])
        pointCoords[0]=InputBounds[1]+VoxelShift[1];

      //Y coordinate
      if (pointCoords[1]<limits[2])
        pointCoords[1]=InputBounds[2]-VoxelShift[2];
      if (pointCoords[1]>limits[3])
        pointCoords[1]=InputBounds[3]+VoxelShift[3];

      //Z coordinate
      if (pointCoords[2]<limits[4])
        pointCoords[2]=InputBounds[4]-VoxelShift[4];
      if (pointCoords[2]>limits[5])
        pointCoords[2]=InputBounds[5]+VoxelShift[5];

      newPoints->SetPoint(i,pointCoords);
    }
    
    polydata->SetPoints(newPoints);
    
    vtkDEL(newPoints);
  
  }

  return polydata;
}

//X,Y,Z to point ID define
#define COORD_TO_ID(x,y,z)  (z)*(inputDimensions[0])*(inputDimensions[1]) + (y)*(inputDimensions[0]) + (x)

//-------------------------------------------------------------------
void vtkALBAVolumeToClosedSmoothSurface::Update()
//------------------------------------------------------------------------------
{

  if (vtkImageData::SafeDownCast(this->GetInput()) != NULL || vtkRectilinearGrid::SafeDownCast(this->GetInput()) != NULL) 
  {

    //common variables
    double spacing[3],range[2],origin[3];
    int inputDimensions[3],newDimension[3],x,y,z,rest;
    vtkPointData *inputPD, *newPD;
    vtkDataArray *inputScalars;
    vtkDoubleArray *newScalars;
    int newPoints;

    //IMAGE DATA
    if (FillHoles && vtkImageData::SafeDownCast(this->GetInput()) != NULL)
    {
      //Create new image data with a border outside 
      vtkImageData *inputVolume;
      
      inputVolume=vtkImageData::SafeDownCast(this->GetInput());
      inputVolume->UpdateData();
      inputVolume->Update();

      // Stores Input Bounds
      inputVolume->GetBounds(InputBounds);
      
      //Get image data necessary information
      inputVolume->GetSpacing(spacing);
      inputVolume->GetDimensions(inputDimensions);
      inputVolume->GetOrigin(origin);
      inputVolume->GetScalarRange(range);
      inputPD=inputVolume->GetPointData();
      inputScalars=inputPD->GetScalars();

      //Creating new imagedata with border outside
      vtkNEW(BorderVolumeID);
      BorderVolumeID->SetSpacing(spacing);
      //New origin is in x-xSpacing,y-ySpacing,z-zSpacing 
      //because we add a voxel outside the volume
      BorderVolumeID->SetOrigin(origin[0]-spacing[0],origin[1]-spacing[1],origin[2]-spacing[2]);
      newPD=BorderVolumeID->GetPointData();
      
      //Saving final filter info
      VoxelShift[0]=VoxelShift[1]=spacing[0]/3.0;
      VoxelShift[2]=VoxelShift[3]=spacing[1]/3.0;
      VoxelShift[4]=VoxelShift[5]=spacing[2]/3.0;
      
    }
    //RECTILINEAR GRID
    else if (FillHoles)
    {
      
      //Create new rectlinear with a border outside 
      vtkRectilinearGrid *inputVolume;
      int ncoord;
      vtkDoubleArray *newXCoord,*newYCoord,*newZCoord;
      vtkDataArray *inputXCoord,*inputYCoord,*inputZCoord;

      inputVolume=vtkRectilinearGrid::SafeDownCast(this->GetInput());
      inputVolume->UpdateData();
      inputVolume->Update();

      // Stores Input Bounds
      inputVolume->GetBounds(InputBounds);
      
      //Get rectilinear grid necessary information
      inputVolume->GetDimensions(inputDimensions);
      inputVolume->GetScalarRange(range);
      inputPD=inputVolume->GetPointData();
      inputScalars=inputPD->GetScalars();


      vtkNEW(BorderVolumeRG);
      newPD=BorderVolumeRG->GetPointData();

      //Create a new coordinate array with two voxel more
      //One on the left and one on the right
      inputXCoord=inputVolume->GetXCoordinates();
      ncoord=inputXCoord->GetNumberOfTuples();
      vtkNEW(newXCoord);
      newXCoord->SetNumberOfComponents(1);
      newXCoord->SetNumberOfTuples(ncoord+2);
      //The size of the added left voxel  is the same of the first
      newXCoord->SetTuple1(0,inputXCoord->GetTuple1(0)-(inputXCoord->GetTuple1(1)-inputXCoord->GetTuple1(0)));
      VoxelShift[0]=(inputXCoord->GetTuple1(1)-inputXCoord->GetTuple1(0))/3.0;
      for(int i=0;i<ncoord;i++)
        newXCoord->SetTuple1(i+1,inputXCoord->GetTuple1(i));
      //The size of the added right voxel is the same of the first
      newXCoord->SetTuple1(ncoord+1,inputXCoord->GetTuple1(ncoord-1)+(inputXCoord->GetTuple1(ncoord-1)-inputXCoord->GetTuple1(ncoord-2)));
      VoxelShift[1]=(inputXCoord->GetTuple1(ncoord-1)-inputXCoord->GetTuple1(ncoord-2))/3.0;
      BorderVolumeRG->SetXCoordinates(newXCoord);

      //Create a new coordinate array with two voxel more
      //One on the left and one on the right
      inputYCoord=inputVolume->GetYCoordinates();
      ncoord=inputYCoord->GetNumberOfTuples();
      vtkNEW(newYCoord);
      newYCoord->SetNumberOfComponents(1);
      newYCoord->SetNumberOfTuples(ncoord+2);
      //The size of the added left voxel  is the same of the first
      newYCoord->SetTuple1(0,inputYCoord->GetTuple1(0)-(inputYCoord->GetTuple1(1)-inputYCoord->GetTuple1(0)));
      VoxelShift[2]=(inputYCoord->GetTuple1(1)-inputYCoord->GetTuple1(0))/3.0;
      for(int i=0;i<ncoord;i++)
        newYCoord->SetTuple1(i+1,inputYCoord->GetTuple1(i));
      newYCoord->SetTuple1(ncoord+1,inputYCoord->GetTuple1(ncoord-1)+(inputYCoord->GetTuple1(ncoord-1)-inputYCoord->GetTuple1(ncoord-2)));
      VoxelShift[3]=(inputYCoord->GetTuple1(ncoord-1)-inputYCoord->GetTuple1(ncoord-2))/3.0;
      BorderVolumeRG->SetYCoordinates(newYCoord);

      //Create a new coordinate array with two voxel more
      //One on the left and one on the right
      inputZCoord=inputVolume->GetZCoordinates();
      ncoord=inputZCoord->GetNumberOfTuples();
      vtkNEW(newZCoord);
      newZCoord->SetNumberOfComponents(1);
      newZCoord->SetNumberOfTuples(ncoord+2);
      //The size of the added left voxel  is the same of the first
      newZCoord->SetTuple1(0,inputZCoord->GetTuple1(0)-(inputZCoord->GetTuple1(1)-inputZCoord->GetTuple1(0)));
      VoxelShift[4]=(inputZCoord->GetTuple1(1)-inputZCoord->GetTuple1(0))/3.0;
      for(int i=0;i<ncoord;i++)
        newZCoord->SetTuple1(i+1,inputZCoord->GetTuple1(i));
      //The size of the added right voxel is the same of the first
      newZCoord->SetTuple1(ncoord+1,inputZCoord->GetTuple1(ncoord-1)+(inputZCoord->GetTuple1(ncoord-1)-inputZCoord->GetTuple1(ncoord-2)));
      VoxelShift[5]=(inputZCoord->GetTuple1(ncoord-1)-inputZCoord->GetTuple1(ncoord-2))/3.0;
      BorderVolumeRG->SetZCoordinates(newZCoord);

      vtkDEL(newXCoord);
      vtkDEL(newYCoord);
      vtkDEL(newZCoord);
    }
     
    //COMMON PART
    //both for RG and ID we need to create the news scalars

    //calculating new dimensions and scalarnumbers
    newDimension[0]=inputDimensions[0]+2;
    newDimension[1]=inputDimensions[1]+2;
    newDimension[2]=inputDimensions[2]+2;
    newPoints=newDimension[0]*newDimension[1]*newDimension[2];

    vtkNEW(newScalars);
    newScalars->SetNumberOfComponents(1);
    newScalars->SetNumberOfTuples(newPoints);

    for (int i=0;i<newPoints;i++)
    {
      //Point coordinates subdivision
      z=i/(newDimension[0]*newDimension[1]);
      rest=i-(z*newDimension[0]*newDimension[1]);
      y=rest/newDimension[0];
      x=rest-(y*newDimension[0]);

      //The voxels in the border is set to the lower range of the scalars
      //If there is a voxel >= of the contour value at the border, we obtain a contour between
      //that voxel and the contour
      if (x==0 || y==0 || z == 0 || x>=newDimension[0]-1 || y >= newDimension[1]-1 || z >= newDimension[2]-1)
        newScalars->SetTuple1(i,range[0]);
      else
        newScalars->SetTuple1(i, inputScalars->GetTuple1( COORD_TO_ID(x-1,y-1,z-1) ) );
    }

    if (FillHoles)
    {
      newPD->SetScalars(newScalars);
      newPD->Update();
    }
    if (BorderVolumeID)
    {
      //setting new dimensions and update
      BorderVolumeID->SetDimensions(newDimension);
      BorderVolumeID->Update();
      BorderVolumeID->UpdateData();
      //set input is need for superclass execution 
      SetInput(BorderVolumeID);
    }
    else
    {
      //setting new dimensions and update
      BorderVolumeRG->SetDimensions(newDimension);
      BorderVolumeRG->Update();
      BorderVolumeRG->UpdateData();
      //set input is need for superclass execution 
      SetInput(BorderVolumeRG);
    }

    Superclass::Update();
  }
  
}

