/*=========================================================================

 Program: MAF2
 Module: medPipeVolumeMIP
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medPipeVolumeMIP_H__
#define __medPipeVolumeMIP_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class mafLODActor;
class vtkOutlineCornerFilter;
class vtkPolyDataMapper;
class vtkOutlineSource;
class vtkImageCast;
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkVolumeRayCastMIPFunction;
class vtkMAFVolumeRayCastMapper;
class vtkActor;
class vtkLODProp3D;
class vtkLookupTable;
class vtkVolume;
class vtkImageResample;
class vtkLookupTable;
//class vtkColorTransferFunction;
class vtkImageCast;
/**
className: medPipeVolumeMIP
Pipe which uses vtkVolumeRayCastMIPFunction with vtkMAFVolumeRayCastMapper and predefined Transfer Functions. 
*/
class MAF_EXPORT medPipeVolumeMIP : public mafPipe
{
public:
  /** RTTI macro */
  mafTypeMacro(medPipeVolumeMIP,mafPipe);
  /** constructor */
           medPipeVolumeMIP();
  /** destructor */
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
 
  /** pipe creation */
  virtual void Create(mafSceneNode *n);
  /** called when vme associated with the pipe is selected */
  virtual void Select(bool select); 

  /** set a resample factor of the rendered volume */
  void SetResampleFactor(double value);
  /** get sample factor for rendered volume */
  double GetResampleFactor(){return m_ResampleFactor;};

  /** Get assembly front */
  virtual vtkMAFAssembly *GetAssemblyFront(){return m_AssemblyFront;};
  /** Get assembly back */
  virtual vtkMAFAssembly *GetAssemblyBack(){return m_AssemblyBack;};


protected:
  /** 
  Given a color LUT, generate color transfer function and opacity transfer function*/
  void UpdateMIPFromLUT();
  /** create gui widget for pipe */
  virtual mafGUI  *CreateGui();

  double					  m_ResampleFactor;
  vtkImageCast                *m_Caster;
  vtkImageResample			  *m_ResampleFilter;	
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  //vtkColorTransferFunction    *m_ColorTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkVolumeRayCastMIPFunction *m_MIPFunction;
  vtkMAFVolumeRayCastMapper      *m_VolumeMapper;
  vtkVolume					  *m_Volume;
  vtkLookupTable      *m_ColorLUT;

	/** For vtkRectilinearGrid */
	vtkOutlineSource       *m_Box;
	vtkPolyDataMapper	     *m_Mapper;
	mafLODActor            *m_Actor;
	vtkOutlineCornerFilter *m_OutlineBox;

};
#endif // __medPipeVolumeMIP_H__
