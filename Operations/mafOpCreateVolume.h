/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpCreateVolume.h,v $
  Language:  C++
  Date:      $Date: 2008-12-10 10:16:17 $
  Version:   $Revision: 1.1.2.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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
class mafOpCreateVolume: public mafOp
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
  bool Accept(mafNode *node);

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
