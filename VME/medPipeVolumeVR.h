/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeVR.h,v $
  Language:  C++
  Date:      $Date: 2008-07-25 11:19:42 $
  Version:   $Revision: 1.4 $
  Authors:   Daniele Giunchi
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medPipeVolumeVR_H__
#define __medPipeVolumeVR_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafPipe.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkActor;
class vtkColorTransferFunction;
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkProperty;
class vtkVolumeTextureMapper2D;
class vtkLODProp3D;
class vtkImageShiftScale;
class vtkProbeFilter;
class vtkRectilinearGrid; 
class vtkStructuredPoints;
class vtkImageResample;
//----------------------------------------------------------------------------
// medPipeVolumeVR :
//----------------------------------------------------------------------------
class medPipeVolumeVR : public mafPipe
{
public:
  mafTypeMacro(medPipeVolumeVR,mafPipe);

           medPipeVolumeVR();
  virtual ~medPipeVolumeVR();

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
  void Show(bool show); 
  virtual void Select(bool select);  
  
  /** Get color transfer function of the volume. */
	vtkColorTransferFunction *GetColorTransferFunction() {return m_ColorTransferFunction;};
	
	/** Get opacity transfer function of the volume. */
  vtkPiecewiseFunction *GetOpacityTransferFunction() {return m_PiecewiseFunction;};
	
	/** Get gradient transfer function of the volume. */
  vtkPiecewiseFunction *GetGradientTransferFunction() {return m_GradientFunction;};
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

	void SetResampleFactor(double value);
	double GetResampleFactor();

protected:
  virtual mafGUI  *CreateGui();

  vtkColorTransferFunction		*m_ColorTransferFunction;
  vtkPiecewiseFunction				*m_PiecewiseFunction;
  vtkPiecewiseFunction				*m_GradientFunction;

  vtkVolumeProperty						*m_VolumePropertyHigh;
  vtkVolumeTextureMapper2D		*m_VolumeTextureMapperHigh;

  vtkVolumeProperty						*m_VolumePropertyLow;
  vtkVolumeTextureMapper2D		*m_VolumeTextureMapperLow; 
  
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
#endif // __medPipeVolumeVR_H__
