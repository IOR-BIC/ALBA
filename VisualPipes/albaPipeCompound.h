/*========================================================================= 
  Program: Multimod Application Framework RELOADED 
  Module: $RCSfile: albaPipeCompound.h,v $ 
  Language: C++ 
  Date: $Date: 2012-04-06 09:52:00 $ 
  Version: $Revision: 1.1.2.6 $ 
  Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
  ========================================================================== 
  Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
  See the COPYINGS file for license details 
  =========================================================================
*/
#ifndef __albaPipeCompound_H__
#define __albaPipeCompound_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaPipe.h"
#include "albaGUIDynamicVP.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkPiecewiseFunction;
class vtkVolumeProperty;
class vtkXRayVolumeMapper;
class vtkActor;
class vtkVolume;
class vtkLookupTable;
class vtkImageResample;
class albaGUIFloatSlider;
class wxNotebook;

/**
    class name: albaPipeCompound
    Pipe which works as a container of other pipes.
*/

class ALBA_EXPORT albaPipeCompound : public albaPipe
{
public:
  /** RTTI Macro */
  albaAbstractTypeMacro(albaPipeCompound, albaPipe);

protected:
  /** IDs for the GUI */
  enum PIPE_VIEWFLOW_WIDGET_ID
  {
    ID_TABCTRL = Superclass::ID_LAST,    
    ID_LAST,
  };
 
  typedef struct PAGE_GROUP
  {    
    bool bVMEOutput;                    ///<true, if szClassName is class name of VME output instead of VME
    const char* szClassName;            ///<class name of VME or VME output that is processed using this page, NULL if everything should be processed
    
    const albaGUIDynamicVP::SUPPORTED_VP_ENTRY* pPipes;  ///<list of available pipes
    int nDefaultPipeIndex;      ///<which pipe is constructed upon "AddNewPage" operation
    const char* szDefaultName;  ///<name of page upon "AddNewPage" operation

    bool bNameCanBeChanged;     ///<true, if the page name can be changed
    bool bNameSingleChange;     ///<true, if the page name cannot be changed after it was set by the user
    bool bVPCanBeChanged;       ///<true, if the pipe can be selected by the user
    bool bVPSingleChange;       ///<true, if after the pipe is selected, it cannot be changed
    bool bPageCanBeClosed;      ///<true, if the page can be closed
    bool bCanAddNewPages;       ///<true, if new pages can be added by the user
  } PAGE_GROUP;

  std::vector< PAGE_GROUP > m_PageGroups; ///<registered groups of pages
  std::vector< int > m_PagesGroupIndex;   ///<group associated with the the page

  albaSceneNode* m_SceneNode;  ///<current scene node to be used  
  wxBoxSizer* m_SbMainSizer;  ///<gui main container, can include either m_Notebook or m_FirstPage
  wxNotebook* m_Notebook;     ///<gui container for pages
  albaGUIDynamicVP* m_FirstPage; ///<reference to the only one page (if not included in the notebook)
  wxString m_FirstPageName;     ///<name of the only page (if not included in the notebook)

public:
  /** constructor */
	albaPipeCompound();
  /** destructor */
	virtual ~albaPipeCompound ();

public:
  /** Creates the VTK rendering pipeline */
  /*virtual*/ void Create(albaSceneNode *n);

	/** Processes events coming from GUI */
	/*virtual*/ void OnEvent(albaEventBase *alba_event);

protected:
  /** creation of the gui */
  /*virtual*/ albaGUI  *CreateGui();
  
  /** Creates page groups. The inherited classes must override this method.
  This method is called from Create method prior to CreatePages.
  It defines the behavior for different VMEs.
  N.B. Groups MAY NOT change after this function is called. */
  virtual void CreatePageGroups() = 0;

  /** Creates initial pages for each group. 
  Called from Create, must be called after CreatePageGroups. */
  virtual void CreatePages();

  /** Creates a new page for the specified group. */
  virtual albaGUIDynamicVP* CreateNewPage(const PAGE_GROUP& group);

  /** Handles the change of name on the current page. */
  virtual void OnChangeName();

  /** Handles the change of VP on the current page. */
  virtual void OnCreateVP();

  /** Handles the destruction of VP on the current page. */
  virtual void OnCloseVP();

  /**
	Called after page has been removed / added from / into the notebook. The
  routine detects how many pages are available in total. If two or more,
  notebook is used, otherwise only one page m_FirstPage is shown (without
  any tab). The change of layout can be forced, no matter on how many pages
  are valid, if bForce is true. Use this carefully. */
  virtual void UpdateGUILayout(bool bForce = false);

  vtkLookupTable              *m_ColorLUT;
  vtkPiecewiseFunction        *m_OpacityTransferFunction;
  vtkVolumeProperty           *m_VolumeProperty;
  vtkXRayVolumeMapper         *m_VolumeMapper;
  vtkVolume                   *m_Volume;
  vtkImageResample						*m_ResampleFilter;
  double  m_ResampleFactor;
   void AddActor();
};  
#endif // __albaPipeCompound_H__
