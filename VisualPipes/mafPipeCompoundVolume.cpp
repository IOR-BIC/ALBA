/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: mafPipeCompoundVolume.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-06-12 16:34:48 $ 
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

#include "mafDecl.h"
#include "mafPipeCompoundVolume.h"
#include "mafVME.h"

#include "vtkDataSet.h"
#include "vtkPointData.h"


///visual pipes for volumes
/*static*/ const mafGUIDynamicVP::SUPPORTED_VP_ENTRY 
  mafPipeCompoundVolume::m_ScalarVolumeVP[] = 
{
  {"mafPipeIsosurface", "iso-surface"},
  {"mafPipeVolumeDRR", "DRR"},
  {"mafPipeVolumeMIP", "MIP"},
  {"mafPipeVolumeVR", "VR"},  
  {NULL, NULL},
};

///visual pipes for volumes
/*static*/ const mafGUIDynamicVP::SUPPORTED_VP_ENTRY 
mafPipeCompoundVolume::m_VectorVolumeVP[] = 
{
  {"mafPipeVectorFieldGlyphs", "glyphs"},  
  {"mafPipeVectorFieldSurface", "surface"},  
  {"mafPipeVectorFieldSlice", "slice"}, 
  {NULL, NULL},
};

///visual pipes for volumes
/*static*/ const mafGUIDynamicVP::SUPPORTED_VP_ENTRY 
mafPipeCompoundVolume::m_TensorVolumeVP[] = 
{
  {"mafPipeTensorFieldGlyphs", "glyphs"},  
  {"mafPipeTensorFieldSurface", "surface"},  
  {"mafPipeTensorFieldSlice", "slice"}, 
  {NULL, NULL},
};

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeCompoundVolume);
//----------------------------------------------------------------------------


//------------------------------------------------------------------------
//Creates page groups. The inherited classes must override this method.
//This method is called from Create method prior to CreatePages.
//It defines the behavior for different VMEs.
//N.B. Groups MAY NOT change after this function is called.
/*virtual*/ void mafPipeCompoundVolume::CreatePageGroups()
//------------------------------------------------------------------------
{
  CreateScalarPageGroups();
  CreateVectorPageGroups();
  CreateTensorPageGroups();
}

//------------------------------------------------------------------------
//Called from CreatePageGroups to create groups for scalars. */
/*virtual*/ bool mafPipeCompoundVolume::CreateScalarPageGroups()
//------------------------------------------------------------------------
{ 
  //check if the input Vme has some scalars to visualize
  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  if (ds != NULL)
  {
    ds->Update();

    vtkPointData* pd = ds->GetPointData();
    if (pd != NULL && pd->GetScalars() != NULL) {
      return CreatePageGroups("Scalars", GetScalarVisualPipes());      
    }
  }

  return false;
}

//------------------------------------------------------------------------
//Called from CreatePageGroups to create groups for vectors. */
/*virtual*/ bool mafPipeCompoundVolume::CreateVectorPageGroups()
//------------------------------------------------------------------------
{
  //check if the input Vme has some vectors to visualize
  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  if (ds != NULL)
  {
    ds->Update();

    vtkPointData* pd = ds->GetPointData();
    if (pd != NULL)
    {
      bool bHasVectors = pd->GetVectors() != NULL || pd->GetNormals() != NULL;
      if (!bHasVectors)
      {
        //try to find it
        int nCount = pd->GetNumberOfArrays();
        for (int i = 0; i < nCount; i++)
        {
          if (pd->GetArray(i)->GetNumberOfComponents() == 3)
          {
            bHasVectors = true;
            break;
          }
        }
      }

      if (bHasVectors){      
        return CreatePageGroups("Vectors", GetVectorVisualPipes());        
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------
//Called from CreatePageGroups to create groups for tensors.
/*virtual*/ bool mafPipeCompoundVolume::CreateTensorPageGroups()
//------------------------------------------------------------------------
{
  //check if the input Vme has some vectors to visualize
  vtkDataSet* ds = m_Vme->GetOutput()->GetVTKData();
  if (ds != NULL)
  {
    ds->Update();

    vtkPointData* pd = ds->GetPointData();
    if (pd != NULL)
    {
      bool bHasTensors = pd->GetTensors() != NULL;
      if (!bHasTensors)
      {
        //try to find it
        int nCount = pd->GetNumberOfArrays();
        for (int i = 0; i < nCount; i++)
        {
          if (pd->GetArray(i)->GetNumberOfComponents() > 3)
          {
            bHasTensors = true;
            break;
          }
        }
      }
    
      if (bHasTensors){
        return CreatePageGroups("Tensors", GetTensorVisualPipes());        
      }
    }
  }

  return false;
}

//------------------------------------------------------------------------
//Creates page groups for the given pipes with default new page name szPageName.
//If pPipes is NULL the operation does nothing.
/*virtual*/bool mafPipeCompoundVolume::CreatePageGroups(const char* szPageName, 
                         const mafGUIDynamicVP::SUPPORTED_VP_ENTRY* pPipes)
//------------------------------------------------------------------------
{
  if (pPipes == NULL)
    return false; //no visual pipe

  PAGE_GROUP group;
  group.szClassName = "mafVMEOutputVolume";
  group.bVMEOutput = true;

  group.pPipes = pPipes;
  group.nDefaultPipeIndex = -1;
  group.szDefaultName = szPageName;
  
  group.bVPCanBeChanged = true;
  group.bVPSingleChange = true;
  group.bNameCanBeChanged = true;
  group.bNameSingleChange = true;
  group.bPageCanBeClosed = true;
  group.bCanAddNewPages = true;

  m_PageGroups.push_back(group);
  return true;
}

//------------------------------------------------------------------------
//Helper that gets the index of pipe in pList, or -1 if no such pipe is available
int mafPipeCompoundVolume::GetVisualPipeIndex(
  const char* pipe_classname, const mafGUIDynamicVP::SUPPORTED_VP_ENTRY* pList)
//------------------------------------------------------------------------
{
  if (pList == NULL || pipe_classname == NULL)
    return -1;  //invalid

  int nIndex = 0;
  while (pList[nIndex].szClassName != NULL)
  {
    if (strcmp(pipe_classname, pList[nIndex].szClassName) == 0)
      return nIndex;  //we found it

    nIndex++;
  }

  return -1;
}