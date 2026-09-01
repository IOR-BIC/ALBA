/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBARayCastCleaner
 Authors: Gianluigi Crimi, Grazia Di Cosmo
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "vtkALBARayCastCleaner.h"

#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkUnsignedShortArray.h"
#include "vtkPointData.h"
#include "vtkALBASmartPointer.h"
#include "albaDefines.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

enum RAY_CAST_MODALITY
{
  CT_MODALITY,
  MR_MODALITY,
};

vtkStandardNewMacro(vtkALBARayCastCleaner);

//----------------------------------------------------------------------------
vtkALBARayCastCleaner::vtkALBARayCastCleaner()
//----------------------------------------------------------------------------
{
  //Default threshold values
  BloodLowerThreshold=200.0;
  BloodUpperThreshold=340.0;
  BoneLowerThreshold=350.0;

  Modality=CT_MODALITY;

}

//----------------------------------------------------------------------------
vtkALBARayCastCleaner::~vtkALBARayCastCleaner()
//----------------------------------------------------------------------------
{ 
}

#define max(a,b)  (((a) > (b)) ? (a) : (b))

//Shift the scalar value from range [x,y] to [0,y-x]
#define scalarShift(X) (X-range[0])


//------------------------------------------------------------------------------
int vtkALBARayCastCleaner::RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
//------------------------------------------------------------------------------
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
	vtkImageData  *input = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
	vtkImageData *output = vtkImageData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  double range[2];
  double newValue, boneValue;
  //generating a copy of the input 
  output->DeepCopy(input);

  //getting image dimension for neighbors calculation  
  output->GetDimensions(VolumeDimension);
  

  //Creating a copy of data array 
  //(proximity checks need to be done with unmodified original values)
  vtkDataArray* imgScalars = (vtkDataArray*)output->GetPointData()->GetScalars();
  vtkUnsignedShortArray * newScalars;
  vtkNEW(newScalars);
  
  int nPoints=output->GetNumberOfPoints();

  imgScalars->GetRange(range);
  
  newScalars->SetNumberOfTuples(nPoints);
  newScalars->SetNumberOfComponents(1);
  
  for (int i=0;i<nPoints;i++)
  {
    newValue=scalarShift(imgScalars->GetTuple1(i));
    newScalars->SetTuple1(i,newValue);
  }
  
  //In MR_MODALITY The cleaner just make scalarshift to avoid vtkVolumeRayCastMapper errors
  //In CT_MODALITY the cleaner remove the blood from bone boundary 
  if (Modality==CT_MODALITY)
  {
    for (int i=0;i<nPoints;i++)
    {
      //this filter removes border interpolation values from bone boundary
      //if i get a blood voxel and a neighbor is a bone I set it's value
      //to bone lower threshold 
      if (isBlood(imgScalars->GetTuple1(i)))
      {
        int maxSidesInNeighboors=BoneInNeighborsAffinity(i,imgScalars);
        if (maxSidesInNeighboors)
        {
          //If there is a bone neighbors we set a bone value in output depending on 
          //level of affinity, in this manner if there is a high affinity the output 
          //voxel has an high level of opacity in raycast pipe
          boneValue= scalarShift(BoneLowerThreshold*(1.0+(maxSidesInNeighboors*0.02)));
          newScalars->SetTuple1(i, boneValue);
        }
      }
    }
  }

  //settings new scalar values to output volume
  output->GetPointData()->SetScalars(newScalars);
  output->GetPointData()->Modified();
  output->GetPointData()->Update();

  vtkDEL(newScalars);

	return 1;
}

#define SP_COORD_TO_ID(x,y,z)  z*(VolumeDimension[0])*(VolumeDimension[1]) + y*(VolumeDimension[0]) + x;

//----------------------------------------------------------------------------
int vtkALBARayCastCleaner::BoneInNeighborsAffinity(int pointID, vtkDataArray* scalars )
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

  int affinity = 0;

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
          {
            int currAffinity=1;
            if (x==ix) currAffinity++;
            if (y==iy) currAffinity++;
            if (z==iz) currAffinity++;
            affinity=max(affinity,currAffinity);
          }
            
        }
      }
    }
  }

  return affinity;
}

//----------------------------------------------------------------------------
int vtkALBARayCastCleaner::isBlood( double scalarValue )
//----------------------------------------------------------------------------
{
  //check if a values is inside blood range
  return (scalarValue >= BloodLowerThreshold && scalarValue <= BloodUpperThreshold);
}

//----------------------------------------------------------------------------
void vtkALBARayCastCleaner::SetModalityToCT()
//----------------------------------------------------------------------------
{
  Modality=CT_MODALITY;
}

//----------------------------------------------------------------------------
void vtkALBARayCastCleaner::SetModalityToMR()
//----------------------------------------------------------------------------
{
  Modality=MR_MODALITY;
}

//----------------------------------------------------------------------------
bool vtkALBARayCastCleaner::IsModalityCT()
//----------------------------------------------------------------------------
{
  return Modality==CT_MODALITY;
}

//----------------------------------------------------------------------------
bool vtkALBARayCastCleaner::IsModalityMR()
//----------------------------------------------------------------------------
{
  return Modality==MR_MODALITY;
}
