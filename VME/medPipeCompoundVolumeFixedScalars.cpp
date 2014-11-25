/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: medPipeCompoundVolumeFixedScalars.cpp,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 09:58:24 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2009 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
#include "medPipeCompoundVolumeFixedScalars.h"
#include "medGUIDynamicVP.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeCompoundVolumeFixedScalars);
mafCxxTypeMacro(medPipeCompoundVolumeIsosurface);
mafCxxTypeMacro(medPipeCompoundVolumeDRR);
mafCxxTypeMacro(medPipeCompoundVolumeMIP);
mafCxxTypeMacro(medPipeCompoundVolumeVR);
//----------------------------------------------------------------------------


//------------------------------------------------------------------------
//Called from CreatePageGroups to create groups for scalars. */
/*virtual*/ bool medPipeCompoundVolumeFixedScalars::CreateScalarPageGroups()
//------------------------------------------------------------------------
{ 
  if (!Superclass::CreateScalarPageGroups()) 
    return false;

  PAGE_GROUP& group = m_PageGroups[m_PageGroups.size() - 1];
  group.bCanAddNewPages = false;
  group.bPageCanBeClosed = false;
  group.bNameCanBeChanged = false;    
  group.bVPCanBeChanged = false;
  group.nDefaultPipeIndex = GetVisualPipeIndex(
    GetDefaultScalarVisualPipe(), GetScalarVisualPipes());
  return true;
}

//------------------------------------------------------------------------
//Called from CreatePageGroups to create groups for vectors. */
/*virtual*/ bool medPipeCompoundVolumeFixedScalars::CreateVectorPageGroups()
//------------------------------------------------------------------------
{
    if (!Superclass::CreateVectorPageGroups()) 
      return false;
    
    PAGE_GROUP& group = m_PageGroups[m_PageGroups.size() - 1];
    group.bCanAddNewPages = false;
    group.bPageCanBeClosed = false;
    group.bNameCanBeChanged = false;
    group.nDefaultPipeIndex = GetVisualPipeIndex(
      GetDefaultVectorVisualPipe(), GetVectorVisualPipes());
    
    return true;
}

//------------------------------------------------------------------------
//Called from CreatePageGroups to create groups for tensors.
/*virtual*/ bool medPipeCompoundVolumeFixedScalars::CreateTensorPageGroups()
//------------------------------------------------------------------------
{
  if (!Superclass::CreateTensorPageGroups()) 
    return false;

    PAGE_GROUP& group = m_PageGroups[m_PageGroups.size() - 1];
    group.bCanAddNewPages = false;
    group.bPageCanBeClosed = false;
    group.bNameCanBeChanged = false;  
    group.nDefaultPipeIndex = GetVisualPipeIndex(
      GetDefaultTensorVisualPipe(), GetTensorVisualPipes());

  return true;
}

//------------------------------------------------------------------------
//Returns the currently constructed scalar visual pipe.
/*virtual*/ mafPipe* medPipeCompoundVolumeFixedScalars::GetCurrentScalarVisualPipe()
//------------------------------------------------------------------------
{
  if (GetDefaultScalarVisualPipe() == NULL)
    return NULL; //we have no scalar pipe by the default

  //scalar page is always the first one (if it exists)
  medGUIDynamicVP* page = NULL;
  if (m_FirstPage != NULL)
    page = m_FirstPage;
  else if (m_Notebook && m_Notebook->GetPageCount() > 0)
    page = (medGUIDynamicVP*)m_Notebook->GetPage(0);
    
  mafPipe* pipe;
  if (page == NULL || (pipe = page->GetCurrentVisualPipe()) == NULL)
    return NULL;  //no page available or no visual pipe
  
  return strcmp(pipe->GetTypeName(), 
    GetDefaultScalarVisualPipe()) == 0 ? pipe : NULL;
}