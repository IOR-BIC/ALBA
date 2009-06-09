/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeCompoundVolumeFixedScalars.h,v $ 
  Language: C++ 
  Date: $Date: 2009-06-09 16:04:32 $ 
  Version: $Revision: 1.1.2.2 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef medPipeCompoundVolumeFixedScalars_h__
#define medPipeCompoundVolumeFixedScalars_h__

#include "medPipeCompoundVolume.h"

/** Scalar visual pipe is fixed and cannot be changed. Vector visual pipe
and tensor visual pipe can be changed but only one change is available.
This class serves for a backward compatibility. */
class medPipeCompoundVolumeFixedScalars : public medPipeCompoundVolume
{
public:
  mafTypeMacro(medPipeCompoundVolumeFixedScalars, medPipeCompoundVolume);

public:	
  medPipeCompoundVolumeFixedScalars() {
  }

  virtual ~medPipeCompoundVolumeFixedScalars () {
  }

public:
  /** Returns the currently constructed scalar visual pipe. */
  virtual mafPipe* GetCurrentScalarVisualPipe();

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
    return "medPipeVectorFieldGlyphs";  //no visual pipe available
  }

  /** Gets the default scalar visual pipe */
  virtual const char* GetDefaultTensorVisualPipe() {
    return NULL;  //no visual pipe available
  }
};

/** Override for Isosurface */
class medPipeCompoundVolumeIsosurface : public medPipeCompoundVolumeFixedScalars
{
public:
  mafTypeMacro(medPipeCompoundVolumeIsosurface, medPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "mafPipeIsosurface";  //no visual pipe available
  }
};

/** Override for MIP */
class medPipeCompoundVolumeDRR : public medPipeCompoundVolumeFixedScalars
{
public:
  mafTypeMacro(medPipeCompoundVolumeDRR, medPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "medPipeVolumeDRR";  //no visual pipe available
  }
};

/** Override for MIP */
class medPipeCompoundVolumeMIP : public medPipeCompoundVolumeFixedScalars
{
public:
  mafTypeMacro(medPipeCompoundVolumeMIP, medPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "medPipeVolumeMIP";  //no visual pipe available
  }
};

/** Override for VR */
class medPipeCompoundVolumeVR : public medPipeCompoundVolumeFixedScalars
{
public:
  mafTypeMacro(medPipeCompoundVolumeVR, medPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "medPipeVolumeVR";  //no visual pipe available
  }
};

#endif // medPipeCompoundVolumeFixedScalars_h__