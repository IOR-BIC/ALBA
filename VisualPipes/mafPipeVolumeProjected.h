/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeProjected
 Authors: Paolo Quadrani - Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeVolumeProjected_H__
#define __mafPipeVolumeProjected_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkWindowLevelLookupTable;
class vtkMAFAssembly;
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkMAFProjectSP;
class vtkMAFProjectRG;
class vtkRectilinearGrid;

//----------------------------------------------------------------------------
// mafPipeVolumeProjected :
//----------------------------------------------------------------------------
/** This visual pipe allows to represent a volume data as an image projecting its
scalar values along a specified axes.*/
class MAF_EXPORT mafPipeVolumeProjected : public mafPipe
{
public:
  mafTypeMacro(mafPipeVolumeProjected,mafPipe);

           mafPipeVolumeProjected();
  virtual ~mafPipeVolumeProjected();

  /** Parameter to initialize a non default projection (X axes) */
  void InitializeProjectParameters(int cam_position);
  
  virtual void Create(mafSceneNode *n);

	void GetBestSpacing(double * bestSpacing, vtkRectilinearGrid* rGrid);

	/** Show-Hide the RX bounding box actor. */
  void Select     (bool select);

	/** Set the range to the lookup table for the RX texture. */
	void SetLutRange(double low, double hi);

	/** Get the range of the texture's lookup table. */
	void GetLutRange(double range[2]);

	/** Enable/Disable the projection of a subrange on the projection direction */
	void EnableRangeProjection(bool enabled);

	/** Sets the range of projection, the range is expressed like volume dims*/
	void SetProjectionRange(int range[2]);

  /** Set Tick Actor Visibility */
  void TickActorVisibilityOn();
  void TickActorVisibilityOff();

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

protected:   
	int       m_CamPosition;
  vtkActor *m_RXActor;
  vtkActor *m_TickActor;
  vtkActor *m_Ghost;

	vtkOutlineCornerFilter *m_VolumeBox;
  vtkPolyDataMapper			 *m_VolumeBoxMapper;
  vtkActor               *m_VolumeBoxActor;
	vtkMAFProjectSP       *m_SPProjFilter;
	vtkMAFProjectRG       *m_RGProjFilter;
	bool m_RangeProjectionEnabled;
	int m_ProjectionRange[2];

  vtkWindowLevelLookupTable *m_Lut;
  vtkMAFAssembly            *m_UsedAssembly;
};  
#endif // __mafPipeVolumeProjected_H__
