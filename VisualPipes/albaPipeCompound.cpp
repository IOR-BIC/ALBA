/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeCompound.cpp,v $ 
  Language: C++ 
  Date: $Date: 2009-09-04 10:29:43 $ 
  Version: $Revision: 1.1.2.3 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaDecl.h"

#include "vtkALBAAssembly.h"
#include "vtkALBASmartPointer.h"
#include "vtkImageData.h"
#include "vtkImageCast.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkXRayVolumeMapper.h"
#include "vtkPlaneSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkVolume.h" 
#include "vtkProperty.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkLookupTable.h"
#include "mmaVolumeMaterial.h"
#include "albaVMEVolume.h"
#include "vtkImageResample.h"

#include "albaPipeCompound.h"

#include "albaSceneNode.h"
#include "albaVME.h"
#include "albaGUI.h"
#include "albaGUIValidator.h"

#include "albaGUIDynamicVP.h"

#include "albaGUIBusyInfo.h"
#include "wx/notebook.h"
#include <float.h>

#include "albaDbg.h"

//----------------------------------------------------------------------------
albaCxxAbstractTypeMacro(albaPipeCompound);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeCompound::albaPipeCompound() : albaPipe()
//----------------------------------------------------------------------------
{
  m_SceneNode = NULL;
  m_Notebook = NULL;
  m_FirstPage = NULL; //no page available
}

//----------------------------------------------------------------------------
albaPipeCompound::~albaPipeCompound()
//----------------------------------------------------------------------------
{
  if (m_FirstPage != NULL){
    cppDEL(m_Notebook); //notebook is not included in m_sbMainSizer 
  }
}

//----------------------------------------------------------------------------
void albaPipeCompound::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  wxCursor busy;

	Superclass::Create(n);
  m_SceneNode = n; 
  
  GetGui(); //force construction of Notebook

  //creates description of groups
  CreatePageGroups();

  //creates initial pages for each group
  CreatePages();

  //AddActor();
}
//------------------------------------------------------------
//add an actor in the view to show init vme with a default type
void albaPipeCompound::AddActor(){
	m_ResampleFactor = 1.0;
	m_Selected = false;
	m_Vme->GetOutput()->Update();
	vtkDataSet* data = m_Vme->GetOutput()->GetVTKData();

	double sr[2];
	data->GetScalarRange(sr);

	vtkNEW(m_ColorLUT);
	m_ColorLUT->SetTableRange(sr);

	//vtkNEW(m_OpacityTransferFunction);
	mmaVolumeMaterial *material = ((albaVMEVolume *)m_Vme)->GetMaterial();
	m_OpacityTransferFunction = material->m_OpacityTransferFunction;

	vtkNEW(m_VolumeProperty);
	m_VolumeProperty->SetScalarOpacity(m_OpacityTransferFunction);
	m_VolumeProperty->SetInterpolationTypeToLinear();

	vtkNEW(m_ResampleFilter);
	vtkNEW(m_VolumeMapper);
	if(vtkImageData::SafeDownCast(data))
	{
		m_ResampleFilter->SetInput((vtkImageData*)data);
		for(int i=0;i<3;i++)
			m_ResampleFilter->SetAxisMagnificationFactor(i,m_ResampleFactor);
		m_ResampleFilter->Update();
		m_VolumeMapper->SetInput(m_ResampleFilter->GetOutput());
	}
	else
		m_VolumeMapper->SetInput(data);

	m_VolumeMapper->SetCroppingRegionPlanes(0, 1, 0, 1, 0, 1);

	vtkNEW(m_Volume);
	m_Volume->SetMapper(m_VolumeMapper);
	m_Volume->PickableOff();

	m_AssemblyFront->AddPart(m_Volume);

}

#pragma region A special GUI with autoresize feature
/** This small helper class is denoted to autoresize 
to ensure that pages are well visible. It is needed because
our gui is placed into scrolling window and it has also scrolling 
window with dynamically changing page. We want to use the entire
size reserved for the main scrolling window => derive sizes of
children from parent instead of vice versa as usually. */
class albaGUIAutoResized : public albaGUI
{
public:
  albaGUIAutoResized(albaObserver *listener) : albaGUI(listener) {    
  }

protected:
  //Called from SetSize methods
  //Ensures that the size is at least equaled to the size of
  //the parent of the GUI (this is to hack FitGui method)
  /*virtual*/ void DoSetSize(int x, int y, 
    int width, int height, int sizeFlags = wxSIZE_AUTO)
  {
    wxSize size = this->GetParent()->GetClientSize();
    int w = size.GetWidth();
    if (w > width)
      width = w;

    int h = size.GetHeight();
    if (h > height)
      height = h;

    albaGUI::DoSetSize(x, y, width, height, sizeFlags);
  }
};

#pragma endregion A special GUI with an autoresize feature

//----------------------------------------------------------------------------
albaGUI *albaPipeCompound::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUIAutoResized(this);

#pragma region GUI Controls
  m_SbMainSizer = new wxBoxSizer( wxVERTICAL );
  m_Notebook = new wxNotebook( m_Gui, ID_TABCTRL, 
    wxDefaultPosition, wxDefaultSize, wxNB_TOP);

  m_SbMainSizer->Add( m_Notebook, 1, wxEXPAND | wxALL, 0 );
  m_Gui->Add(m_SbMainSizer, 1, wxEXPAND);  
#pragma endregion GUI Controls
  
  return m_Gui;
}

//----------------------------------------------------------------------------
void albaPipeCompound::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{			  
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{	
    if (e->GetId() == ID_TABCTRL)
    {        
      //this was sent from some page (i.e., pipe update, etc.)
      switch (e->GetArg())
      {
      case albaGUIDynamicVP::ID_NAME:
        OnChangeName();
        break;

      case albaGUIDynamicVP::ID_CREATE_VP:
        OnCreateVP();
        break;

      case albaGUIDynamicVP::ID_CLOSE_VP:
        OnCloseVP();
        break;
      }

			GetLogicManager()->CameraUpdate();
      return;
    }    

    //Superclass::OnEvent(alba_event);
	}
  
  //forward the event to our listener to deal with it
  albaEventMacro(*alba_event);    
}

//------------------------------------------------------------------------
//Creates initial pages for each group.
/*virtual*/ void albaPipeCompound::CreatePages()
//------------------------------------------------------------------------
{
  const char* VMEclassnames[2] = {    
    m_Vme->GetOutput()->GetTypeName(), m_Vme->GetTypeName()
  };

  int nCount = (int)m_PageGroups.size();
  for (int i = 0; i < nCount; i++)
  {
    PAGE_GROUP& group = m_PageGroups[i];
    
    //detect what VME we have here
    if (group.szClassName != NULL &&          
      strcmp(group.szClassName, VMEclassnames[group.bVMEOutput ? 0 : 1]) != 0)
      continue;  //this group is not valid for the current VME
    
    //so let us create new page
    albaGUIDynamicVP* newpage = CreateNewPage(group);
    
    wxString szName = wxString(newpage->GetName());
    if (group.bNameCanBeChanged)
      szName += _(" *");
    
    m_Notebook->AddPage(newpage, szName);    
    m_PagesGroupIndex.push_back(i);    
  } 

  //additional index to speedup some process
  m_PagesGroupIndex.push_back(-1);

  UpdateGUILayout();  
}

//------------------------------------------------------------------------
// Creates a new page for the specified group.
/*virtual*/ albaGUIDynamicVP* albaPipeCompound::CreateNewPage(const PAGE_GROUP& group)
//------------------------------------------------------------------------
{
  long GUIStyle = 0;
  if (!group.bNameCanBeChanged)
    GUIStyle = albaGUIDynamicVP::GS_NO_NAME;

  //check if it makes sense to display change "VP"
  if (group.bVPCanBeChanged && group.nDefaultPipeIndex >= 0)
  {
    if (group.pPipes[1].szClassName == NULL)
      GUIStyle |= albaGUIDynamicVP::GS_NO_CREATE_VP;
  }
  else if (!group.bVPCanBeChanged)
  {
    GUIStyle |= albaGUIDynamicVP::GS_NO_CREATE_VP;
    _ASSERT(group.nDefaultPipeIndex >= 0);
  }

  if (!group.bPageCanBeClosed || group.nDefaultPipeIndex < 0)
    GUIStyle |= albaGUIDynamicVP::GS_NO_CLOSE_VP;

  albaGUIDynamicVP* newpage = new albaGUIDynamicVP(m_Notebook, ID_TABCTRL, GUIStyle);
  newpage->SetListener(this);
  newpage->SetSceneNode(m_SceneNode);    
  newpage->SetVPipesList(group.pPipes);    
  newpage->SetVPipeIndex(group.nDefaultPipeIndex);
  
  if (group.szDefaultName != NULL)    
    newpage->SetName(group.szDefaultName);  

  return newpage;
}

//------------------------------------------------------------------------
//Handles the change of name on the current page.
/*virtual*/ void albaPipeCompound::OnChangeName()
//------------------------------------------------------------------------
{
  if (m_FirstPage != NULL)
    m_FirstPageName = m_FirstPage->GetName();
  else
  {
    int nCurSel = m_Notebook->GetSelection();
    _ASSERT_RET(nCurSel >= 0);

    albaGUIDynamicVP* page = (albaGUIDynamicVP*)m_Notebook->GetCurrentPage();  
    _ASSERT_RET(page != NULL);

    m_Notebook->SetPageText(nCurSel, page->GetName());
  }
}

//------------------------------------------------------------------------
//Handles the change of VP on the current page.
/*virtual*/ void albaPipeCompound::OnCreateVP()
//------------------------------------------------------------------------
{ 
  //there are several modes we want to support
  //1) for every group there is zero or more permanent pages 
  // and one "new" page. Visual pipe on "new" page is unspecified.
  // When the user selects the visual pipe on this new page, this
  // page becomes permanents, which involves: name for this page is
  // created, close, create widgets, etc. are set to be hidden and
  // the user no longer can change visual pipe on this page.
  // New "new" page is created. 
  //
  //2) as the previous mode but visual pipe can be changed as many 
  // times as needed, so in this case the "new" page should be 
  // set only, if the current page is the "new" page
  //
  //3) there is no new page when VP changes (dtto version 1 or 2 with 
  //nCanAddNewPage = false)

  int nCurSel = m_FirstPage != NULL ? 0 : m_Notebook->GetSelection();
  _ASSERT_RET(nCurSel >= 0);

  albaGUIDynamicVP* page = m_FirstPage != NULL ? m_FirstPage : 
    (albaGUIDynamicVP*)m_Notebook->GetCurrentPage();  
  _ASSERT_RET(page != NULL);

  int nGroupId = m_PagesGroupIndex[nCurSel];  
  PAGE_GROUP& group = m_PageGroups[nGroupId];  

  //change name
  wxString szName = page->GetName();
  if (szName.Trim().IsEmpty())
  {
    if (group.szDefaultName != NULL)
      szName = group.szDefaultName;
    else
      szName = group.pPipes[page->GetVPipeIndex()].szUserFriendlyName;
  }
  
  if (m_FirstPage != NULL)
    m_FirstPageName = szName;
  else
    m_Notebook->SetPageText(nCurSel, szName);
  
  //modify styles
  long GUINewStyle = 0;
  if (!group.bPageCanBeClosed)
    GUINewStyle = albaGUIDynamicVP::GS_NO_CLOSE_VP;

  if (!group.bNameCanBeChanged || group.bNameSingleChange)
    GUINewStyle |= albaGUIDynamicVP::GS_NO_NAME;

  if (group.bVPSingleChange)
    GUINewStyle |= albaGUIDynamicVP::GS_NO_CREATE_VP;
  page->SetGUIStyle(GUINewStyle);
  
  //now check, if we can add new "new" page
  if (group.bCanAddNewPages)
  {
    //detect, if the current page is the "new" page for the current group
    //the array is +1 long, there is terminating -1 element
    bool bIsNewPage = nGroupId != m_PagesGroupIndex[nCurSel + 1];
    if (bIsNewPage)
    {
      //so let us create new page
      albaGUIDynamicVP* newpage = CreateNewPage(group);

      wxString szName = wxString(newpage->GetName());
      if (group.bNameCanBeChanged)
        szName += _(" *");

      m_Notebook->InsertPage((m_FirstPage == NULL ? nCurSel + 1 : 0), newpage, szName);
      m_PagesGroupIndex.insert(m_PagesGroupIndex.begin() + nCurSel, nGroupId);   

      UpdateGUILayout();
    }
  }

  m_Gui->FitGui();   //ensure m_Gui is of the correct size
  m_Gui->Layout();   //resize m_Gui controls to fit the new m_Gui client size
}

//------------------------------------------------------------------------
//Handles the destruction of VP on the current page. 
/*virtual*/ void albaPipeCompound::OnCloseVP()
//------------------------------------------------------------------------
{
  if (m_FirstPage != NULL)
  {
    //we will destroy everything      
    UpdateGUILayout(true);
  }
  
  int nCurSel = m_Notebook->GetSelection();
  _ASSERT_RET(nCurSel >= 0);

  PAGE_GROUP& group = m_PageGroups[m_PagesGroupIndex[nCurSel]];
  _ASSERT_RET(group.bPageCanBeClosed != false);

  m_PagesGroupIndex.erase(m_PagesGroupIndex.begin() + nCurSel);
  m_Notebook->DeletePage(nCurSel);

  UpdateGUILayout();  
  m_Gui->FitGui();   //ensure m_Gui is of the correct size
  m_Gui->Layout();   //resize m_Gui controls to fit the new m_Gui client size
}

//------------------------------------------------------------------------
//Called after page has been removed / added from / into the notebook. The
//routine detects how many pages are available in total. If two or more,
//notebook is used, otherwise only one page m_FirstPage is shown (without
//any tab). The change of layout can be forced, no matter on how many pages
//are valid, if bForce is true. Use this carefully.
/*virtual*/ void albaPipeCompound::UpdateGUILayout(bool bForce)
//------------------------------------------------------------------------
{
  if (!bForce)
  {
    int nCount = (int)m_Notebook->GetPageCount();
    if (nCount != 1) 
    {
      _ASSERT(m_FirstPage == NULL);
      return; //notebook is already valid
    }
  }

  //notebook contains only one page => there will be a change
  if (m_FirstPage == NULL)
  {
    //there is only one page available in the notebook
    m_FirstPage = (albaGUIDynamicVP*)m_Notebook->GetPage(0);
    m_FirstPageName = m_Notebook->GetPageText(0);
    m_Notebook->RemovePage(0);

    m_FirstPage->Reparent(m_Gui);
    m_SbMainSizer->Detach(m_Notebook);
    m_SbMainSizer->Add(m_FirstPage, 1, wxEXPAND | wxALL, 0 );

    m_Notebook->Reparent(albaGetFrame());
    m_Notebook->Show(false);
  }
  else
  {
    m_Notebook->Reparent(m_Gui);
    m_Notebook->Show(true);

    m_FirstPage->Reparent(m_Notebook);
    m_SbMainSizer->Detach(m_FirstPage);
    
    m_Notebook->InsertPage(0, m_FirstPage, m_FirstPageName);
    m_SbMainSizer->Add( m_Notebook, 1, wxEXPAND | wxALL, 0 );
    m_FirstPage = NULL;
  }
}