/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeCompoundVolume.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 10:01:46 $ 
  Version: $Revision: 1.1.2.4 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef albaPipeCompoundVolume_h__
#define albaPipeCompoundVolume_h__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipeCompound.h"

/** General class for Volumes with compound pipes */
class ALBA_EXPORT albaPipeCompoundVolume : public albaPipeCompound
{
public:
  albaTypeMacro(albaPipeCompoundVolume, albaPipeCompound);

protected:
  ///default volume pipes to be used with the 
  static const albaGUIDynamicVP::SUPPORTED_VP_ENTRY m_ScalarVolumeVP[];
  static const albaGUIDynamicVP::SUPPORTED_VP_ENTRY m_VectorVolumeVP[];
  static const albaGUIDynamicVP::SUPPORTED_VP_ENTRY m_TensorVolumeVP[];

public:	
  albaPipeCompoundVolume() {
  }

  virtual ~albaPipeCompoundVolume () {
  }

protected:
  /** Creates page groups. The inherited classes must override this method.
  This method is called from Create method prior to CreatePages.
  It defines the behavior for different VMEs.
  N.B. Groups MAY NOT change after this function is called. 
  In this overriden version it calls CreateScalarPageGroups,
  CreateVectorPageGroups and CreateTensorPageGroups*/
  /*virtual*/ void CreatePageGroups();

  /** Creates page groups for the given pipes with default new page name szPageName.
  If pPipes is NULL the operation does nothing and returns false. */
  virtual bool CreatePageGroups(const char* szPageName, 
    const albaGUIDynamicVP::SUPPORTED_VP_ENTRY* pPipes);

  /** Called from CreatePageGroups to create groups for scalars. */
  virtual bool CreateScalarPageGroups();

  /** Called from CreatePageGroups to create groups for vectors. */
  virtual bool CreateVectorPageGroups();

  /** Called from CreatePageGroups to create groups for tensors. */
  virtual bool CreateTensorPageGroups();

  /** Returns visual pipes available for the volume.
  To be overriden in inherited classes to support different things. */
  inline virtual const albaGUIDynamicVP::SUPPORTED_VP_ENTRY* GetScalarVisualPipes() {
    return m_ScalarVolumeVP;  //default
  }

  /** Returns visual pipes available for the volume.
  To be overriden in inherited classes to support different things. */
  inline virtual const albaGUIDynamicVP::SUPPORTED_VP_ENTRY* GetVectorVisualPipes() {
    return m_VectorVolumeVP;  //default
  }

  /** Returns visual pipes available for the volume.
  To be overriden in inherited classes to support different things. */
  inline virtual const albaGUIDynamicVP::SUPPORTED_VP_ENTRY* GetTensorVisualPipes() {
    return m_TensorVolumeVP;  //default is no tensor pipe
  }

  /** Helper that gets the index of pipe in pList, or -1 if no such pipe is available */
  int GetVisualPipeIndex(const char* pipe_classname, 
    const albaGUIDynamicVP::SUPPORTED_VP_ENTRY* pList);

};
#endif // albaPipeCompoundVolume_h__