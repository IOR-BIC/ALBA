/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeMIP.h,v $
  Language:  C++
  Date:      $Date: 2011-07-13 07:38:59 $
  Version:   $Revision: 1.11.2.4 $
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
#include "medDefines.h"
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
class MED_EXPORT medPipeVolumeMIP : public mafPipe
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
