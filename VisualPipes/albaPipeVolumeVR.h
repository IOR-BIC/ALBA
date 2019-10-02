/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVolumeVR
 Authors: Daniele Giunchi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaPipeVolumeVR_H__
#define __albaPipeVolumeVR_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkProperty;
class vtkALBAVolumeTextureMapper2D;
class vtkLODProp3D;
class vtkImageShiftScale;
class vtkProbeFilter;
class vtkRectilinearGrid; 
class vtkImageData;
class vtkImageResample;
/**
className: albaPipeVolumeVR
Pipe which uses vtkALBAVolumeTextureMapper2D and different Piecewise and ColorTransfer Functions.
*/
class ALBA_EXPORT albaPipeVolumeVR : public albaPipe
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeVolumeVR,albaPipe);
  /** constructor */
           albaPipeVolumeVR();
  /** destructor */
  virtual ~albaPipeVolumeVR();

  /** process events coming from gui */
  virtual void OnEvent(albaEventBase *alba_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_MIP_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_RESAMPLE_FACTOR,
    ID_LAST
  };

  /** Create. */
  virtual void Create(albaSceneNode *n);

  /** Show. */
  void Show(bool show); 

  /** Select. */
  virtual void Select(bool select);  
  
  /** Get color transfer function of the volume. */
	vtkColorTransferFunction *GetColorTransferFunction() {return m_ColorTransferFunction;};
	
	/** Get opacity transfer function of the volume. */
  vtkPiecewiseFunction *GetOpacityTransferFunction() {return m_PiecewiseFunction;};
	
	/** Get gradient transfer function of the volume. */
  vtkPiecewiseFunction *GetGradientTransferFunction() {return m_GradientFunction;};
  /** set the number of slices in z axes */
  void SetNumberPoints(int n);

	/** Add one point for the scalar transfer function. */
  void AddPoint(int scalarPoint,double opacity);
	
	/** Add one point for the color transfer function. */
  void AddPoint(int scalarPoint,double red,double green,double blue);
	
	/** Add one point for the gradient transfer function. */
  void AddGradPoint(int scalarPoint,double gradient);
  
	/** Remove one point for the scalar transfer function. */
	void RemoveOpacityPoint(int scalarPoint);
  
	/** Remove one point for the color transfer function. */
	void RemoveColorPoint(int scalarPoint);
  
	/** Remove one point for the gradient transfer function. */
	void RemoveOpacityGradPoint(int scalarPoint);

	/** Set ShadeOn HiRes prop. */
	void VolumePropertyHighShadeOn(void);

	/** Set ShadeOff HiRes prop. */
	void VolumePropertyHighShadeOff(void);

  /** Set resample factor. */
	void SetResampleFactor(double value);

  /** get resample factor. */
	double GetResampleFactor();

protected:
  /** create the gui widget for the pipe*/
  virtual albaGUI  *CreateGui();

  vtkColorTransferFunction		*m_ColorTransferFunction;
  vtkPiecewiseFunction				*m_PiecewiseFunction;
  vtkPiecewiseFunction				*m_GradientFunction;

  vtkVolumeProperty						*m_VolumePropertyHigh;
  vtkALBAVolumeTextureMapper2D		*m_VolumeTextureMapperHigh;

  vtkVolumeProperty						*m_VolumePropertyLow;
  vtkALBAVolumeTextureMapper2D		*m_VolumeTextureMapperLow; 
  
  vtkLODProp3D								*m_ActorLOD;
  vtkProperty									*m_PropertyLOD;
  vtkImageShiftScale					*m_ImageShift;

  vtkImageData					*m_StructuredImage;
  vtkProbeFilter							*m_Probe;

	vtkImageResample						*m_ResampleFilter;

	
  bool	  m_IsStructured;
  bool	  m_SliceSet;
	bool    m_Created;
	bool		m_Rescale;

  int			m_NumberOfSlices;

	double	m_ResampleFactor;
	double	m_Range[2];
	double	m_UnsignRange[2];
	double	m_Minimum;	
	double	m_Maximum;
};
#endif // __albaPipeVolumeVR_H__
