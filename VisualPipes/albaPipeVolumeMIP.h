/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeMIP
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVolumeMIP_H__
#define __albaPipeVolumeMIP_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class albaLODActor;
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkOutlineSource;
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
//class vtkColorTransferFunction;
class vtkImageCast;
/**
className: albaPipeVolumeMIP
Pipe which uses vtkVolumeRayCastMIPFunction with vtkALBAVolumeRayCastMapper and predefined Transfer Functions. 
*/
class ALBA_EXPORT albaPipeVolumeMIP : public albaPipe
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeVolumeMIP,albaPipe);
  /** constructor */
           albaPipeVolumeMIP();
  /** destructor */
  virtual ~albaPipeVolumeMIP();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_MIP_WIDGET_ID
  {
    ID_RESAMPLE_FACTOR = Superclass::ID_LAST,
    ID_LUT_CHOOSER,
    ID_LAST
  };
 
  /** pipe creation */
  virtual void Create(albaSceneNode *n);
  /** called when vme associated with the pipe is selected */
  virtual void Select(bool select); 

  /** set a resample factor of the rendered volume */
  void SetResampleFactor(double value);
  /** get sample factor for rendered volume */
  double GetResampleFactor(){return m_ResampleFactor;};

  /** Get assembly front */
  virtual vtkALBAAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  /** Get assembly back */
  virtual vtkALBAAssembly *GetAssemblyBack(){return m_AssemblyBack;};


protected:
  /** 
  Given a color LUT, generate color transfer function and opacity transfer function*/
  void UpdateMIPFromLUT();
  /** create gui widget for pipe */
  virtual albaGUI  *CreateGui();

  double					  m_ResampleFactor;
  vtkImageCast                *m_Caster;
  vtkImageResample			  *m_ResampleFilter;	
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  //vtkColorTransferFunction    *m_ColorTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkVolumeRayCastMIPFunction *m_MIPFunction;
  vtkVolumeRayCastMapper       *m_VolumeMapper;
  vtkVolume					  *m_Volume;
  vtkLookupTable      *m_ColorLUT;

	/** For vtkRectilinearGrid */
	vtkOutlineSource       *m_Box;
	vtkPolyDataMapper	     *m_Mapper;
	albaLODActor            *m_Actor;
	vtkOutlineCornerFilter *m_OutlineBox;

};
#endif // __albaPipeVolumeMIP_H__
