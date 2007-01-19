/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeVolumeVR.h,v $
  Language:  C++
  Date:      $Date: 2007-01-19 15:24:10 $
  Version:   $Revision: 1.2 $
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
    ID_LAST
  };

  virtual void Create(mafSceneNode *n);
  void Show(bool show); 
  virtual void Select(bool select); 

	vtkActor    *m_act_a;
	vtkActor    *m_sel_a;
	bool         m_created; 
  
  /** Get color transfer function of the volume. */
	vtkColorTransferFunction *GetColorTransferFunction() {return v_ctf;};
	
	/** Get opacity transfer function of the volume. */
  vtkPiecewiseFunction *GetOpacityTransferFunction() {return v_pf;};
	
	/** Get gradient transfer function of the volume. */
  vtkPiecewiseFunction *GetGradientTransferFunction() {return v_gf;};
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
  
	double m_range[2];
	double	m_UnsignRange[2];
	double	m_Minimum;	
	double	m_Maximum;
	bool	m_Rescale;

protected:
  virtual mmgGui  *CreateGui();

  vtkColorTransferFunction		*v_ctf;
  vtkPiecewiseFunction			*v_pf;
  vtkPiecewiseFunction			*v_gf;

  vtkVolumeProperty				*v_vpHi;
  vtkVolumeTextureMapper2D		*v_vtmHi;

  vtkVolumeProperty				*v_vpLo;
  vtkVolumeTextureMapper2D		*v_vtmLo; 
  
  vtkLODProp3D						*v_lod;
  vtkProperty						*v_lodP;
  vtkImageShiftScale				*v_imageshift;

  vtkStructuredPoints			*v_structuredImage;
  vtkProbeFilter					*v_probe;

  bool	   m_isStructured;
  bool	   m_SliceSet;
  int			m_NumberOfSlices;
};
#endif // __medPipeVolumeVR_H__
