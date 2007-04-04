/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeMIP.h,v $
  Language:  C++
  Date:      $Date: 2007-04-04 10:07:58 $
  Version:   $Revision: 1.6 $
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
class vtkImageResample;
class vtkImageMedian3D;

class vtkVolume;

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
    ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_RESAMPLE_FACTOR,
    ID_LAST
  };

  virtual void Create(mafSceneNode *n);
  virtual void Select(bool select); 

	void SetResampleFactor(double value);
	double GetResampleFactor();

protected:
  /** 
  Given a color LUT, generate color transfer function and opacity transfer function*/
  void UpdateMIPFromLUT();

  virtual mmgGui  *CreateGui();

  vtkLookupTable              *m_ColorLUT;
  vtkImageCast                *m_Caster;
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkVolumeRayCastMIPFunction *m_MIPFunction;
  vtkVolumeRayCastMapper      *m_VolumeMapper;
  vtkVolumeRayCastMapper      *m_VolumeMapperLow;
  vtkLODProp3D                *m_VolumeLOD;

  vtkVolume                   *m_Volume;

	vtkImageResample						*m_ResampleFilter;
	vtkImageMedian3D						*m_Median;

	

  vtkActor               *m_SelectionActor;
  double  m_VolumeBounds[6];
  double  m_VolumeOrientation[3];
  double  m_VolumePosition[3];

	double	m_ResampleFactor;
};
#endif // __medPipeVolumeMIP_H__
