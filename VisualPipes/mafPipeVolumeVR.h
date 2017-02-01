/*=========================================================================

 Program: MAF2
 Module: mafPipeVolumeVR
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafPipeVolumeVR_H__
#define __mafPipeVolumeVR_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkProperty;
class vtkMAFVolumeTextureMapper2D;
class vtkLODProp3D;
class vtkImageShiftScale;
class vtkProbeFilter;
class vtkRectilinearGrid; 
class vtkStructuredPoints;
class vtkImageResample;
/**
className: mafPipeVolumeVR
Pipe which uses vtkMAFVolumeTextureMapper2D and different Piecewise and ColorTransfer Functions.
*/
class MAF_EXPORT mafPipeVolumeVR : public mafPipe
{
public:
  /** RTTI macro */
  mafTypeMacro(mafPipeVolumeVR,mafPipe);
  /** constructor */
           mafPipeVolumeVR();
  /** destructor */
  virtual ~mafPipeVolumeVR();

  /** process events coming from gui */
  virtual void OnEvent(mafEventBase *maf_event);

  /** IDs for the GUI */
  enum PIPE_VOLUME_MIP_WIDGET_ID
  {
    ID_LUT_CHOOSER = Superclass::ID_LAST,
		ID_RESAMPLE_FACTOR,
    ID_LAST
  };

  /** Create. */
  virtual void Create(mafSceneNode *n);

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
  virtual mafGUI  *CreateGui();

  vtkColorTransferFunction		*m_ColorTransferFunction;
  vtkPiecewiseFunction				*m_PiecewiseFunction;
  vtkPiecewiseFunction				*m_GradientFunction;

  vtkVolumeProperty						*m_VolumePropertyHigh;
  vtkMAFVolumeTextureMapper2D		*m_VolumeTextureMapperHigh;

  vtkVolumeProperty						*m_VolumePropertyLow;
  vtkMAFVolumeTextureMapper2D		*m_VolumeTextureMapperLow; 
  
  vtkLODProp3D								*m_ActorLOD;
  vtkProperty									*m_PropertyLOD;
  vtkImageShiftScale					*m_ImageShift;

  vtkStructuredPoints					*m_StructuredImage;
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
#endif // __mafPipeVolumeVR_H__
