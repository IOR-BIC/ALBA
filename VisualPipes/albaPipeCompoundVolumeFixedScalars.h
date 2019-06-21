/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeCompoundVolumeFixedScalars.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:01:46 $ 
  Version: $Revision: 1.1.2.5 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef albaPipeCompoundVolumeFixedScalars_h__
#define albaPipeCompoundVolumeFixedScalars_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeCompoundVolume.h"

/** Scalar visual pipe is fixed and cannot be changed. Vector visual pipe
and tensor visual pipe can be changed but only one change is available.
This class serves for a backward compatibility. */
class ALBA_EXPORT albaPipeCompoundVolumeFixedScalars : public albaPipeCompoundVolume
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeCompoundVolumeFixedScalars, albaPipeCompoundVolume);

public:
  /** constructor */
  albaPipeCompoundVolumeFixedScalars() {
  }

  /** destructor */
  virtual ~albaPipeCompoundVolumeFixedScalars () {
  }

public:
  /** Returns the currently constructed scalar visual pipe. */
  virtual albaPipe* GetCurrentScalarVisualPipe();

protected:  
  /** Called from CreatePageGroups to create groups for scalars. */
  /*virtual*/ bool CreateScalarPageGroups();

  /** Called from CreatePageGroups to create groups for vectors. */
  /*virtual*/ bool CreateVectorPageGroups();

  /** Called from CreatePageGroups to create groups for tensors. */
  /*virtual*/ bool CreateTensorPageGroups();  

  /** Gets the default scalar visual pipe */
  virtual const char* GetDefaultScalarVisualPipe() {
    return NULL;  //no visual pipe available
  }

  /** Gets the default scalar visual pipe */
  virtual const char* GetDefaultVectorVisualPipe() {
    return "albaPipeVectorFieldGlyphs";  //no visual pipe available
  }

  /** Gets the default scalar visual pipe */
  virtual const char* GetDefaultTensorVisualPipe() {
    return NULL;  //no visual pipe available
  }
};

/** Override for Isosurface */
class ALBA_EXPORT albaPipeCompoundVolumeIsosurface : public albaPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeCompoundVolumeIsosurface, albaPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "albaPipeIsosurface";  //no visual pipe available
  }
};

/** Override for MIP */
class ALBA_EXPORT albaPipeCompoundVolumeDRR : public albaPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeCompoundVolumeDRR, albaPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "albaPipeVolumeDRR";  //no visual pipe available
  }
};

/** Override for MIP */
class ALBA_EXPORT albaPipeCompoundVolumeMIP : public albaPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeCompoundVolumeMIP, albaPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "albaPipeVolumeMIP";  //no visual pipe available
  }
};

/** Override for VR */
class ALBA_EXPORT albaPipeCompoundVolumeVR : public albaPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  albaTypeMacro(albaPipeCompoundVolumeVR, albaPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "albaPipeVolumeVR";  //no visual pipe available
  }
};

#endif // albaPipeCompoundVolumeFixedScalars_h__