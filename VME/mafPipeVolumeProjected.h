/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeProjected.h,v $
  Language:  C++
  Date:      $Date: 2007-07-17 10:52:24 $
  Version:   $Revision: 1.6 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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

//----------------------------------------------------------------------------
// mafPipeVolumeProjected :
//----------------------------------------------------------------------------
class mafPipeVolumeProjected : public mafPipe
{
public:
  mafTypeMacro(mafPipeVolumeProjected,mafPipe);

           mafPipeVolumeProjected();
  virtual ~mafPipeVolumeProjected();

  /** Parameter to initialize a non default projection (X axes) */
  void InitializeProjectParameters(int cam_position);
  
  virtual void Create(mafSceneNode *n);

	/** Show-Hide the RX bounding box actor. */
  void Select     (bool select);

	/** Set the range to the lookup table for the RX texture. */
	void SetLutRange(double low, double hi);

	/** Get the range of the texture's lookup table. */
	void GetLutRange(double range[2]);

  /** Set Tick Actor Visibility */
  void TickActorVisibilityOn();
  void TickActorVisibilityOff();

  /** Set the actor picking*/
  void SetActorPicking(int enable = true);

protected:   
	int       m_CamPosition;
  vtkActor *m_RXActor;
  vtkActor *m_TickActor;
  vtkActor *m_ghost;

	vtkOutlineCornerFilter *m_VolumeBox;
  vtkPolyDataMapper			 *m_VolumeBoxMapper;
  vtkActor               *m_VolumeBoxActor;

  vtkWindowLevelLookupTable *m_Lut;
  vtkMAFAssembly            *m_UsedAssembly;
};  
#endif // __mafPipeVolumeProjected_H__
