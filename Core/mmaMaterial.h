/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: mmaMaterial
 Authors: Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mmaMaterial_H__
#define __mmaMaterial_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaAttribute.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkProperty;
class vtkLookupTable;
class vtkImageData;

//----------------------------------------------------------------------------
// mmaMaterial:
//----------------------------------------------------------------------------
/**  */  
class ALBA_EXPORT mmaMaterial : public albaAttribute
{
public:
	         mmaMaterial();
  virtual ~mmaMaterial();

  albaTypeMacro(mmaMaterial, albaAttribute);
  
  enum MATERIAL_TYPE
  {
    USE_VTK_PROPERTY,
    USE_LOOKUPTABLE,
    USE_TEXTURE
  };

  enum TEXTURE_MAP_MODE
  {
    PLANE_MAPPING = 0,
    CYLINDER_MAPPING,
    SPHERE_MAPPING
  };

	/** Build the material icon. */
  wxBitmap *MakeIcon();
  
  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0) const;

  /** Copy the contents of another Meter attribute into this one. */
  virtual void DeepCopy(const albaAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const albaAttribute *a);

  /** Set the texture image to map on the surface */
  void SetMaterialTexture(vtkImageData *tex);

  /** Return the texture set as vtkImageData */
  vtkImageData *GetMaterialTexture();

  /** Return the vme's id representing the texture */
  int GetMaterialTextureID();

  /** Set the albaVMEImage id to use as texture to map on the surface */
  void SetMaterialTexture(int tex_id);

  /** Apply shading parameters to the vtkProperty */
  virtual void UpdateProp();

  /** This method updates member variables from Lookup Table*/
  void UpdateFromLut();

  vtkProperty        *m_Prop;
  vtkLookupTable     *m_ColorLut;

  albaString m_MaterialName;
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

  double    m_HueRange[2];
  double    m_SaturationRange[2];
  double    m_TableRange[2];
  int       m_NumValues;
  int       m_MaterialType;
  int       m_TextureMappingMode;

protected:
  vtkImageData *m_TextureImage;
  int           m_TextureID;

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);
};
#endif
