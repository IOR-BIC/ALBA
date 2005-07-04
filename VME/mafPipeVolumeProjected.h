/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVolumeProjected.h,v $
  Language:  C++
  Date:      $Date: 2005-07-04 17:18:06 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _mafPipeVolumeProjected_H_
#define _mafPipeVolumeProjected_H_

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

	/** Show-Hide the rx actor. */
//	void Show       (bool show); 

	/** Show-Hide the rx bounding box actor. */
  void Select     (bool select);

	/** Set the range to the lookup table for the rx texture. */
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
#endif // _mafPipeVolumeProjected_H_
