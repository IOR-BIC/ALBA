/*==========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFBridgeHoleFilter.h,v $
Language:  C++
Date:      $Date: 2009-03-20 16:39:48 $
Version:   $Revision: 1.1.2.2 $
Authors:   Gianluigi Crimi, Grazia Di Cosmo
=============================================================================
Copyright (c) 2010
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
===========================================================================*/

#ifndef __vtkMEDRayCastCleaner_H__
#define __vtkMEDRayCastCleaner_H__

#include "vtkMEDConfigure.h"
#include "vtkStructuredPointsToStructuredPointsFilter.h"

class vtkImageData;
class vtkStructuredPoints;
class vtkDataArray;


/** vtkMedRayCastCleaner: This filter removes border interpolation values from bone boundary
if a blood voxel is in bone boundary the corresponding scalar output contains the bone lower threshold 
the effect of this filter is like a bone sanding
*/
//---------------------------------------------------------------------------
class VTK_vtkMED_EXPORT vtkMEDRayCastCleaner : public vtkStructuredPointsToStructuredPointsFilter
//---------------------------------------------------------------------------
{
public:
  
  /** Add collect revision method */
  vtkTypeRevisionMacro(vtkMEDRayCastCleaner,vtkStructuredPointsToStructuredPointsFilter);

  /** Dynamic constructor */
  static vtkMEDRayCastCleaner *New();

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

protected:

  /** Execute this filter */
  void Execute();

private:

  /** Default constructor */
  vtkMEDRayCastCleaner();

  /** Default destructor */
  ~vtkMEDRayCastCleaner();

  /** Return true if a bone voxel is present on point neighbors*/
  int BoneInNeighbors(int pointID, vtkDataArray* scalars);

  
  /** Return true if scalarValue is in blood range */
  int isBlood(double scalarValue);

  //Variables
  int VolumeDimension[3];
  int BoneLowerThreshold;
  int BloodLowerThreshold;
  int BloodUpperThreshold;
};

#endif