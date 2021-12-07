/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpCreateVolume
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpCreateVolume_H__
#define __albaOpCreateVolume_H__

#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEVolumeGray;

//----------------------------------------------------------------------------
// albaOpCreateVolume :
//----------------------------------------------------------------------------
/** Operation used to create a albaVolumeGray containing the vtkStructuredPoint
dataset filled with a constant scalar value. This is useful for testing purposes.*/
class ALBA_EXPORT albaOpCreateVolume: public albaOp
{
public:
  albaOpCreateVolume(const wxString &label = "Create Volume");
  ~albaOpCreateVolume(); 

  albaTypeMacro(albaOpCreateVolume, albaOp);

  /** Receive events coming from the user interface.*/
  void OnEvent(albaEventBase *alba_event);

  /** Return a copy of the operation.*/
  albaOp* Copy();

  /** Builds operation's interface. */
  void OpRun();

  /** Set the volume density from code.*/
  void SetVolumeDensity(double density) {m_Density = density;};

  /** Return the volume density of the volume.*/
  double GetVolumeDensity() {return m_Density;};

  /** Set/Get the volume spacing.*/
  void SetVolumeSpacing(double spc[3]);

  /** Set/Get the volume spacing.*/
  void GetVolumeSpacing(double spc[3]);

  /** Set/Get the volume real dimension.*/
  void SetVolumeDimensions(double dim[3]);

  /** Set/Get the volume real dimension.*/
  void GetVolumeDimensions(double dim[3]);

  /** Create the vtkImageData and put it into the albaVolumeGray.*/
  void CreateVolume();

private:

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool InternalAccept(albaVME*node);

  albaVMEVolumeGray *m_Volume;
  double m_Spacing[3]; ///< Voxel spacing of the created volume
  double m_Dimensions[3]; ///< Dimensions of created volume
  double m_Density; ///< Constant density value that fill the created volume
};
#endif
