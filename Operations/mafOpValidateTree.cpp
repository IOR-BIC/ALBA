/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafOpValidateTree.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-28 12:04:32 $
  Version:   $Revision: 1.4 $
  Authors:   Paolo Quadrani
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


#include "mafOpValidateTree.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmgGui.h"

#include "mafStorage.h"
#include "mafEventIO.h"
#include "mafDataVector.h"
#include "mafVMEItem.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMERoot.h"
#include "mafNodeIterator.h"

//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpValidateTree);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpValidateTree::mafOpValidateTree(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo = true;
  m_MSFPath = "";
}
//----------------------------------------------------------------------------
mafOpValidateTree::~mafOpValidateTree()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafOp* mafOpValidateTree::Copy()   
//----------------------------------------------------------------------------
{
	return new mafOpValidateTree(m_Label);
}
//----------------------------------------------------------------------------
void mafOpValidateTree::OpRun()
//----------------------------------------------------------------------------
{
  int result = ValidateTree();
  if (result == mafOpValidateTree::VALIDATE_SUCCESS)
  {
    wxMessageBox(_("Tree validation terminated succesfully!!."), _("Info"));
  }
  else if (result == mafOpValidateTree::VALIDATE_WARNING)
  {
    wxMessageBox(_("Tree Patched!! In log area you can find details."), _("Warning"));
  }
  else
  {
    wxMessageBox(_("Tree invalid!! In log area you can find details."), _("Warning"));
  }
  mafEventMacro(mafEvent(this,OP_RUN_OK));
}
//----------------------------------------------------------------------------
int mafOpValidateTree::ValidateTree()
//----------------------------------------------------------------------------
{
  int result = mafOpValidateTree::VALIDATE_SUCCESS;

  mafNode *node;
  mafVMERoot *root = mafVMERoot::SafeDownCast(m_Input->GetRoot());
  assert(root != NULL);
  int max_item_id = root->GetMaxItemId();
  int max_node_id = root->GetMaxNodeId();

  mafNodeIterator *iter = root->NewIterator();
  try
  {
    for (node = iter->GetFirstNode(); node; node = iter->GetNextNode())
    {
      // check node ID
      bool valid = node->IsValid();
      if (!valid && !node->IsMAFType(mafVMERoot))
      {
        ErrorLog(mafOpValidateTree::INVALID_NODE, node->GetName());
        node->UpdateId();
        if (result != mafOpValidateTree::VALIDATE_ERROR)
        {
          result = mafOpValidateTree::VALIDATE_WARNING;
        }
      }
      int numLinks = node->GetNumberOfLinks();
      if (numLinks > 0)
      {
        // check node links
        mafNode::mafLinksMap::iterator lnk_it;
        for (lnk_it = node->GetLinks()->begin(); lnk_it != node->GetLinks()->end(); lnk_it++)
        {
          if (lnk_it->second.m_Node == NULL)
          {
            ErrorLog(mafOpValidateTree::LINK_NULL, node->GetName(), lnk_it->first);
            result = mafOpValidateTree::VALIDATE_ERROR;
            continue;
          }
          valid = root->IsInTree(lnk_it->second.m_Node);
          if (!valid)
          {
            ErrorLog(mafOpValidateTree::LINK_NOT_PRESENT, lnk_it->second.m_Node->GetName());
            result = mafOpValidateTree::VALIDATE_ERROR;
          }
          valid = lnk_it->second.m_Node->IsValid();
          if (!valid && !lnk_it->second.m_Node->IsMAFType(mafVMERoot))
          {
            ErrorLog(mafOpValidateTree::INVALID_NODE, lnk_it->second.m_Node->GetName());
            result = mafOpValidateTree::VALIDATE_ERROR;
          }
        }
      }
      // check data (binary data files are associated to that file that 
      // have the DataVector => inherit from mafVMEGenericAbstract
      mafString urlString = "";
      mafString archiveFilename = "";
      mafString absFilename = "";
      mafVMEItem *item = NULL;
      bool singleFileMode = false;
      mafVMEGenericAbstract *vme = mafVMEGenericAbstract::SafeDownCast(node);
      if (vme)
      {
        mafDataVector *dv = vme->GetDataVector();
        if (dv)
        {
          for (int t = 0; t < dv->GetNumberOfItems(); t++)
          {
            if (m_MSFPath.IsEmpty())
            {
              mafEventIO e(this,NODE_GET_STORAGE);
              vme->ForwardUpEvent(e);
              mafStorage *storage = e.GetStorage();
              if (storage != NULL)
              {
                m_MSFPath = storage->GetURL();
                m_MSFPath.ExtractPathName();
              }
            }
            // binary files exists => VME is animated on data
            item = dv->GetItemByIndex(t);
            if (item == NULL)
            {
              ErrorLog(mafOpValidateTree::ITEM_NOT_PRESENT, vme->GetName());
              result = mafOpValidateTree::VALIDATE_ERROR;
            }
            else
            {
              int item_id = item->GetId();
              if (max_item_id < item_id)
              {
                ErrorLog(mafOpValidateTree::MAX_ITEM_ID_PATCHED, vme->GetName());
                root->SetMaxItemId(item_id);
                if (result != mafOpValidateTree::VALIDATE_ERROR)
                {
                  result = mafOpValidateTree::VALIDATE_WARNING;
                }
              }

              urlString = item->GetURL();
              if (urlString.IsEmpty())
              {
                ErrorLog(mafOpValidateTree::URL_EMPTY, vme->GetName());
                result = mafOpValidateTree::VALIDATE_ERROR;
              }
              archiveFilename = item->GetArchiveFileName();
              singleFileMode = dv->GetSingleFileMode();
              if (singleFileMode && archiveFilename.IsEmpty())
              {
                ErrorLog(mafOpValidateTree::ARCHIVE_FILE_NOT_PRESENT, vme->GetName());
                result = mafOpValidateTree::VALIDATE_ERROR;
              }
              absFilename = m_MSFPath;
              absFilename << "/";
              if (singleFileMode)
              {
                absFilename << archiveFilename;
              }
              else
              {
                absFilename << urlString;
              }
              if (!wxFileExists(absFilename.GetCStr()))
              {
                ErrorLog(mafOpValidateTree::BINARY_FILE_NOT_PRESENT, absFilename.GetCStr());
                result = mafOpValidateTree::VALIDATE_ERROR;
              }
            } // item != NULL
          } // for() on items
        } // if(dv)
      } // if(vme) vme is inherited from a mafVMEGenericAbstract => has binary data file
    } // for () on node into the tree
  } // try
  catch (...)
  {
    ErrorLog(mafOpValidateTree::EXCEPTION_ON_ITERATOR, iter->GetCurrentNode()->GetName());
  }
  iter->Delete();
  return result;
}
//----------------------------------------------------------------------------
void mafOpValidateTree::ErrorLog(int error_num, const char *node_name, const char *description)
//----------------------------------------------------------------------------
{
  switch(error_num)
  {
    case mafOpValidateTree::INVALID_NODE:
      mafLogMessage(_("Patched Node '%s' with an invalid ID!!"), node_name);
    break;
    case mafOpValidateTree::LINK_NOT_PRESENT:
      mafLogMessage(_("Link node '%s' is not present into the tree"), node_name);
    break;
    case mafOpValidateTree::LINK_NULL:
      mafLogMessage(_("Link '%s' of node '%s' is null"), description, node_name);
    break;
    case mafOpValidateTree::EXCEPTION_ON_ITERATOR:
      mafLogMessage(_("Exception occurred during iteration on node '%s'"), node_name);
    break;
    case mafOpValidateTree::ITEM_NOT_PRESENT:
      mafLogMessage(_("Item not present in node '%s'"), node_name);
    break;
    case mafOpValidateTree::MAX_ITEM_ID_PATCHED:
      mafLogMessage(_("Item of Node '%s' has caused Max item Id to be patched!!"), node_name);
    break;
    case mafOpValidateTree::URL_EMPTY:
      mafLogMessage(_("URL referring to binary data not present in node '%s'"), node_name);
    break;
    case mafOpValidateTree::BINARY_FILE_NOT_PRESENT:
      mafLogMessage(_("Binary data file '%s' not present"), node_name);
    break;
    case mafOpValidateTree::ARCHIVE_FILE_NOT_PRESENT:
      mafLogMessage(_("Archive data file not present for node '%s'"), node_name);
    break;
  }
}
