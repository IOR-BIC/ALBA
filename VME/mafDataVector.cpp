/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataVector.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-11 11:21:57 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "mafDataVector.h"
#include "mmuTimeSet.h"
#include "mafStorageElement.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafIndent.h"
#include "mafVMERoot.h"
#include "mafAttribute.h"

#include <fstream>

//-----------------------------------------------------------------------
mafCxxTypeMacro(mafDataVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
mafDataVector::mafDataVector()
//-----------------------------------------------------------------------
{
  m_Crypting        = false;
  m_DataModified    = false;
  m_SingleFileMode  = false;
  m_VectorID        = -1;
}

//-----------------------------------------------------------------------
mafDataVector::~mafDataVector()
//-----------------------------------------------------------------------
{
}

//-------------------------------------------------------------------------
bool mafDataVector::GetCrypting()
//-------------------------------------------------------------------------
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
void mafDataVector::SetCrypting(bool flag)
//-------------------------------------------------------------------------
{
  m_Crypting = flag;
  Modified();
}


//-----------------------------------------------------------------------
void mafDataVector::ShallowCopy(mafDataVector *array)
//-----------------------------------------------------------------------
{
  RemoveAllItems();
  for (Iterator it=array->Begin();it!=array->End();it++)
  {
    mafVMEItem *item=it->second;
	  mafVMEItem *copy = item->NewInstance();
    assert(copy);
	  copy->ShallowCopy(item);
    AppendItem(item);
  }

  Modified();
}

//-----------------------------------------------------------------------
void mafDataVector::SetSingleFileMode(bool mode)
//-----------------------------------------------------------------------
{
  m_SingleFileMode = mode;
  Modified();
}

//-----------------------------------------------------------------------
void mafDataVector::AppendItem(mafVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::AppendItem(m);
}
//-----------------------------------------------------------------------
void mafDataVector::PrependItem(mafVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::PrependItem(m);
}
//-----------------------------------------------------------------------
void mafDataVector::InsertItem(mafVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::InsertItem(m);
}
//-----------------------------------------------------------------------
int mafDataVector::InternalStore(mafStorageElement *parent)
//-----------------------------------------------------------------------
{
  parent->SetAttribute("NumberOfItems",mafString(GetNumberOfItems()));
  parent->SetAttribute("ItemTypeName",GetItemTypeName());

  // retrieve the tree root
  mafEventIO e(this,NODE_GET_ROOT);
  InvokeEvent(e);

  mafVMERoot *root=(mafVMERoot *)e.GetData();
  assert(root);

  if (GetVectorID()<0)
    m_VectorID = root->GetNextItemId();

  // the DataVector ID
  parent->SetAttribute("VectorID",mafString(m_VectorID));

  mafStorage *storage=root->GetStorage();
  assert(storage);

  // define base file name for data files
  mafString base_url = storage->GetURL();
  int last_slash = base_url.FindLastChr('.');

  base_url.Erase(last_slash);

  DataMap::iterator it;
  
  if (IsDataModified()) // if some data added or removed...
  {
    // store data
    bool new_data=false;
    
    std::vector<mafString> data_files;
    data_files.resize(GetNumberOfItems());
    
    // store single data elements into its own files
    int i;
    for (it=Begin(),i=0;it!=End();it++,i++)
    {
      mafVMEItem *item=it->second;
      
      // set item ID if not yet set
      if (item->GetId()<0)
      {
        item->SetId(root->GetNextItemId());
      
        mafString data_file_url;
      
        data_file_url<<base_url<<"."<<mafString(item->GetId())<<"."<<item->GetDataFileExtension(); // extension is defined by the kind of item itself
        item->SetURL(data_file_url); 
      }
      
      int ret;
      
      // in single file mode force writing into a local file to merge all files together
      if (m_SingleFileMode)
      {      
        // find a tmp file name to store the data inside...
        mafString tmp_filename;
        storage->GetTmpFile(tmp_filename);
        data_files[i]=tmp_filename;
       
        item->SetIOModeToTmpFile();
        item->SetTmpFileName(tmp_filename);
        ret=item->StoreData();
      }
      else
      {
        item->SetIOModeToDefault();
        ret=item->StoreData();
      }
      
      switch (ret)
      {
      case MAF_OK: new_data=true; break;  // new data written on disk
      case MAF_ERROR: return MAF_ERROR;   // I/O error while writing
      }
      
      // ...
    }
    
    // if single data file...
    // To be completed!!!
    if (m_SingleFileMode)
    {
      
      // the URL to which the single file will be stored
      mafString file_url;
      file_url<<base_url<<"."<<mafString(m_VectorID)<<".mdf"; // extension for single file format

      // in single file mode merge single files into a single file
      if (new_data)
      {
        mafString tmp_filename;
        storage->GetTmpFile(tmp_filename);
        
        // open local output file
        std::ofstream outfile;
        outfile.open(tmp_filename, std::ios_base::out | std::ios_base::binary );
        if (outfile.fail())
        {
          mafErrorMacro("Cannot open output file for merged VME Item data: " << tmp_filename);
          return -1;
        }

        // merge files
        for (it=Begin(),i=0;it!=End();it++,i++)
        {
          std::ifstream infile;
          infile.open(data_files[i], std::ios_base::in | std::ios_base::binary );

          if (infile.fail())
          {
            mafErrorMacro("Cannot open input file for external data: " << data_files[i]);   
            return MAF_ERROR;
          }

          outfile << infile.rdbuf();

          // extract info about the position in the single file
          // to be implemented

        }

        // delete single files...
        for (i=0;i<GetNumberOfItems();i++)
        {
          storage->ReleaseTmpFile(data_files[i]);          
        }
      }
    }
  }

  parent->SetAttribute("SingleFileMode",m_SingleFileMode?"true":"false");

  // Store meta-data (meta-data is stored later to be able set some info about stored data files)
  for (it=Begin();it!=End();it++)
  {
    parent->StoreObject("VItem",it->second.GetPointer());
  }

  m_DataModified = false;

  return MAF_OK;
}
//-----------------------------------------------------------------------
int mafDataVector::InternalRestore(mafStorageElement *node)
//-----------------------------------------------------------------------
{
  mafString item_type,single_file;

  mafID num_items;

  if (node->GetAttributeAsInteger("NumberOfItems", num_items) && \
      node->GetAttribute("ItemTypeName", item_type) && \
      node->GetAttribute("SingleFileMode", single_file) \
    )
  {
    SetItemTypeName(item_type);
    SetSingleFileMode(single_file=="true"||single_file=="True"||single_file=="TRUE");
  }
  else
  {
    return MAF_ERROR;
  }

  // restore items meta-data
  mafStorageElement::ChildrenVector elements;
  node->GetNestedElementsByName("VItem",elements);

  assert(num_items==elements.size()); // check the number of elements

  for (int i=0;i<num_items;i++)
  {
    mafVMEItem *item=mafVMEItem::SafeDownCast(elements[i]->RestoreObject());
    assert(item);
    if (!item)
    {
      mafErrorMacro("Cannot restore VME-Item: Wrong object type or I/O problems");
      return MAF_ERROR;
    }
    AppendItem(item);
  }

  // data is restored on demand by single items!

  return MAF_OK;
}

//-----------------------------------------------------------------------
void mafDataVector::OnEvent(mafEventBase *event)
//-----------------------------------------------------------------------
{
  mafEventMacro(*event);
}


