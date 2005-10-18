/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMSFImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-18 21:52:33 $
  Version:   $Revision: 1.6 $
  Authors:   Marco Petrone - Paolo Quadrani
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafIncludeWX.h" // to be removed

#include "mafMSFImporter.h"
#include "mafVMERoot.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEGeneric.h"
#include "mafTagArray.h"
#include "mafTagItem.h"
#include "mafVMEItemVTK.h"
#include "mafDataVectorVTK.h"
#include "mmuUtility.h"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mafMatrixVector.h"
#include "mafVMEGroup.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mmaMaterial.h"

//------------------------------------------------------------------------------
// mmuMSF1xDocument
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int mmuMSF1xDocument::InternalStore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  mafErrorMessage("Writing MSF 1.x files is not supported!");
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::InternalRestore(mafStorageElement *node)
//------------------------------------------------------------------------------
{
  ///////////////////////////////////////////////
  // code to import the file from old MSF file //
  ///////////////////////////////////////////////
  mafString root_name;
  if (node->GetAttribute("Name",root_name))
    m_Root->SetName(root_name);

  mafID max_item_id;
  if (node->GetAttributeAsInteger("MaxItemId",max_item_id))
    m_Root->SetMaxItemId(max_item_id);
  

  mafStorageElement::ChildrenVector children;
  children = node->GetChildren();

  for (int i=0;i<children.size();i++)
  {
    if (mafCString("TArray") == children[i]->GetName())
    {
      if (RestoreTagArray(children[i],m_Root->GetTagArray()) != MAF_OK)
      {
        mafErrorMacro("MSFImporter: error restoring Tag Array of node: \""<<m_Root->GetName()<<"\"");
      }
    }
    else if (mafCString("VME") == children[i]->GetName())
    {
      mafVME *child_vme=RestoreVME(children[i],m_Root);
      if (child_vme==NULL)
      {
        mafErrorMacro("Error while restoring a VME (parent is the root)");
      }
      else
      {
        m_Root->AddChild(child_vme);
      }
    }    
  }
  
  return m_Root->Initialize();
}

//------------------------------------------------------------------------------
mafVME *mmuMSF1xDocument::RestoreVME(mafStorageElement *node, mafVME *parent)
//------------------------------------------------------------------------------
{
  mafVME *vme = NULL;
  // restore due attributes
  mafString vme_type;
  if (node->GetAttribute("Type",vme_type))
  {
    vme = CreateVMEInstance(vme_type);
    //assert(vme);
    if (!vme)
      return NULL;
    
    mafString vme_name;
    if (node->GetAttribute("Name",vme_name))
    {
      vme->SetName(vme_name);
  
      // traverse children and restore TagArray, MatrixVector and VMEItems 
      mafStorageElement::ChildrenVector children;
      children = node->GetChildren();

      for (int i=0;i<children.size();i++)
      {
        // Restore a TagArray element
        if (mafCString("TArray") == children[i]->GetName())
        {
          if (RestoreTagArray(children[i],vme->GetTagArray()) != MAF_OK)
          {
            mafErrorMacro("MSFImporter: error restoring Tag Array of node: \""<<vme->GetName()<<"\"");
            return NULL;
          }

          /////////////////////////////////////////// 
          // here should process VME-specific tags //
          ///////////////////////////////////////////
          mafTagArray *ta = vme->GetTagArray();
          if (ta->IsTagPresent("material"))
          {
            RestoreMaterial(vme);
          }
          if (vme_type == "mflVMELandmarkCloud")
          {
            int num_lm = ((mafVMELandmarkCloud *)vme)->GetNumberOfLandmarks();
            double rad = ((mafVMELandmarkCloud *)vme)->GetRadius();
          }
        }

        // restore VME-Item element
        else if (mafCString("VItem") == children[i]->GetName())
        {
          if (RestoreVItem(children[i],vme) != MAF_OK)
          {
            mafErrorMacro("MSFImporter: error restoring VME-Item of node: \""<<vme->GetName()<<"\"");
            return NULL;
          }
        }

        // restore MatrixVector element
        else if (mafCString("VMatrix") == children[i]->GetName())
        {
          mafVMEGenericAbstract *vme_generic = mafVMEGenericAbstract::SafeDownCast(vme);
          assert(vme_generic);
          if (RestoreVMatrix(children[i],vme_generic->GetMatrixVector()) != MAF_OK)
          {
            mafErrorMacro("MSFImporter: error restoring VME-Item of node: \""<<vme->GetName()<<"\"");
            return NULL;
          }
        }

        // restore children VMEs
        else if (mafCString("VME") == children[i]->GetName())
        {
          mafVME *child_vme=RestoreVME(children[i],vme);
          if (child_vme==NULL)
          {
            mafErrorMacro("MSFImporter: error restoring child VME (parent=\""<<vme->GetName()<<"\")");
            continue;
          }
          if (vme_type!="mafVMELink" && vme_type!="mafVMEAlias")
          {
            // add the new VME as a child of the given parent node
            if (vme_type == "mflVMELandmarkCloud" && child_vme->IsMAFType(mafVMELandmark))
            {
              ((mafVMELandmarkCloud *)vme)->SetLandmark((mafVMELandmark *)child_vme);
              child_vme->Delete();
              child_vme = NULL;
            }
            else
            {
              vme->AddChild(child_vme);
            }
          }
          else
          {
            // for VME-link and VME-alias we simply need to create links in current VME

            // ... to be implemented
          }
        } 
      }
    } // Name
  } // Type
  
  return vme;
}

//------------------------------------------------------------------------------
mafVME *mmuMSF1xDocument::CreateVMEInstance(mafString &name)
//------------------------------------------------------------------------------
{
  if (
    name == "mafVMEGeneric"         ||
    name == "mflVMEExternalData"    ||
    name == "mflVMEAlias"           ||
    name == "mflVMELink"
    )
  {
    return mafVMEGeneric::New();
  }
  else if (name == "mflVMEGroup")
  {
    return mafVMEGroup::New();
  }
  else if (name == "mflVMESurface")
  {
    return mafVMESurface::New();
  }
  else if (name == "mflVMEGrayVolume")
  {
    return mafVMEVolumeGray::New();
  }
  else if (name == "mflVMELandmarkCloud")
  {
    return mafVMELandmarkCloud::New();
  }
  else if (name == "mflVMELandmark")
  {
    return mafVMELandmark::New();
  }
  else
  {
    mafErrorMacro("Unknown VME type: \""<<name.GetCStr()<<"\"");
    return NULL;
  }
}

//------------------------------------------------------------------------------
void mmuMSF1xDocument::RestoreMaterial(mafVME *vme)
//------------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)vme->GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    vme->SetAttribute("MaterialAttributes", material);
  }

  mafTagItem *mat_item = vme->GetTagArray()->GetTag("material");
  material->m_MaterialName =  mat_item->GetComponent(MAT_NAME);
  material->m_Ambient[0] = mat_item->GetComponentAsDouble(MAT_AMBIENT_R);
  material->m_Ambient[1] = mat_item->GetComponentAsDouble(MAT_AMBIENT_G);
  material->m_Ambient[2] = mat_item->GetComponentAsDouble(MAT_AMBIENT_B);
  material->m_AmbientIntensity = mat_item->GetComponentAsDouble(MAT_AMBIENT_INTENSITY);
  material->m_Diffuse[0] = mat_item->GetComponentAsDouble(MAT_DIFFUSE_R);
  material->m_Diffuse[1] = mat_item->GetComponentAsDouble(MAT_DIFFUSE_G);
  material->m_Diffuse[2] = mat_item->GetComponentAsDouble(MAT_DIFFUSE_B);
  material->m_DiffuseIntensity = mat_item->GetComponentAsDouble(MAT_DIFFUSE_INTENSITY);
  material->m_Specular[0] = mat_item->GetComponentAsDouble(MAT_SPECULAR_R);
  material->m_Specular[1] = mat_item->GetComponentAsDouble(MAT_SPECULAR_G);
  material->m_Specular[2] = mat_item->GetComponentAsDouble(MAT_SPECULAR_B);
  material->m_SpecularIntensity = mat_item->GetComponentAsDouble(MAT_SPECULAR_INTENSITY);
  material->m_SpecularPower = mat_item->GetComponentAsDouble(MAT_SPECULAR_POWER);
  material->m_Opacity = mat_item->GetComponentAsDouble(MAT_OPACITY);
  material->m_Representation = (int)mat_item->GetComponentAsDouble(MAT_REPRESENTATION);
  material->UpdateProp();
  vme->GetTagArray()->DeleteTag("material");
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::RestoreVItem(mafStorageElement *node, mafVME *vme)
//------------------------------------------------------------------------------
{
  mafTimeStamp item_time;
  if (node->GetAttributeAsDouble("TimeStamp",item_time))
  {
    mafString data_type;
    if (node->GetAttribute("DataType",data_type))
    {
      mafID item_id;
      if (node->GetAttributeAsInteger("Id",item_id))
      {
        mafString data_file;
        if (node->GetAttribute("DataFile",data_file))
        {
          mafSmartPointer<mafVMEItemVTK> vitem;
          mafStorageElement *tarray=node->FindNestedElement("TArray");
          if (tarray)
          {
            if (RestoreTagArray(tarray,vitem->GetTagArray())==MAF_OK)
            {
              mafVMEGeneric *vme_generic=mafVMEGeneric::SafeDownCast(vme);
              assert(vme_generic);
              vme_generic->GetDataVector()->AppendItem(vitem);

              /////////////////////////////////////////////// 
              // here should process VMEItem-specific tags //
              ///////////////////////////////////////////////
              mafTagArray *ta = vitem->GetTagArray();
              if (ta->IsTagPresent("MFL_CRYPTING"))
              {
                mafTagItem *ti = ta->GetTag("MFL_CRYPTING");
                vme->SetCrypting((int)ti->GetValueAsDouble());
                ta->DeleteTag("MFL_CRYPTING");
              }
              if (ta->IsTagPresent("VTK_DATASET_BOUNDS"))
              {
                double *b = vitem->GetBounds();
                mafTagItem *ti = ta->GetTag("VTK_DATASET_BOUNDS");
                for (int c=0;c<6;c++)
                {
                  b[c] = ti->GetComponentAsDouble(c);
                }
                ta->DeleteTag("VTK_DATASET_BOUNDS");
              }
            }
          } // tarray
          vitem->SetTimeStamp(item_time);
          vitem->SetId(item_id);
          vitem->SetURL(data_file);
          return MAF_OK;
        } // DataFile
      } // Id
    } // DataType
  } // TimeStamp
  return MAF_ERROR;
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::RestoreVMatrix(mafStorageElement *node, mafMatrixVector *vmatrix)
//------------------------------------------------------------------------------
{
  // restore single matrices
  mafStorageElement::ChildrenVector children;
  children = node->GetChildren();

  for (int i = 0;i<children.size();i++)
  {
    assert(mafCString("Matrix") == children[i]->GetName());

    mafSmartPointer<mafMatrix> matrix;
    int restored_matrix = children[i]->RestoreMatrix(matrix);
    if (restored_matrix != MAF_ERROR)
    {
      vmatrix->AppendKeyMatrix(matrix);
    }
    else
    {
      return MAF_ERROR;
    }
  }
  return MAF_OK;
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::RestoreTagArray(mafStorageElement *node, mafTagArray *tarray)
//------------------------------------------------------------------------------
{
  mafStorageElement::ChildrenVector children;
  children = node->GetChildren();

  for (int i = 0;i<children.size();i++)
  {
    if (mafCString("TItem") == children[i]->GetName())
    {
      mafID num_of_comps;
      if (children[i]->GetAttributeAsInteger("Mult",num_of_comps))
      {
        mafString tag_name;
        if (children[i]->GetAttribute("Tag",tag_name))
        {
          mafString tag_type;
          if (children[i]->GetAttribute("Type",tag_type))
          {
            mafTagItem titem;
            titem.SetNumberOfComponents(num_of_comps);
            titem.SetName(tag_name);
    
            if (tag_type=="NUM")
            {
              titem.SetType(MAF_NUMERIC_TAG);
            }
            else if (tag_type=="STR")
            {
              titem.SetType(MAF_STRING_TAG);
            }
            else if (tag_type=="MIS")
            {
              titem.SetType(MAF_MISSING_TAG);
            }
            else
            {
              titem.SetType(atof(tag_type));
            }

            mafStorageElement::ChildrenVector tag_comps;
            tag_comps = children[i]->GetChildren();
            int idx=0;
            for (int n = 0;n<tag_comps.size();n++)
            {
              if (mafCString("TC")==tag_comps[n]->GetName())
              {
                mafString tc;
                tag_comps[n]->RestoreText(tc);
                titem.SetComponent(tc,idx);
                idx++;
              }
              else
              {
                mafErrorMacro("Error parning a TItem element inside a TagArray: expected <TC> sub element, found <"<<tag_comps[n]->GetName()<<">");
              } 
            } 
            tarray->SetTag(titem);
          } // Type
        } // Tag
      } // Mult
      //mafErrorMacro("Error parning a TItem element inside a TagArray: missing required Attribute");
    } // TItem
  }
  return MAF_OK;
}

//------------------------------------------------------------------------------
// mafMSFImporter
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafMSFImporter)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafMSFImporter::mafMSFImporter()
//------------------------------------------------------------------------------
{
  SetVersion("1.0");
  SetFileType("MSF");
  SetDocument(new mmuMSF1xDocument); // create a MSF doc
  
}

//------------------------------------------------------------------------------
mafMSFImporter::~mafMSFImporter()
//------------------------------------------------------------------------------
{
  cppDEL(m_Document); // delete the document object
}

//------------------------------------------------------------------------------
void mafMSFImporter::SetRoot(mafVMERoot *root)
//------------------------------------------------------------------------------
{
  ((mmuMSF1xDocument *)m_Document)->SetRoot(root);
}

//------------------------------------------------------------------------------
mafVMERoot *mafMSFImporter::GetRoot()
//------------------------------------------------------------------------------
{
  return ((mmuMSF1xDocument *)m_Document)->GetRoot();
}

