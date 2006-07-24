/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmaMaterial.h,v $
  Language:  C++
  Date:      $Date: 2006-07-24 08:52:59 $
  Version:   $Revision: 1.5 $
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
  virtual void DeepCopy(const mafAttribute *a);

  /** Compare with another Meter attribute. */
  virtual bool Equals(const mafAttribute *a);

  /** Set the texture image to map on the surface */
  void SetMaterialTexture(vtkImageData *tex);

  /** Return the texture set as vtkImageData */
  vtkImageData *GetMaterialTexture();

  /** Return the vme's id representing the texture */
  int GetMaterialTextureID();

  /** Set the mafVMEImage id to use as texture to map on the surface */
  void SetMaterialTexture(int tex_id);

  /** Apply shading parameters to the vtkProperty */
  virtual void UpdateProp();

  vtkProperty        *m_Prop;
  vtkVolumeProperty2 *m_VolumeProp;
  vtkLookupTable     *m_ColorLut;
  vtkWindowLevelLookupTable *m_GrayLut;

  mafString m_MaterialName;
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
  int       m_NumValues;
  int       m_MaterialType;
  int       m_TextureMappingMode;

protected:
  vtkImageData *m_TextureImage;
  int           m_TextureID;

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);
};
#endif
