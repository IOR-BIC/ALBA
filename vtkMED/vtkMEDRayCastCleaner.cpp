/*===========================================================================
Program:   Medical
Module:    $RCSfile: vtkMEDImageFillHolesRemoveIslands.cxx,v $
Language:  C++
Date:      $Date: 2012-01-26 13:44:58 $
Version:   $Revision: 1.1.2.2 $
Authors:   Gianluigi Crimi, Grazia Di Cosmo
=============================================================================
Copyright (c) 2010
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
===========================================================================*/


#include "vtkMedRayCastCleaner.h"

#include "vtkObjectFactory.h"
#include "vtkStructuredPoints.h"
#include "vtkUnsignedCharArray.h"
#include "vtkshortArray.h"
#include "vtkPointData.h"
#include "vtkMAFSmartPointer.h"
#include "mafDefines.h"



#define PENINSULA_CORNER_MAXIMUM_NUMBER_OF_PIXELS 1

vtkCxxRevisionMacro(vtkMEDRayCastCleaner, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkMEDRayCastCleaner);

//----------------------------------------------------------------------------
vtkMEDRayCastCleaner::vtkMEDRayCastCleaner()
//----------------------------------------------------------------------------
{
  //Default threshold values
  BloodLowerThreshold=200.0;
  BloodUpperThreshold=340.0;
  BoneLowerThreshold=350.0;

}

//----------------------------------------------------------------------------
vtkMEDRayCastCleaner::~vtkMEDRayCastCleaner()
//----------------------------------------------------------------------------
{ 
}


//------------------------------------------------------------------------------
void vtkMEDRayCastCleaner::Execute()
//------------------------------------------------------------------------------
{
  vtkStructuredPoints *outputImage = this->GetOutput();
  this->GetInput()->Update();
  
  //generating a copy of the input 
  outputImage->DeepCopy(this->GetInput());
  outputImage->UpdateData();
  outputImage->Update();

  //getting image dimension for neighbours calculation  
  outputImage->GetDimensions(VolumeDimension);
  

  //Creating a copy of data array 
  //(proximity checks need to be done with unmodified original values)
  vtkDataArray* imgScalars = (vtkDataArray*)outputImage->GetPointData()->GetScalars();
  vtkShortArray* scalarsCopy;
  vtkNEW(scalarsCopy);

  scalarsCopy->DeepCopy(imgScalars);
  

  int nPoints=outputImage->GetNumberOfPoints();

  for (int i=0;i<nPoints;i++)
  {
    //this filter removes border interpolation values from bone boundary
    //if i get a blood voxel and a neighbor is a bone I set it's value
    //to bone lower threshold 
    if (isBlood(scalarsCopy->GetTuple1(i)) && BoneInNeighbors(i,scalarsCopy) )
      imgScalars->SetTuple1(i, BoneLowerThreshold);
  }
  
  //settings new scalar values to output volume
  outputImage->GetPointData()->SetScalars(imgScalars);
  outputImage->GetPointData()->Modified();
  outputImage->GetPointData()->Update();
  outputImage->UpdateData();
  outputImage->Update();
  this->SetOutput(outputImage);

  vtkDEL(scalarsCopy);
}

#define SP_COORD_TO_ID(x,y,z)  z*(VolumeDimension[0])*(VolumeDimension[1]) + y*(VolumeDimension[0]) + x;

//----------------------------------------------------------------------------
int vtkMEDRayCastCleaner::BoneInNeighbors(int pointID, vtkDataArray* scalars )
//----------------------------------------------------------------------------
{

  int x,y,z;
  int rest;
  int currentID;

  //Point coordinates subdivision
  z=pointID/(VolumeDimension[0]*VolumeDimension[1]);
  rest=pointID-(z*VolumeDimension[0]*VolumeDimension[1]);
  y=rest/VolumeDimension[0];
  x=rest-(y*VolumeDimension[0]);

  //the neighbors are disposed like a Rubik cube Sub-cubes
  //We need to search the max value only on valid neighbors 
  //the next three "for" are cicling on z-y-x from point coord -1 to +1
  //the max well be selected
  for (int iz=z-1;iz<=z+1;iz++)
  {
    //if this z-value is outside the volume we just jumps all the sub-loop
    //because all sub-loop are outside the volume 
    if (iz<0 || iz >= VolumeDimension[2])
      continue;
    for (int iy=y-1;iy<=y+1;iy++)
    {
      //if this y-value is outside the volume we just jumps all the sub-loop
      //because x-loop is outside the volume 
      if (iy<0 || iy >= VolumeDimension[1])
        continue;
      for (int ix=x-1;ix<=x+1;ix++)
      {
        //We need to control if the x value is outside the volume
        if (ix>0 && ix < VolumeDimension[0])
        {
          currentID=SP_COORD_TO_ID(ix,iy,iz);
          if ( (currentID != pointID) && (scalars->GetTuple1(currentID)>=BoneLowerThreshold))
            return true;
        }
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------
int vtkMEDRayCastCleaner::isBlood( double scalarValue )
//----------------------------------------------------------------------------
{
  //check if a values is inside blood range
  return (scalarValue >= BloodLowerThreshold && scalarValue <= BloodUpperThreshold);
}
