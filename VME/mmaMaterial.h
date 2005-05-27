/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaMaterial.h,v $
  Language:  C++
  Date:      $Date: 2005-05-27 13:51:59 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __mmaMaterial_H__
#define __mmaMaterial_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafAttribute.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkProperty;
class vtkVolumeProperty2;
class vtkLookupTable;
class vtkWindowLevelLookupTable;
class vtkImageData;

enum MATERIAL_TYPE
{
  USE_VTK_PROPERTY,
  USE_LOOKUPTABLE,
  USE_TEXTURE
};

//----------------------------------------------------------------------------
// mmaMaterial:
//----------------------------------------------------------------------------
/**  */  
class MAF_EXPORT mmaMaterial : public mafAttribute
{
public:
	         mmaMaterial();
  virtual ~mmaMaterial();

  mafTypeMacro(mmaMaterial, mafAttribute);
  
	/** Build the material icon. */
  wxBitmap *MakeIcon();
  
  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const mafAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const mafAttribute *a);

  /** Set the texture image to map on the surface */
  void SetTextureImage(vtkImageData *tex);

  /** Apply shading parameters to the vtkProperty */
  virtual void UpdateProp();

	vtkImageData       *m_TextureImage;
  vtkProperty        *m_Prop;
  vtkVolumeProperty2 *m_VolumeProp;
  vtkLookupTable     *m_ColorLut;
  vtkWindowLevelLookupTable *m_GrayLut;

  mafString m_Name;
  wxBitmap *m_Icon;
  double    m_Value;
  double    m_Ambient[3];
  double    m_AmbientIntensity;
  double    m_Diffuse[3];
  double    m_DiffuseIntensity;
  double    m_Specular[3];
  double    m_SpecularIntensity;
  double    m_SpecularPower;
  double    m_Opacity;
  double    m_Representation;

  double    m_Level_LUT;
  double    m_Window_LUT;
  double    m_HueRange[2];
  double    m_SaturationRange[2];
  double    m_TableRange[2];
  int       m_NumColors;

  int       m_TextureID;

  int       m_MaterialType;

protected:
  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};
#endif
