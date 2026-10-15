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

#ifndef __vtkALBAVolumeToClosedSmoothSurface_H__
#define __vtkALBAVolumeToClosedSmoothSurface_H__

#define _WINSOCKAPI_ 

#include "vtkDataObjectAlgorithm.h"


/** vtkALBAVolumeToClosedSmoothSurface: This filter generates an contour surface from a volume, 
and add the possibility of getting an closed or smoothed surface.
*/
//---------------------------------------------------------------------------
class ALBA_EXPORT vtkALBAVolumeToClosedSmoothSurface : public vtkDataObjectAlgorithm
{
public:
  
  /** Add collect revision method */
  vtkTypeMacro(vtkALBAVolumeToClosedSmoothSurface, vtkDataObjectAlgorithm);

    
	// Usual data generation method
	int RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** inputVector, vtkInformationVector* outputVector);

  /** create an instance of the object */
  static vtkALBAVolumeToClosedSmoothSurface *New();

  /** Return true if fill border holes procedure is enabled. */
  vtkGetMacro(FillBorderHoles, int); 
  /** Enable or disables fill border holes procedure. */  
	vtkSetMacro(FillBorderHoles, int);
  /** bool macro, Enable or disables fill border holes procedure. */
  vtkBooleanMacro(FillBorderHoles, int);

  /** Return true if output smoothing is enabled. */
  vtkGetMacro(SmoothSurface, int);  
  /** Enable or disables output smoothing. */  
  void SetSmoothSurface(int value){SmoothSurface=value;};
  /** bool macro, Enable or disables output smoothing. */
  vtkBooleanMacro(SmoothSurface, int);

	/** Get the threshold for contour algorithms */
	vtkGetMacro(ContourValue, double);

	/** Set the threshold for contour algorithms */
	vtkSetMacro(ContourValue, double);

protected:

  /**
   * Fill the input port information objects for this algorithm.  This
   * is invoked by the first call to GetInputPortInformation for each
   * port so subclasses can specify what they can handle.
   */
  int FillInputPortInformation(int port, vtkInformation *info) override;
	
  /**
   * Fill the output port information objects for this algorithm.
   * This is invoked by the first call to GetOutputPortInformation for
   * each port so subclasses can specify what they can handle.
   */
  int FillOutputPortInformation(int port, vtkInformation *info) override;

private:

  int FillBorderHoles;
  int SmoothSurface;
  double ContourValue;

  /** Default constructor */
  vtkALBAVolumeToClosedSmoothSurface();

  /** Default destructor */
  ~vtkALBAVolumeToClosedSmoothSurface();


  /** Caluclates the scale/traslation to obtain a cube in [-1,1], 
     If toUnity is set to false returns the inverse factors*/
  void GetTransformFactor( int toUnity,double *bounds, double *scale, double *traslation );
};

#endif