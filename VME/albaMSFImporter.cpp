/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaMSFImporter
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaIncludeWX.h" // to be removed

#include "albaMSFImporter.h"
#include "albaVMERoot.h"
#include "albaVMEGenericAbstract.h"
#include "albaVMEGeneric.h"
#include "albaTagArray.h"
#include "albaTagItem.h"
#include "albaVMEItemVTK.h"
#include "albaDataVector.h"
#include "albaUtility.h"
#include "albaStorable.h"
#include "albaStorageElement.h"
#include "mmaMaterial.h"
#include "mmaMeter.h"
#include "albaMatrixVector.h"

#include "albaVMEGroup.h"
#include "albaVMEImage.h"
#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEMeter.h"
#include "albaVMEPointSet.h"
#include "albaVMEProber.h"
#include "albaVMERefSys.h"
#include "albaVMESlicer.h"
#include "albaVMESurface.h"
#include "albaVMEVolumeGray.h"
#include "albaVMEVolumeRGB.h"
#include "albaVMEExternalData.h"

#include "albaVMEIterator.h"

#include <vector>

//------------------------------------------------------------------------------
// mmuMSF1xDocument
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int mmuMSF1xDocument::InternalStore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  albaErrorMessage("Writing MSF 1.x files is not supported!");
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::InternalRestore(albaStorageElement *node)
//------------------------------------------------------------------------------
{
  ///////////////////////////////////////////////
  // code to import the file from old MSF file //
  ///////////////////////////////////////////////
  albaString root_name;
  if (node->GetAttribute("Name",root_name))
    m_Root->SetName(root_name);

  albaID max_item_id;
  if (node->GetAttributeAsInteger("MaxItemId",max_item_id))
    m_Root->SetMaxItemId(max_item_id);
  

  albaStorageElement::ChildrenVector children;
  children = node->GetChildren();

  for (int i=0;i<children.size();i++)
  {
    if (albaCString("TArray") == children[i]->GetName())
    {
      if (RestoreTagArray(children[i],m_Root->GetTagArray()) != ALBA_OK)
      {
        albaErrorMacro("MSFImporter: error restoring Tag Array of node: \""<<m_Root->GetName()<<"\"");
      }
    }
    else if (albaCString("VME") == children[i]->GetName())
    {
      albaVME *child_vme=RestoreVME(children[i],m_Root);
      if (child_vme==NULL)
      {
        albaErrorMacro("Error while restoring a VME (parent is the root)");
      }
      else
      {
        m_Root->AddChild(child_vme);

        child_vme->UnRegister(NULL);
      }
    }    
  }
  albaVME *n = NULL;
  std::vector<albaVME *> link_list;
  albaVMEIterator *iter = m_Root->NewIterator();
  // iteration for updating VME's ID
  for (n = iter->GetFirstNode(); n; n=iter->GetNextNode())
  {
    n->UpdateId();
  }
  // iteration for setting up linked vme
  for (n = iter->GetFirstNode(); n;n=iter->GetNextNode())
  {
    if (n->IsALBAType(albaVMEGeneric) && n->GetTagArray()->IsTagPresent("mflVMELink"))
    {
      link_list.push_back(n);
      albaTagItem *tag = n->GetTagArray()->GetTag("VME_ALIAS_PATH");
      albaVME *linkedVME = this->ParsePath(m_Root, tag->GetValue());
      if (linkedVME != NULL)
      {
        albaID sub_id = -1;
        if (n->GetTagArray()->IsTagPresent("SUBLINK_ID"))
          sub_id = (albaID)n->GetTagArray()->GetTag("SUBLINK_ID")->GetValueAsDouble();

				if(sub_id == -1)
					n->GetParent()->SetLink(n->GetName(), linkedVME);
				else
					n->GetParent()->SetOldSubIdLink(n->GetName(), linkedVME->GetId(), sub_id);
      }
    }
  }
  iter->Delete();
  // remove all albaVMEGeneric representing links
  for (int l=0;l<link_list.size();l++)
  {
    link_list[l]->ReparentTo(NULL);
  }
  link_list.clear();
  return m_Root->Initialize();
}
//------------------------------------------------------------------------------
albaVME *mmuMSF1xDocument::ParsePath(albaVMERoot *root,const char *path)
//------------------------------------------------------------------------------
{
  const char *str=path;
  albaVME *node=NULL;

  if (albaString::StartsWith(str,"/MSF"))
  {
    str+=4; // jump to next token

    node=root;
    for (;albaString::StartsWith(str,"/VME[");)
    {
      int idx;
      if (sscanf(str,"/VME[%d]",&idx)==EOF)
      {
        albaErrorMacro("Error Parsing XPATH string: \""<<str<<"\"");
        return NULL;
      }
      node=node->GetChild(idx);  

      if (node==NULL)
      {
        albaWarningMacro("Corrupted Link");
        return NULL;
      }

      // OK... this is not a true Regular Expression pareser!
      const char *next=strchr(str,']');
      if (next)
      {
        str=next+1;
      }
      else
      {
        albaErrorMacro("Error Parsing XPATH string");
        return NULL;
      }
    }

    // if string was not yet finished... parse error!!!
    if ((*str!=0)&&(!albaString::Compare(str,"/")))
    {
      albaErrorMacro("Error Parsing XPATH string: \""<<str<<"\"");
      return NULL;
    }
  }
  return node;
}

//------------------------------------------------------------------------------
albaVME *mmuMSF1xDocument::RestoreVME(albaStorageElement *node, albaVME *parent)
//------------------------------------------------------------------------------
{
  albaVME *vme = NULL;
  // restore due attributes
  albaString vme_type;
  if (node->GetAttribute("Type",vme_type))
  {
    vme = CreateVMEInstance(vme_type);
    if (!vme)
      return NULL;

    albaString vme_name;

    if (node->GetAttribute("Name",vme_name))
    {
      vme->SetName(vme_name);
      // traverse children and restore TagArray, MatrixVector and VMEItems 
      albaStorageElement::ChildrenVector children;
      children = node->GetChildren();
      for (int i=0;i<children.size();i++)
      {
        // Restore a TagArray element
        //if (albaCString("TArray") == children[i]->GetName())
        if (albaString("TArray").Equals(children[i]->GetName()))
        {
          if (RestoreTagArray(children[i],vme->GetTagArray()) != ALBA_OK)
          {
            albaErrorMacro("MSFImporter: error restoring Tag Array of node: \""<<vme->GetName()<<"\"");
            return NULL;
          }

          /////////////////////////////////////////// 
          // here should process VME-specific tags //
          ///////////////////////////////////////////
          albaTagArray *ta = vme->GetTagArray();
          if (ta->IsTagPresent("material"))
          {
            RestoreMaterial(vme);
          }
          if (ta->IsTagPresent("ALBA_TOOL_VME"))
          {
            ta->DeleteTag("ALBA_TOOL_VME");
          }
          if (vme_type == "mflVMEMeter")
          {
            RestoreMeterAttribute(vme);
          }
          else if (vme_type == "mflVMELandmarkCloud" || vme_type == "mflVMERigidLandmarkCloud" || vme_type == "mflVMEDynamicLandmarkCloud")
          {
            int num_lm = ((albaVMELandmarkCloud *)vme)->GetNumberOfLandmarks();
            double rad = ((albaVMELandmarkCloud *)vme)->GetRadius();
          }
        }
        // restore VME-Item element
        else if (albaCString("VItem") == children[i]->GetName())
        {
          if (RestoreVItem(children[i],vme) != ALBA_OK)
          {
            albaErrorMacro("MSFImporter: error restoring VME-Item of node: \""<<vme->GetName()<<"\"");
            return NULL;
          }
        }

        // restore MatrixVector element
        else if (albaCString("VMatrix") == children[i]->GetName())
        {
          albaVMEGenericAbstract *vme_generic = albaVMEGenericAbstract::SafeDownCast(vme);
          if (vme_generic && RestoreVMatrix(children[i],vme_generic->GetMatrixVector()) != ALBA_OK)
          {
            albaErrorMacro("MSFImporter: error restoring VME-Item of node: \""<<vme->GetName()<<"\"");
            return NULL;
          }
          if (albaVMEGroup::SafeDownCast(vme))
          {
            albaSmartPointer<albaMatrix> m;
            m->DeepCopy(albaVMEGroup::SafeDownCast(vme)->GetMatrixVector()->GetMatrix(0));
            albaVMEGroup::SafeDownCast(vme)->SetMatrix(*m);
          }
          
        }
        
        // restore children VMEs
        else if (albaCString("VME") == children[i]->GetName())
        {
          albaVME *child_vme=RestoreVME(children[i],vme);
          if (child_vme==NULL)
          {
            albaErrorMacro("MSFImporter: error restoring child VME (parent=\""<<vme->GetName()<<"\")");
            continue;
          }

          // add the new VME as a child of the given parent node
          if (vme_type == "mflVMELandmarkCloud" || vme_type == "mflVMERigidLandmarkCloud" || vme_type == "mflVMEDynamicLandmarkCloud" && child_vme->IsALBAType(albaVMELandmark))
          {
              vme->AddChild(child_vme);
          }
          else if ((vme->IsALBAType(albaVMEMeter) || vme->IsALBAType(albaVMEProber)) && child_vme->GetTagArray()->IsTagPresent("mflVMELink"))
          {
            // this is a particular case for albaVMEMeter, in which the links are changed name
            if (albaCString(child_vme->GetName()) == "StartLink")
            {
              child_vme->SetName("StartVME");
              if (vme->GetTagArray()->IsTagPresent("MFL_METER_START_VME_ID"))
              {
                child_vme->GetTagArray()->SetTag("SUBLINK_ID",vme->GetTagArray()->GetTag("MFL_METER_START_VME_ID")->GetValue());
                vme->GetTagArray()->DeleteTag("MFL_METER_START_VME_ID");
              }
            }
            else if (albaCString(child_vme->GetName()) == "EndLink1")
            {
              child_vme->SetName("EndVME1");
              if (vme->GetTagArray()->IsTagPresent("MFL_METER_END_VME_1_ID"))
              {
                child_vme->GetTagArray()->SetTag("SUBLINK_ID",vme->GetTagArray()->GetTag("MFL_METER_END_VME_1_ID")->GetValue());
                vme->GetTagArray()->DeleteTag("MFL_METER_END_VME_1_ID");
              }
            }
            else if (albaCString(child_vme->GetName()) == "EndLink2")
            {
              child_vme->SetName("EndVME2");
              if (vme->GetTagArray()->IsTagPresent("MFL_METER_END_VME_2_ID"))
              {
                child_vme->GetTagArray()->SetTag("SUBLINK_ID",vme->GetTagArray()->GetTag("MFL_METER_END_VME_2_ID")->GetValue());
                vme->GetTagArray()->DeleteTag("MFL_METER_END_VME_2_ID");
              }
            }
            else if (albaCString(child_vme->GetName()) == "SurfaceLink")
            {
              child_vme->SetName("Surface");
            }
            else if (albaCString(child_vme->GetName()) == "VolumeLink")
            {
              child_vme->SetName("Volume");
            }
            vme->AddChild(child_vme);
          }
          else
          {
            vme->AddChild(child_vme);
          }
        } // VMR restore
      }// Children
    } // Name
  } // Type
  return vme;
}

//------------------------------------------------------------------------------
albaVME *mmuMSF1xDocument::CreateVMEInstance(albaString &name)
//------------------------------------------------------------------------------
{
  if (
    name == "albaVMEGeneric"         ||
    name == "mflVMEAlias"
    )
  {
    return albaVMEGeneric::New();
  }
  else if (name == "mflVMELink")
  {
    albaVME *link = albaVMEGeneric::New();
    link->GetTagArray()->SetTag("mflVMELink","1");
    return link;
  }
  else if (name == "mflVMEExternalData")
  {
    return albaVMEExternalData::New();
  }
  else if (name == "mflVMEGroup")
  {
    return albaVMEGroup::New();
  }
  else if (name == "mflVMESurface")
  {
    return albaVMESurface::New();
  }
  else if (name == "mflVMEGenericVolume")
  {
    return albaVMEVolumeRGB::New();
  }
  else if (name == "mflVMEGrayVolume")
  {
    return albaVMEVolumeGray::New();
  }
  else if (name == "mflVMELandmarkCloud" || name == "mflVMEDynamicLandmarkCloud"  || name == "mflVMERigidLandmarkCloud")
  {
    return albaVMELandmarkCloud::New();
  }
  else if (name == "mflVMELandmark")
  {
    return albaVMELandmark::New();
  }
  else if (name == "mflVMEImage")
  {
    return albaVMEImage::New();
  }
  else if (name == "mflVMEMeter")
  {
    return albaVMEMeter::New();
  }
  else if (name == "mflVMEPointSet")
  {
    return albaVMEPointSet::New();
  }
  else if (name == "mflVMERefSys")
  {
    return albaVMERefSys::New();
  }
  else if (name == "mflVMEMaps")
  {
    return albaVMEProber::New();
  }
  else if (name == "mflVMESlicer")
  {
    return albaVMESlicer::New();
  }
  else
  {
    albaErrorMacro("Unknown VME type: \""<<name.GetCStr()<<"\"");
    return NULL;
  }
}

//------------------------------------------------------------------------------
void mmuMSF1xDocument::RestoreMeterAttribute(albaVME *vme)
//------------------------------------------------------------------------------
{
  albaVMEMeter *meter = albaVMEMeter::SafeDownCast(vme);
  if (meter)
  {
    mmaMeter *meter_attrib = meter->GetMeterAttributes();
    albaTagArray *meter_ta  = meter->GetTagArray();
    int num_tags = meter_ta->GetNumberOfTags();
    std::vector<std::string> tag_list;
    meter_ta->GetTagList(tag_list);
    albaTagItem *ti = NULL;
    albaString tag_name;
    double component;
    for (int t=0; t<num_tags; t++)
    {
      tag_name = tag_list[t].c_str();
      if (tag_name.Equals("MFL_METER_END_VME_1_ID") || 
          tag_name.Equals("MFL_METER_START_VME_ID") ||
          tag_name.Equals("MFL_METER_END_VME_2_ID"))
      {
        continue;
      }
      ti = meter_ta->GetTag(tag_name.GetCStr());
      component = ti->GetComponentAsDouble(0);
      if (tag_name.Equals("MFL_METER_TYPE"))
      {
        meter_attrib->m_MeterMode = (int)component;
      }
      else if (tag_name.Equals("MFL_METER_COLOR_MODE"))
      {
        meter_attrib->m_ColorMode = (int)component;
      }
      else if (tag_name.Equals("MFL_METER_MEASURE_TYPE"))
      {
        meter_attrib->m_MeasureType = (int)component;
      }
      else if (tag_name.Equals("MFL_METER_REPRESENTATION"))
      {
        meter_attrib->m_Representation = (int)component;
      }
      else if (tag_name.Equals("MFL_METER_TUBE_CAPPING"))
      {
        meter_attrib->m_Capping = (int)component;
      }
      else if (tag_name.Equals("MFL_METER_EVENT_THRESHOLD"))
      {
        meter_attrib->m_GenerateEvent = (int)component;
      }
      else if (tag_name.Equals("MFL_METER_INIT_MEASURE"))
      {
        meter_attrib->m_InitMeasure = component;
      }
      else if (tag_name.Equals("MFL_METER_DELTA_PERCENT"))
      {
        meter_attrib->m_DeltaPercent = (int)component;
      }
      else if (tag_name.Equals("MFL_METER_LABEL_VISIBILITY"))
      {
        meter_attrib->m_LabelVisibility = (int)component;
      }
      else if (tag_name.Equals("MFL_METER_RADIUS"))
      {
        meter_attrib->m_TubeRadius = component;
      }
      else if (tag_name.Equals("MFL_METER_DISTANCE_RANGE"))
      {
        meter_attrib->m_DistanceRange[0] = component;
        meter_attrib->m_DistanceRange[1] = ti->GetComponentAsDouble(1);
      }
      else
        continue;
      meter_ta->DeleteTag(tag_name.GetCStr());
    }
  }
}

//------------------------------------------------------------------------------
void mmuMSF1xDocument::RestoreMaterial(albaVME *vme)
//------------------------------------------------------------------------------
{
  mmaMaterial *material = (mmaMaterial *)vme->GetAttribute("MaterialAttributes");
  if (material == NULL)
  {
    material = mmaMaterial::New();
    vme->SetAttribute("MaterialAttributes", material);
  }

  albaTagItem *mat_item = vme->GetTagArray()->GetTag("material");
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
int mmuMSF1xDocument::RestoreVItem(albaStorageElement *node, albaVME *vme)
//------------------------------------------------------------------------------
{
  albaTimeStamp item_time;
  if (node->GetAttributeAsDouble("TimeStamp",item_time))
  {
    albaString data_type;
    if (node->GetAttribute("DataType",data_type))
    {
      albaID item_id;
      if (node->GetAttributeAsInteger("Id",item_id))
      {
        albaString data_file;
        if (node->GetAttribute("DataFile",data_file))
        {
          albaSmartPointer<albaVMEItemVTK> vitem;
          albaStorageElement *tarray=node->FindNestedElement("TArray");
          albaVMEGeneric *vme_generic=albaVMEGeneric::SafeDownCast(vme);
          assert(vme_generic);
          if (tarray)
          {
            if (RestoreTagArray(tarray,vitem->GetTagArray())==ALBA_OK)
            {
              /////////////////////////////////////////////// 
              // here should process VMEItem-specific tags //
              ///////////////////////////////////////////////
              albaTagArray *ta = vitem->GetTagArray();
              if (ta->IsTagPresent("MFL_CRYPTING"))
              {
                albaTagItem *ti = ta->GetTag("MFL_CRYPTING");
                vme->SetCrypting((int)ti->GetValueAsDouble());
                vitem->SetCrypting((int)ti->GetValueAsDouble() != 0);
                ta->DeleteTag("MFL_CRYPTING");
              }
              if (ta->IsTagPresent("VTK_DATASET_BOUNDS"))
              {
                double *b = vitem->GetBounds();
                albaTagItem *ti = ta->GetTag("VTK_DATASET_BOUNDS");
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
          vitem->SetDataType(data_type);
          vme_generic->GetDataVector()->AppendItem(vitem);
          vme_generic->GetDataVector()->SetCrypting(vitem->GetCrypting());
          return ALBA_OK;
        } // DataFile
      } // Id
    } // DataType
  } // TimeStamp
  return ALBA_ERROR;
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::RestoreVMatrix(albaStorageElement *node, albaMatrixVector *vmatrix)
//------------------------------------------------------------------------------
{
  // restore single matrices
  albaStorageElement::ChildrenVector children;
  children = node->GetChildren();

  vmatrix->RemoveAllItems();

  for (int i = 0;i<children.size();i++)
  {
    assert(albaCString("Matrix") == children[i]->GetName());

    albaSmartPointer<albaMatrix> matrix;
    int restored_matrix = children[i]->RestoreMatrix(matrix);
    if (restored_matrix != ALBA_ERROR)
    {
      vmatrix->AppendKeyMatrix(matrix);
    }
    else
    {
      return ALBA_ERROR;
    }
  }
  return ALBA_OK;
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::RestoreTagArray(albaStorageElement *node, albaTagArray *tarray)
//------------------------------------------------------------------------------
{
  albaStorageElement::ChildrenVector children;
  children = node->GetChildren();

  for (int i = 0;i<children.size();i++)
  {
    if (albaCString("TItem") == children[i]->GetName())
    {
      albaID num_of_comps;
      if (children[i]->GetAttributeAsInteger("Mult",num_of_comps))
      {
        albaString tag_name;
        if (children[i]->GetAttribute("Tag",tag_name))
        {
          albaString tag_type;
          if (children[i]->GetAttribute("Type",tag_type))
          {
            albaTagItem titem;
            titem.SetNumberOfComponents(num_of_comps);
            titem.SetName(tag_name);
    
            if (tag_type=="NUM")
            {
              titem.SetType(ALBA_NUMERIC_TAG);
            }
            else if (tag_type=="STR")
            {
              titem.SetType(ALBA_STRING_TAG);
            }
            else if (tag_type=="MIS")
            {
              titem.SetType(ALBA_MISSING_TAG);
            }
            else
            {
              titem.SetType(atof(tag_type));
            }

            albaStorageElement::ChildrenVector tag_comps;
            tag_comps = children[i]->GetChildren();
            int idx=0;
            for (int n = 0;n<tag_comps.size();n++)
            {
              if (albaCString("TC")==tag_comps[n]->GetName())
              {
                albaString tc;
                tag_comps[n]->RestoreText(tc);
                titem.SetComponent(tc,idx);
                idx++;
              }
              else
              {
                albaErrorMacro("Error parning a TItem element inside a TagArray: expected <TC> sub element, found <"<<tag_comps[n]->GetName()<<">");
              } 
            } 
            tarray->SetTag(titem);
          } // Type
        } // Tag
      } // Mult
      //albaErrorMacro("Error parning a TItem element inside a TagArray: missing required Attribute");
    } // TItem
  }
  return ALBA_OK;
}

//------------------------------------------------------------------------------
// albaMSFImporter
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaMSFImporter)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaMSFImporter::albaMSFImporter()
//------------------------------------------------------------------------------
{
  SetVersion("1.0");
  SetFileType("MSF");
  SetDocument(new mmuMSF1xDocument); // create a MSF doc
}

//------------------------------------------------------------------------------
albaMSFImporter::~albaMSFImporter()
//------------------------------------------------------------------------------
{
  cppDEL(m_Document); // delete the document object
}

//------------------------------------------------------------------------------
void albaMSFImporter::SetRoot(albaVMERoot *root)
//------------------------------------------------------------------------------
{
  ((mmuMSF1xDocument *)m_Document)->SetRoot(root);
}

//------------------------------------------------------------------------------
albaVMERoot *albaMSFImporter::GetRoot()
//------------------------------------------------------------------------------
{
  return ((mmuMSF1xDocument *)m_Document)->GetRoot();
}
