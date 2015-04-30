/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: mafPipeCompoundVolumeFixedScalars.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:01:46 $ 
  Version: $Revision: 1.1.2.5 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef mafPipeCompoundVolumeFixedScalars_h__
#define mafPipeCompoundVolumeFixedScalars_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafPipeCompoundVolume.h"

/** Scalar visual pipe is fixed and cannot be changed. Vector visual pipe
and tensor visual pipe can be changed but only one change is available.
This class serves for a backward compatibility. */
class MAF_EXPORT mafPipeCompoundVolumeFixedScalars : public mafPipeCompoundVolume
{
public:
  /** RTTI macro */
  mafTypeMacro(mafPipeCompoundVolumeFixedScalars, mafPipeCompoundVolume);

public:
  /** constructor */
  mafPipeCompoundVolumeFixedScalars() {
  }

  /** destructor */
  virtual ~mafPipeCompoundVolumeFixedScalars () {
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
    return "mafPipeVectorFieldGlyphs";  //no visual pipe available
  }

  /** Gets the default scalar visual pipe */
  virtual const char* GetDefaultTensorVisualPipe() {
    return NULL;  //no visual pipe available
  }
};

/** Override for Isosurface */
class MAF_EXPORT mafPipeCompoundVolumeIsosurface : public mafPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  mafTypeMacro(mafPipeCompoundVolumeIsosurface, mafPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "mafPipeIsosurface";  //no visual pipe available
  }
};

/** Override for MIP */
class MAF_EXPORT mafPipeCompoundVolumeDRR : public mafPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  mafTypeMacro(mafPipeCompoundVolumeDRR, mafPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "mafPipeVolumeDRR";  //no visual pipe available
  }
};

/** Override for MIP */
class MAF_EXPORT mafPipeCompoundVolumeMIP : public mafPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  mafTypeMacro(mafPipeCompoundVolumeMIP, mafPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "mafPipeVolumeMIP";  //no visual pipe available
  }
};

/** Override for VR */
class MAF_EXPORT mafPipeCompoundVolumeVR : public mafPipeCompoundVolumeFixedScalars
{
public:
  /** RTTI macro */
  mafTypeMacro(mafPipeCompoundVolumeVR, mafPipeCompoundVolumeFixedScalars);

protected:  
  /** Gets the default scalar visual pipe */
  /*virtual*/ const char* GetDefaultScalarVisualPipe() {
    return "mafPipeVolumeVR";  //no visual pipe available
  }
};

#endif // mafPipeCompoundVolumeFixedScalars_h__