/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafDataVector.cpp,v $
  Language:  C++
  Date:      $Date: 2007-08-22 16:57:52 $
  Version:   $Revision: 1.16 $
  Authors:   Marco Petrone - Paolo Quadrani
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
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "mmuTimeSet.h"
#include "mafStorageElement.h"
#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafIndent.h"
#include "mafVMERoot.h"
#include "mafAttribute.h"

#include <fstream>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafDataVector::SINGLE_FILE_DATA);

//-----------------------------------------------------------------------
mafCxxTypeMacro(mafDataVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
mafDataVector::mafDataVector()
//-----------------------------------------------------------------------
{
  m_Crypting        = false;
  m_DataModified    = false;
  m_SingleFileMode  = true;
  m_JustRestored    = false;
  m_VectorID        = -1;
  m_ArchiveName     = "";
}

//-----------------------------------------------------------------------
mafDataVector::~mafDataVector()
//-----------------------------------------------------------------------
{
  Iterator it;
  for (it=Begin();it!=End();it++)
  {
    it->second->SetListener(NULL); // detach items before destroying
  }
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
  if (m_Crypting == flag)
  {
    return;
  }
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

  mafVMERoot *root = mafVMERoot::SafeDownCast(e.GetRoot());
  assert(root);

  if (GetVectorID()<0)
    m_VectorID = root->GetNextItemId();

  // the DataVector ID
  parent->SetAttribute("VectorID",mafString(m_VectorID));

  mafStorage *storage = root->GetStorage();
  assert(storage);

  // define base file name for data files
  mafString base_url = storage->GetURL();
  int last_dot = base_url.FindLastChr('.');

  base_url.Erase(last_dot);
  mafString base_name = base_url;

  int last_slash = base_name.FindLastChr('/');
  if (last_slash >= 0)
  {
    base_name.Erase(0,last_slash);
  }
  
  // this test force data to be written when the MSF filename has changed.
  // The case when data has just been loaded is avoided, since in that case
  // there is not yet an old filename to which the file was saved.
  DataMap::iterator it;
  bool base_name_changed = m_LastBaseURL != base_url && !m_JustRestored;
  bool release_old_files = !base_name_changed && !m_LastBaseURL.IsEmpty();
  
  // now check if the filename is (really) changed or if something has changed.
  // Notice that in mafVMEAbstractGaneric, when the storage URL is changed the 
  // filename change event it caught and a Modified() is forced to the m_DataVector, 
  // thus in case the MSF has just been loaded (m_JustReastored is true) but the
  // storage object's URL has been changed, the DataVector appears as being Modified
  // and data save to the new place is forced!
  if (base_name_changed || IsDataModified()) // if some data added or removed...
  {
    if (m_LastBaseURL != base_url)
    {
      // ask how to save the binary file (as setting of the application):
      // single file or multi file.
      mafEvent single_file_event(this, SINGLE_FILE_DATA);
      InvokeEvent(&single_file_event);
      SetSingleFileMode(single_file_event.GetBool());
    }

    // store data
    bool new_data = false;
    
    std::vector<mafString> data_files;
    data_files.resize(GetNumberOfItems());
    
    int i, ret;
    mafVMEItem *item = NULL;

    // store single data elements into its own files or into a single file if m_SingleFileMode is true
    if (m_SingleFileMode)
    {
      it = Begin();
      // check if there is at least one item
      if (it != End())
      {
        m_ArchiveName = base_name;
        m_ArchiveName << ".";
        m_ArchiveName << m_VectorID;
        m_ArchiveName << ".z";
        item = it->second;
        m_ArchiveName << item->GetDataFileExtension();
        mafString tmp_archive;
        storage->GetTmpFile(tmp_archive);
        wxFileOutputStream out(tmp_archive.GetCStr());
        wxZipOutputStream zip(out);
        if (!out || !zip)
          return MAF_ERROR;

        for (it = Begin(), i = 0; it != End(); it++, i++)
        {
          item = it->second;

          // set item ID if not yet set
          if (item->GetId() < 0)
            item->SetId(root->GetNextItemId());

          // data file URL is specified as a local filename
          mafString data_file_url;
          data_file_url << base_name << "." << mafString(item->GetId()) << "." << item->GetDataFileExtension(); // extension is defined by the kind of item itself

          item->SetCrypting(m_Crypting);
          item->SetIOModeToMemory();
          // if in SaveAs do not remove old filename...
          //if (!base_name_changed && !m_LastBaseURL.IsEmpty())
          if(release_old_files)
          {
            item->ReleaseOldFileOn();
          }
          else
          {
            item->ReleaseOldFileOff();
          }

          ret = item->StoreData(NULL); //Storing in memory not require to create a file.
          if (ret == MAF_ERROR)
          {
            return ret;
          }
          item->SetURL(data_file_url);
          item->ReleaseOldFileOn(); // restore to default

          item->SetArchiveFileName(m_ArchiveName);
          if (!item->StoreToArchive(zip))
          {
            ret = MAF_ERROR;
          }

          // Free the memory allocated from the string saved into the archive.
          item->ReleaseOutputMemory();

          switch (ret)
          {
            case MAF_OK: 
              new_data = true;
            break;  // new data written on disk
            case MAF_ERROR:
              return MAF_ERROR;   // I/O error while writing
          }
        }

        if (!zip.Close() || !out.Close())
        {
          return MAF_ERROR;
        }

        storage->StoreToURL(tmp_archive, m_ArchiveName);
      }
    }
    else
    {
      for (it = Begin(), i = 0; it != End(); it++, i++)
      {
        item = it->second;

        // set item ID if not yet set
        if (item->GetId() < 0)
          item->SetId(root->GetNextItemId());

        // data file URL is specified as a local filename
        mafString data_file_url;
        data_file_url << base_name << "." << mafString(item->GetId()) << "." << item->GetDataFileExtension(); // extension is defined by the kind of item itself

        item->SetCrypting(m_Crypting);

        // if in SaveAs do not remove old filename...
        if(release_old_files)
        {
          item->ReleaseOldFileOn();
        }
        else
        {
          item->ReleaseOldFileOff();
        }

        item->SetIOModeToDefault();
        ret = item->StoreData(data_file_url);

        item->ReleaseOldFileOn(); // restore to default
        switch (ret)
        {
          case MAF_OK: 
            new_data = true;
          break;  // new data written on disk
          case MAF_ERROR:
            return MAF_ERROR;   // I/O error while writing
        }
      }
    }
    // update the last base name for next time writing
    m_LastBaseURL = base_url;
    m_JustRestored = false;
  }

  parent->SetAttribute("SingleFileMode",m_SingleFileMode ? "true" : "false");
  if (m_SingleFileMode)
  {
    parent->SetAttribute("ArchiveFileName", m_ArchiveName);
  }

  // Store meta-data (meta-data is stored later to be able set some info about stored data files)
  for (it = Begin(); it != End(); it++)
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

  m_JustRestored = true;

  if (node->GetAttributeAsInteger("NumberOfItems", num_items) && \
      node->GetAttribute("ItemTypeName", item_type) && \
      node->GetAttribute("SingleFileMode", single_file) && \
      node->GetAttributeAsInteger("VectorID", m_VectorID) \
    )
  {
    SetItemTypeName(item_type);
    SetSingleFileMode(single_file == "true" || single_file == "True" || single_file == "TRUE");
    if (m_SingleFileMode)
    {
      node->GetAttribute("ArchiveFileName", m_ArchiveName);
    }
  }
  else
  {
    return MAF_ERROR;
  }

  // restore items meta-data
  mafStorageElement::ChildrenVector elements;
  node->GetNestedElementsByName("VItem",elements);

  assert(num_items == elements.size()); // check the number of elements

  for (int i = 0; i < num_items; i++)
  {
    mafVMEItem *item = mafVMEItem::SafeDownCast(elements[i]->RestoreObject());
    assert(item);
    if (!item)
    {
      mafErrorMacro("Cannot restore VME-Item: Wrong object type or I/O problems");
      return MAF_ERROR;
    }
    if (m_SingleFileMode)
    {
      item->SetIOModeToMemory();
      item->SetArchiveFileName(m_ArchiveName);
    }
    AppendItem(item);
  }
  // data is restored on demand by single items!

  // force the flag to false to avoid data rewriting while storing
  m_DataModified = false;

  return MAF_OK;
}

//-----------------------------------------------------------------------
void mafDataVector::OnEvent(mafEventBase *maf_event)
//-----------------------------------------------------------------------
{
  if (maf_event->GetId() == mafVMEItem::VME_ITEM_DATA_MODIFIED)
  {
    m_DataModified=true;
    return;
  }
  mafEventMacro(*maf_event);
}
