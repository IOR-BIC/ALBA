/*=========================================================================

 Program: MAF2
 Module: vtkMAFRayCastCleaner
 Authors: Gianluigi Crimi, Grazia Di Cosmo
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkMAFRayCastCleaner_H__
#define __vtkMAFRayCastCleaner_H__

#include "mafConfigure.h"
#include "vtkMAFStructuredPointsAlgorithm.h"

class vtkImageData;
class vtkStructuredPoints;
class vtkDataArray;


/** vtkMAFRayCastCleaner: This filter prepare a volume for raycast render
It makes a shift the scalar value from range [x,y] to [0,y-x], because the raycast renderer needs
unsigned short input.

It removes border interpolation values from bone boundary
if a blood voxel is in bone boundary the corresponding scalar output contains the bone lower threshold 
the effect of this filter is like a bone sanding.
*/
//---------------------------------------------------------------------------
class MAF_EXPORT vtkMAFRayCastCleaner : public vtkMAFStructuredPointsAlgorithm
//---------------------------------------------------------------------------
{
public:
  
  /** Add collect revision method */
  vtkTypeMacro(vtkMAFRayCastCleaner,vtkMAFStructuredPointsAlgorithm);

  /** Dynamic constructor */
  static vtkMAFRayCastCleaner *New();

  /** Set The Bone Lower Threshold */
  vtkSetMacro(BoneLowerThreshold,double);
 
  /** Get the Bone Lower Threshold */
  vtkGetMacro(BoneLowerThreshold,double);

  /** Set The Blood Lower Threshold */
  vtkSetMacro(BloodLowerThreshold,double);

  /** Get the Blood Lower Threshold */
  vtkGetMacro(BloodLowerThreshold,double);

  /** Set The Blood Upper Threshold */
  vtkSetMacro(BloodUpperThreshold,double);

  /** Get the Blood Upper Threshold */
  vtkGetMacro(BloodUpperThreshold,double);

  /** Set Modality to CT */
  void SetModalityToCT();

  /** Return true if modality is CT */
  bool IsModalityCT();

  /** Set Modality to MR */
  void SetModalityToMR();

  /** Return true if modality is MR */
  bool IsModalityMR();

protected:

  /** Execute this filter */
  int RequestData( vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

private:

  /** Default constructor */
  vtkMAFRayCastCleaner();

  /** Default destructor */
  ~vtkMAFRayCastCleaner();

  /** Return 0 if no bone voxel is present on point neighbors
  If bones are present in voxel neighbors return a value determining 
  the affinity level of the nearest bone. 
  The affinity levels are:
  1 if nearest bone voxel shares a corner
  2 if nearest bone voxel shares a side
  3 if nearest bone voxel shares a face
  */
  int BoneInNeighborsAffinity(int pointID, vtkDataArray* scalars);

  
  /** Return true if scalarValue is in blood range */
  int isBlood(double scalarValue);

  //Variables
  int VolumeDimension[3];
  int BoneLowerThreshold;
  int BloodLowerThreshold;
  int BloodUpperThreshold;
  int Modality;
};

#endif