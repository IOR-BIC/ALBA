/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaDataVector
 Authors: Marco Petrone - Paolo Quadrani
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaDataVector.h"
#include <wx/zipstrm.h>
#include <wx/zstream.h>
#include <wx/sstream.h>
#include <wx/wfstream.h>
#include <wx/fs_zip.h>

#include "albaZipUtility.h"
#include "mmuTimeSet.h"
#include "albaStorageElement.h"
#include "albaVMEStorage.h"
#include "albaEventIO.h"
#include "albaIndent.h"
#include "albaVMERoot.h"
#include "albaAttribute.h"

#include <fstream>
#include "albaProgressBarHelper.h"

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
/** Event ID used to know if the VME should serialize itself as a single or multiple binary files.*/
// static const albaID m_SingleFileDataId = mmuIdFactory::GetNextId("SINGLE_FILE_DATA");

//-----------------------------------------------------------------------
albaCxxTypeMacro(albaDataVector)
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------
albaDataVector::albaDataVector()
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
albaDataVector::~albaDataVector()
//-----------------------------------------------------------------------
{
  Iterator it;
  for (it=Begin();it!=End();it++)
  {
    it->second->SetListener(NULL); // detach items before destroying
  }
}
//-------------------------------------------------------------------------
albaID albaDataVector::GetSingleFileDataId()
//-------------------------------------------------------------------------
{
  static const albaID singleFileDataId = mmuIdFactory::GetNextId("SINGLE_FILE_DATA");
  return singleFileDataId;
}
//-------------------------------------------------------------------------
bool albaDataVector::GetCrypting()
//-------------------------------------------------------------------------
{
  return m_Crypting;
}

//-------------------------------------------------------------------------
void albaDataVector::SetCrypting(bool flag)
//-------------------------------------------------------------------------
{
  if (m_Crypting == flag)
  {
    return;
  }
  m_Crypting = flag;
  Modified();
}

//-------------------------------------------------------------------------
albaID albaDataVector::GetVectorID()
//-------------------------------------------------------------------------
{
  if (m_VectorID < 0)
  {
    // retrieve the tree root
    albaEventIO e(this,NODE_GET_ROOT);
    InvokeEvent(e);

    albaVMERoot *root = albaVMERoot::SafeDownCast(e.GetRoot());
    m_VectorID = root ? root->GetNextItemId():-1;
  }
  return m_VectorID;
}

//-------------------------------------------------------------------------
void albaDataVector::UpdateVectorId()
//-------------------------------------------------------------------------
{
  // retrieve the tree root
  albaEventIO e(this,NODE_GET_ROOT);
  InvokeEvent(e);

  albaVMERoot *root = albaVMERoot::SafeDownCast(e.GetRoot());
  m_VectorID = root ? root->GetNextItemId():-1;
}

//-----------------------------------------------------------------------
void albaDataVector::ShallowCopy(albaDataVector *array)
//-----------------------------------------------------------------------
{
  RemoveAllItems();
  for (Iterator it=array->Begin();it!=array->End();it++)
  {
    albaVMEItem *item=it->second;
	  albaVMEItem *copy = item->NewInstance();
    assert(copy);
	  copy->ShallowCopy(item);
    AppendItem(copy); //  Changed by Losi 09.24.2009:
                      //  Before was AppendItem(item) this generated leaks because copy was unreferenced
  }

  Modified();
}

//-------------------------------------------------------------------------
void albaDataVector::DeepCopyVmeLarge(albaDataVector *o)
//-------------------------------------------------------------------------
{
  RemoveAllItems();
  Iterator it;
  for (it=o->Begin();it!=o->End();it++)
  {
    albaVMEItem *m=it->second;
    albaVMEItem *new_item=m->NewInstance();
    new_item->DeepCopyVmeLarge(m);
    AppendItem(new_item);
  }
  Modified();
}

//-----------------------------------------------------------------------
void albaDataVector::SetSingleFileMode(bool mode)
//-----------------------------------------------------------------------
{
  m_SingleFileMode = mode;
  Modified();
}

//-----------------------------------------------------------------------
void albaDataVector::AppendItem(albaVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::AppendItem(m);
}
//-----------------------------------------------------------------------
void albaDataVector::PrependItem(albaVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::PrependItem(m);
}
//-----------------------------------------------------------------------
void albaDataVector::InsertItem(albaVMEItem *m)
//-----------------------------------------------------------------------
{
  m->SetListener(this);
  Superclass::InsertItem(m);
}
//-----------------------------------------------------------------------
int albaDataVector::InternalStore(albaStorageElement *parent)
//-----------------------------------------------------------------------
{
  parent->SetAttribute("NumberOfItems",albaString(GetNumberOfItems()));
  parent->SetAttribute("ItemTypeName",GetItemTypeName());

  // retrieve the tree root
  albaEventIO e(this,NODE_GET_ROOT);
  InvokeEvent(e);

  albaVMERoot *root = albaVMERoot::SafeDownCast(e.GetRoot());
  assert(root);

  m_VectorID = GetVectorID();

  // the DataVector ID
  parent->SetAttribute("VectorID",albaString(m_VectorID));

  albaVMEStorage *storage = root->GetStorage();
  assert(storage);

  // define base file name for data files
  albaString base_url = storage->GetURL();
  int last_dot = base_url.FindLastChr('.');

  base_url.Erase(last_dot);
  albaString base_name = base_url;

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
  // Notice that in albaVMEAbstractGaneric, when the storage URL is changed the 
  // filename change event it caught and a Modified() is forced to the m_DataVector, 
  // thus in case the MSF has just been loaded (m_JustReastored is true) but the
  // storage object's URL has been changed, the DataVector appears as being Modified
  // and data save to the new place is forced!
  if (base_name_changed || IsDataModified()) // if some data added or removed...
  {
    if (m_LastBaseURL != base_url)
    {
      /*// ask how to save the binary file (as setting of the application):
      // single file or multi file.
      albaEvent single_file_event(this, SINGLE_FILE_DATA);
      InvokeEvent(&single_file_event);
      SetSingleFileMode(single_file_event.GetBool());*/
      bool single_file_mode;
      single_file_mode = GetNumberOfItems() > 1;
      SetSingleFileMode(single_file_mode);
    }

    // store data
    bool new_data = false;
    
    std::vector<albaString> data_files;
    data_files.resize(GetNumberOfItems());
    
    int i, ret;
    albaVMEItem *item = NULL;

    // store single data elements into its own files or into a single file if m_SingleFileMode is true
    if (m_SingleFileMode)
    {
      it = Begin();
      // check if there is at least one item
      if (it != End())
      {
        //////////////////////////////////////////////////////////////////////////
        int resolvedURL = ALBA_OK;
        albaString filename;
        resolvedURL = storage->ResolveInputURL(m_ArchiveName, filename);

        if (resolvedURL == ALBA_OK && wxFileExists(filename.GetCStr()))//Only if exist an archive where it is possible read the vtk data
        {
          albaProgressBarHelper progressHelper(m_Listener);
					progressHelper.InitProgressBar("",false);


          std::vector<albaString> filesExtracted = ZIPOpen(filename);

          DataMap::iterator itTmp;
          albaVMEItem *itemTmp = NULL;
          itTmp = Begin();
          int dataIndex = 0;
          int step = round(this->GetNumberOfItems() / 100) + 1;
          for (itTmp = Begin(); itTmp != End(); itTmp++)
          {
						progressHelper.UpdateProgressBar(dataIndex*100/this->GetNumberOfItems());
            itemTmp = itTmp->second;
            int IOmode = itemTmp->GetIOMode();
            itemTmp->SetIOModeToDefault();
            itemTmp->UpdateData();
            itemTmp->SetIOMode(IOmode);

            dataIndex++;
          }
        
					progressHelper.CloseProgressBar();

          for (int i=0;i<filesExtracted.size();i++)
          {
            remove(filesExtracted[i]);
          }
        }

        //////////////////////////////////////////////////////////////////////////

        m_ArchiveName = base_name;
        m_ArchiveName << ".";
        m_ArchiveName << m_VectorID;
        m_ArchiveName << ".z";

        it = Begin();
        item = it->second;
        item->UpdateData();
        m_ArchiveName << item->GetDataFileExtension();
        albaString tmp_archive;
        storage->GetTmpFile(tmp_archive);
        wxFileOutputStream out(tmp_archive.GetCStr());
        wxZipOutputStream zip(out);
        if (!out || !zip)
          return ALBA_ERROR;

        for (it = Begin(), i = 0; it != End(); it++, i++)
        {
          item = it->second;

          // set item ID if not yet set
          if (item->GetId() < 0)
            item->SetId(root->GetNextItemId());

          // data file URL is specified as a local filename
          albaString data_file_url;
          data_file_url << base_name << "." << albaString(item->GetId()) << "." << item->GetDataFileExtension(); // extension is defined by the kind of item itself

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
          if (ret == ALBA_ERROR)
          {
            return ret;
          }
          item->SetURL(data_file_url);
          item->ReleaseOldFileOn(); // restore to default

          item->SetArchiveFileName(m_ArchiveName);
          if (!item->StoreToArchive(zip))
          {
            albaMessage(_("Unable to write %s into archive file %s"), data_file_url, m_ArchiveName.GetCStr());
            ret = ALBA_ERROR;
          }

          // Free the memory allocated from the string saved into the archive.
          item->ReleaseOutputMemory();

          switch (ret)
          {
            case ALBA_OK: 
              new_data = true;
            break;  // new data written on disk
            case ALBA_ERROR:
              return ALBA_ERROR;   // I/O error while writing
          }
        }

        if (!zip.Close() || !out.Close())
        {
          return ALBA_ERROR;
        }

        storage->StoreToURL(tmp_archive, m_ArchiveName);
      }
    }
    else
    {
      for (it = Begin(), i = 0; it != End(); it++, i++)
      {
        item = it->second;
        item->UpdateData();

        // set item ID if not yet set
        if (item->GetId() < 0)
          item->SetId(root->GetNextItemId());

        // data file URL is specified as a local filename
        albaString data_file_url;
        data_file_url << base_name << "." << albaString(item->GetId()) << "." << item->GetDataFileExtension(); // extension is defined by the kind of item itself

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
          case ALBA_OK: 
            new_data = true;
          break;  // new data written on disk
          case ALBA_ERROR:
            return ALBA_ERROR;   // I/O error while writing
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
  return ALBA_OK;
}
//-----------------------------------------------------------------------
int albaDataVector::InternalRestore(albaStorageElement *node)
//-----------------------------------------------------------------------
{
  albaString item_type,single_file;
  albaID num_items;

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
    return ALBA_ERROR;
  }

  // restore items meta-data
  albaStorageElement::ChildrenVector elements;
  node->GetNestedElementsByName("VItem",elements);

  assert(num_items == elements.size()); // check the number of elements

  for (int i = 0; i < num_items; i++)
  {
    albaVMEItem *item = albaVMEItem::SafeDownCast(elements[i]->RestoreObject());
    assert(item);
    if (!item)
    {
      albaErrorMacro("Cannot restore VME-Item: Wrong object type or I/O problems");
      return ALBA_ERROR;
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

  return ALBA_OK;
}

//-----------------------------------------------------------------------
void albaDataVector::OnEvent(albaEventBase *alba_event)
//-----------------------------------------------------------------------
{
  if (alba_event->GetId() == albaVMEItem::VME_ITEM_DATA_MODIFIED)
  {
    m_DataModified=true;
    return;
  }
  albaEventMacro(*alba_event);
}
