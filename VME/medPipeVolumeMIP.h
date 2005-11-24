/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeMIP.h,v $
  Language:  C++
  Date:      $Date: 2005-11-24 14:54:40 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef _medPipeVolumeMIP_H_
#define _medPipeVolumeMIP_H_

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkImageCast;
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkVolumeRayCastMIPFunction;
class vtkVolumeRayCastMapper;
class vtkActor;
class vtkLODProp3D;
//----------------------------------------------------------------------------
// medPipeVolumeMIP :
//----------------------------------------------------------------------------
class medPipeVolumeMIP : public mafPipe
{
public:
  mafTypeMacro(medPipeVolumeMIP,mafPipe);

           medPipeVolumeMIP();
  virtual ~medPipeVolumeMIP();

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

protected:
  vtkImageCast                *m_Caster;
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkVolumeRayCastMIPFunction *m_MIPFunction;
  vtkVolumeRayCastMapper      *m_VolumeMapper;
  vtkVolumeRayCastMapper      *m_VolumeMapperLow;
  vtkLODProp3D                *m_VolumeLOD;

  vtkActor               *m_SelectionActor;
};
#endif // _medPipeVolumeMIP_H_
