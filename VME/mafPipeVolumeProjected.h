/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeProjected.h,v $
  Language:  C++
  Date:      $Date: 2006-03-07 15:06:54 $
  Version:   $Revision: 1.3 $
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

protected:   
	int       m_CamPosition;
  vtkActor *m_RXActor;
  vtkActor *m_TickActor;
  vtkActor *m_ghost;

  vtkWindowLevelLookupTable *m_Lut;
  vtkMAFAssembly            *m_UsedAssembly;
};  
#endif // __mafPipeVolumeProjected_H__
