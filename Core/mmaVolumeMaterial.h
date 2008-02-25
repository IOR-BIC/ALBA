/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaVolumeMaterial.h,v $
  Language:  C++
  Date:      $Date: 2008-02-25 19:41:36 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmaVolumeMaterial_H__
#define __mmaVolumeMaterial_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAttribute.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkLookupTable;
class vtkPiecewiseFunction;
class vtkColorTransferFunction;
class vtkVolumeProperty;
class vtkVolumeProperty2;

//----------------------------------------------------------------------------
// mmaVolumeMaterial:
//----------------------------------------------------------------------------
/**  */  
class MAF_EXPORT mmaVolumeMaterial : public mafAttribute
{
public:
	         mmaVolumeMaterial();
  virtual ~mmaVolumeMaterial();

  mafTypeMacro(mmaVolumeMaterial, mafAttribute);
  
  /** Print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const mafAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const mafAttribute *a);

  /** Call to update volume properties.*/
  void UpdateProp();

  /** Used to update all member variables from LUT or Transfer Functions.*/
  void UpdateFromTables();

  vtkLookupTable            *m_ColorLut;
  vtkPiecewiseFunction      *m_OpacityTransferFunction;
  vtkPiecewiseFunction      *m_GradientTransferFunction;
  vtkColorTransferFunction  *m_ColorTransferFunction;
  vtkVolumeProperty         *m_VolumeProperty;
  vtkVolumeProperty2        *m_VolumeProperty2;

  mafString m_MaterialName; ///< Associate a name to the volume material, this will be visible into the .msf file

  double m_Level_LUT;
  double m_Window_LUT;
  double m_HueRange[2];
  double m_SaturationRange[2];
  double m_TableRange[2];
  int    m_NumValues;
  int    m_InterpolationType; ///< Set the interpolation for the volume rendering: can be 0 (VTK_NEAREST_INTERPOLATION) or 1 (VTK_LINEAR_INTERPOLATION)
  int    m_Shade; ///< Store the shade parameter for volume rendering (can be 0 pr 1)

protected:
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  int    m_NumOpacityValues;
  int    m_NumGradientValues;
  int    m_NumColorValues;
};
#endif
