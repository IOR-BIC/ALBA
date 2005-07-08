/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafMSFImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-08 16:32:29 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone m.petrone@cineca.it
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "mafIncludeWX.h" // to be removed

#include "mafMSFImporter.h"
#include "mafVMERoot.h"
#include "mafVMEGeneric.h"
#include "mafTagArray.h"
#include "mafVMEItemVTK.h"
#include "mafDataVectorVTK.h"
#include "mmuUtility.h"
#include "mafStorable.h"
#include "mafStorageElement.h"
#include "mafMatrixVector.h"
#include "mafVMEGroup.h"

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
//-------------------------------------------------------
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
    if (mafCString("VME") == children[i]->GetName())
    {
      mafVME *child_vme=RestoreVME(children[i],m_Root);
      if (child_vme==NULL)
      {
        mafErrorMacro("Error while restoring a VME (parent is the root)");
      }
      m_Root->AddChild(child_vme);
    }    
  }
  
  return m_Root->Initialize();
}

//------------------------------------------------------------------------------
mafVME *mmuMSF1xDocument::RestoreVME(mafStorageElement *node, mafVME *parent)
//------------------------------------------------------------------------------
{
  // restore due attributes
  mafString vme_name;
  if (node->GetAttribute("Name",vme_name))
  {
    m_Root->SetName(vme_name);

    mafString vme_type;
    if (node->GetAttribute("Type",vme_type))
    {
      mafVME *vme = CreateVMEInstance(vme_type);
      assert(vme);
  
      // traverse children and restore TagArray, MatrixVector and VMEItems 
      mafStorageElement::ChildrenVector children;

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
          mafVMEGeneric *vme_generic= mafVMEGeneric::SafeDownCast(vme);
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
            return NULL;
          }
        } 

        if (vme_type!="mafVMELink" && vme_type!="mafVMEAlias")
        {
          // add the new VME as a child of the given parent node
          parent->AddChild(vme);
        }
        else
        {
          // for VME-link and VME-alias we simply need to create links in current VME

          // ... to be implemented
        }
      }
      
    } // Type
  } // Name

  
  return NULL;
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
          vitem->SetTimeStamp(item_time);
          vitem->SetId(item_id);
          vitem->SetURL(data_file);
          mafStorageElement *tarray=node->FindNestedElement("TArray");
          if (tarray)
          {
            if (RestoreTagArray(tarray,vitem->GetTagArray())==MAF_OK)
            {
              mafVMEGeneric *vme_generic=mafVMEGeneric::SafeDownCast(vme);
              assert(vme_generic);
              vme_generic->GetDataVector()->AppendItem(vitem);
              return MAF_OK;
            }
          } // tarray
        } // DataFile
      } // Id
    } // DataType
  } // TimeStamp
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::RestoreVMatrix(mafStorageElement *node, mafMatrixVector *vmatrix)
//------------------------------------------------------------------------------
{

  // restore single matrices
  mafStorageElement::ChildrenVector children;
  for (int i = 0;i<children.size();i++)
  {
    assert(mafCString("Matrix") == children[i]->GetName());

    mafSmartPointer<mafMatrix> matrix;
    children[i]->RestoreMatrix(matrix);
    vmatrix->AppendKeyMatrix(matrix);
  }
}

//------------------------------------------------------------------------------
int mmuMSF1xDocument::RestoreTagArray(mafStorageElement *node, mafTagArray *tarray)
//------------------------------------------------------------------------------
{
  mafStorageElement::ChildrenVector children;
  for (int i = 0;i<children.size();i++)
  {
    if (mafCString("TItem") == children[i])
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
            titem->SetNumberOfComponents(num_of_comps);
            titem->SetName(tag_name);
    
            if (tag_type=="NUM")
            {
              titem->SetType(MAF_NUMERIC_TAG);
            }
            else if (tag_type=="STR")
            {
              titem->SetType(MAF_STRING_TAG);
            }
            else if (tag_type=="MIS")
            {
              titem->SetType(MAF_MISSING_TAG);
            }
            else
            {
              titem->SetType(atof(tag_type));
            }


            mafStorageElement::ChildrenVector tag_comps;
            int idx=0;
            for (int n = 0;n<tag_comps.size();n++,idx++)
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
                mafErrorMacro("Error parning a TItem element inside a TagArray: expected <TC> sub element, found <"<<tag_comps[n].GetName()<<">");
              } 
            } 
          } // Type
        } // Tag
      } // Mult
      mafErrorMacro("Error parning a TItem element inside a TagArray: missing required Attribute");
    } // TItem
  }
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

