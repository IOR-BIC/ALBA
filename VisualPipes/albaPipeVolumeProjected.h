/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeProjected
 Authors: Paolo Quadrani - Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVolumeProjected_H__
#define __albaPipeVolumeProjected_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkWindowLevelLookupTable;
class vtkALBAAssembly;
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkALBAProjectSP;
class vtkALBAProjectVolume;
class vtkRectilinearGrid;

//----------------------------------------------------------------------------
// albaPipeVolumeProjected :
//----------------------------------------------------------------------------
/** This visual pipe allows to represent a volume data as an image projecting its
scalar values along a specified axes.*/
class ALBA_EXPORT albaPipeVolumeProjected : public albaPipe
{
public:
  albaTypeMacro(albaPipeVolumeProjected,albaPipe);

           albaPipeVolumeProjected();
  virtual ~albaPipeVolumeProjected();

  /** Parameter to initialize a non default projection (X axes) */
  void InitializeProjectParameters(int cam_position);
  
  virtual void Create(albaSceneNode *n);

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

	/** Sets projection modality, 0 ToMean, 1 ToMax*/
	void SetProjectionModality(int modality);

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
	vtkALBAProjectVolume    *m_ProjectFilter;
	bool m_RangeProjectionEnabled;
	int m_ProjectionRange[2];

  vtkWindowLevelLookupTable *m_Lut;
  vtkALBAAssembly            *m_UsedAssembly;
};  
#endif // __albaPipeVolumeProjected_H__
