/*=========================================================================

 Program: MAF2
 Module: mafOpCreateVolume
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpCreateVolume_H__
#define __mafOpCreateVolume_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVolumeGray;

//----------------------------------------------------------------------------
// mafOpCreateVolume :
//----------------------------------------------------------------------------
/** Operation used to create a mafVolumeGray containing the vtkStructuredPoint
dataset filled with a constant scalar value. This is useful for testing purposes.*/
class MAF_EXPORT mafOpCreateVolume: public mafOp
{
public:
  mafOpCreateVolume(const wxString &label = "Create Volume");
  ~mafOpCreateVolume(); 

  mafTypeMacro(mafOpCreateVolume, mafOp);

  /** Receive events coming from the user interface.*/
  void OnEvent(mafEventBase *maf_event);

  /** Return a copy of the operation.*/
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafVME*node);

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

  /** Create the vtkStructuredPoints and put it into the mafVolumeGray.*/
  void CreateVolume();

private:
  mafVMEVolumeGray *m_Volume;
  double m_Spacing[3]; ///< Voxel spacing of the created volume
  double m_Dimensions[3]; ///< Dimensions of created volume
  double m_Density; ///< Constant density value that fill the created volume
};
#endif
