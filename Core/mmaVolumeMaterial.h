/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaVolumeMaterial
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mmaVolumeMaterial_H__
#define __mmaVolumeMaterial_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaAttribute.h"

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
class ALBA_EXPORT mmaVolumeMaterial : public albaAttribute
{
public:
	         mmaVolumeMaterial();
  virtual ~mmaVolumeMaterial();

  albaTypeMacro(mmaVolumeMaterial, albaAttribute);
  
  /** Print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const albaAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const albaAttribute *a);

  /** Call to update volume properties.*/
  void UpdateProp();

  /** Used to update all member variables from LUT or Transfer Functions.*/
  void UpdateFromTables();

  /** Used to apply gamma correction to lut preset, need preset in order to avoid recursive applying on the same lut*/
  void ApplyGammaCorrection(int preset = -1);

  vtkLookupTable            *m_ColorLut;
  vtkPiecewiseFunction      *m_OpacityTransferFunction;
  vtkPiecewiseFunction      *m_GradientTransferFunction;
  vtkColorTransferFunction  *m_ColorTransferFunction;
  vtkVolumeProperty         *m_VolumeProperty;
  vtkVolumeProperty2        *m_VolumeProperty2;

  albaString m_MaterialName; ///< Associate a name to the volume material, this will be visible into the .msf file

  double m_Level_LUT;
  double m_Window_LUT;
  double m_GammaCorrection;
  double m_HueRange[2];
  double m_SaturationRange[2];
  int    m_NumValues;
  int    m_InterpolationType; ///< Set the interpolation for the volume rendering: can be 0 (VTK_NEAREST_INTERPOLATION) or 1 (VTK_LINEAR_INTERPOLATION)
  int    m_Shade; ///< Store the shade parameter for volume rendering (can be 0 pr 1)


	/** Returns TableRange */
	double const *GetTableRange() { return m_TableRange; }

	/** Sets TableRange */
	void SetTableRange(double *tableRange) {
		m_TableRange[0] = tableRange[0]; m_TableRange[1] = tableRange[1];
	}
	void SetTableRange(double a, double b) {
		m_TableRange[0] = a; m_TableRange[1] = b;
	}

protected:
  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  int    m_NumOpacityValues;
  int    m_NumGradientValues;
  int    m_NumColorValues;

private:
	double m_TableRange[2];

};
#endif
