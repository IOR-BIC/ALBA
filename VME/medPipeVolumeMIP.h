/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeMIP.h,v $
  Language:  C++
  Date:      $Date: 2007-04-04 10:52:54 $
  Version:   $Revision: 1.7 $
  Authors:   Paolo Quadrani
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeVolumeMIP_H__
#define __medPipeVolumeMIP_H__

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
class vtkLookupTable;
class vtkVolume;
class vtkImageResample;
class vtkLookupTable;
//----------------------------------------------------------------------------
// medPipeVolumeMIP :
//----------------------------------------------------------------------------
class medPipeVolumeMIP : public mafPipe
{
public:
  mafTypeMacro(medPipeVolumeMIP,mafPipe);

           medPipeVolumeMIP();
  virtual ~medPipeVolumeMIP();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_MIP_WIDGET_ID
  {
    ID_RESAMPLE_FACTOR = Superclass::ID_LAST,
    ID_LUT_CHOOSER,
    ID_LAST
  };

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

  void SetResampleFactor(double value);
  double GetResampleFactor(){return m_ResampleFactor;};

protected:
  /** 
  Given a color LUT, generate color transfer function and opacity transfer function*/
  void UpdateMIPFromLUT();

  virtual mmgGui  *CreateGui();

  double					  m_ResampleFactor;
  vtkImageResample			  *m_ResampleFilter;	
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkVolumeRayCastMIPFunction *m_MIPFunction;
  vtkVolumeRayCastMapper      *m_VolumeMapper;
  vtkVolume					  *m_Volume;
  vtkLookupTable      *m_ColorLUT;

};
#endif // __medPipeVolumeMIP_H__
