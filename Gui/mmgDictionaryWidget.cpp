/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmgDictionaryWidget.cpp,v $
  Language:  C++
  Date:      $Date: 2006-05-04 11:48:14 $
  Version:   $Revision: 1.1 $
  Authors:   Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mmgDictionaryWidget.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/laywin.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>

#include "mafDecl.h"
#include "mafEvent.h"
#include "mafTagArray.h"

#include "mmgListCtrl.h"
#include "mmgDictionaryWidget.h"

#include "mafVME.h"
#include "mafVMERoot.h"
#include "mafVMELandmarkCloud.h"

#include "mafXMLStorage.h"
#include "mafStorageElement.h"

wxString mmgDictionaryWidget::m_File = "";

enum 
{
	LOAD_DICT = MINID,
};
//----------------------------------------------------------------------------
mmgDictionaryWidget::mmgDictionaryWidget(wxWindow *parent, int id)
//----------------------------------------------------------------------------
{  
  m_NumItem = 0;
  m_Items = NULL;
  m_Vme   = NULL;

  m_List = new mmgListCtrl(parent,id,false,true);
  m_List->Show(false);
  m_List->SetListener(this);
  m_List->SetSize(wxSize(450,400));

  if(m_File != "") LoadDictionary(m_File);
}
//----------------------------------------------------------------------------
mmgDictionaryWidget::~mmgDictionaryWidget()
//----------------------------------------------------------------------------
{
  for(int i=0; i<m_NumItem; i++) delete m_Items[i];
  delete [] m_Items; 
  cppDEL(m_List);
}
//----------------------------------------------------------------------------
void mmgDictionaryWidget::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if(mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
	  switch(e->GetId())
	  {
			case ITEM_SELECTED:
	      mafEventMacro(*e);
			break;
	  }
	}
}
//----------------------------------------------------------------------------
void mmgDictionaryWidget::LoadDictionary()
//----------------------------------------------------------------------------
{
  wxString wild_dict	= "Dictionary file (*.dic)|*.dic|All files (*.*)|*.*";
  wxString m_dict		  = mafGetApplicationDirectory().c_str();
  m_dict = m_dict + "\\Config\\Dictionary\\";
	wxString file       = mafGetOpenFile(m_dict,wild_dict,"Choose Dictionary File",m_List).c_str(); 
	if(file != "") LoadDictionary(file);
}
//----------------------------------------------------------------------------
void mmgDictionaryWidget::LoadDictionary(wxString file)
//----------------------------------------------------------------------------
{
  m_File = file;
  
  // XML storage to restore
  mafXMLStorage restore;
  restore.SetURL(m_File);
  restore.SetFileType("DIC");
  restore.SetVersion("2.0");

  // create a new object to restore into
  mafStorableDictionary *storeDict = new mafStorableDictionary;
  restore.SetDocument(storeDict);
  restore.Restore();
  
	if(m_NumItem) delete [m_NumItem] m_Items;			//modified by Paolo 16-6-2003
  m_NumItem = storeDict->m_StrVector.size();
  m_Items = new wxString*[m_NumItem];

  m_List->Reset();
  m_List->SetColumnLabel(0, "names already in use are displayed with the red icon");
  for(int i=0; i<m_NumItem; i++)
  {
     m_Items[i] = new wxString(storeDict->m_StrVector[i]);
     m_List->AddItem(i,*m_Items[i]);
  }
  
  storeDict->Delete();
  ValidateAllItem();
}
//----------------------------------------------------------------------------
void mmgDictionaryWidget::SetCloud(mafVME *vme)
//----------------------------------------------------------------------------
{
  m_Vme = vme;
	ValidateAllItem();
}
//----------------------------------------------------------------------------
// SIL - to be removed
void mmgDictionaryWidget::ValidateItem(wxString item, bool valid)
//----------------------------------------------------------------------------
{
  ITEM_ICONS icon = (valid) ? ITEM_GRAY : ITEM_RED;

  for(int i=0; i<m_NumItem; i++)
  {
    if ( *m_Items[i] == item )
    {
      m_List->SetItemIcon(i,icon);
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mmgDictionaryWidget::ValidateAllItem(bool valid)
//----------------------------------------------------------------------------
{
  if(!m_Vme)
  {
		for(int i=0; i<m_NumItem; i++)
		{
      m_List->SetItemIcon(i,ITEM_GRAY);
		}
	}
  else
	{
    mafVMELandmarkCloud* lc = (mafVMELandmarkCloud*)m_Vme;
		for(int i=0; i<m_NumItem; i++)
		{
			ITEM_ICONS icon = (lc->FindInTreeByName(*m_Items[i])) ? ITEM_RED : ITEM_GRAY ;
			m_List->SetItemIcon(i,icon);
		}
	}
}

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafStorableDictionary);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafStorableDictionary::mafStorableDictionary()
//------------------------------------------------------------------------------
{
  ;
}
//------------------------------------------------------------------------------
mafStorableDictionary::~mafStorableDictionary()
//------------------------------------------------------------------------------
{
  m_StrVector.resize(0);
}
//------------------------------------------------------------------------------
// example of deserialization code
int mafStorableDictionary::InternalRestore(mafStorageElement *element)
//------------------------------------------------------------------------------
{
  m_StrVector.resize(element->GetChildren()[0]->GetChildren().size());
  
  if(element->RestoreVectorN("Dictionary",m_StrVector,element->GetChildren()[0]->GetChildren().size(),"DItem"))
    return MAF_ERROR;
  return MAF_OK;
}

