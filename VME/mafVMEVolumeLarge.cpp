/**=======================================================================

File:    	 mafVMEVolumeLarge.cpp
Language:  C++
Date:      8:2:2008   11:28
Version:   $Revision: 1.1.2.3 $
Authors:   Josef Kohout (Josef.Kohout@beds.ac.uk)

Copyright (c) 2008
University of Bedfordshire
=========================================================================
See: Agrawal A, Kohout J, Clapworthy GJ, McFarlane NJB, Dong F, Viceconti M, 
Taddei F, Testi D. Interactive Out-of-Core Exploration of Large Volume 
Datasets in VTK-Based Visualisation Systems. 
In: Proceedings of EG UK Theory and Practice of Computer Graphics (TP.CG. 2008), 
June 9-11, 2008, Manchester, UK, p. 1-8
=========================================================================*/
#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafVMEVolumeLarge.h"
#include "mafVMEVolumeLargeUtils.h"
#include "mafVMEVolumeGray.h"
#include "mafDataPipeInterpolatorVTK.h"
#include "mafDataVector.h"
#include "mafVMEItemVTK.h"
#include "mafTagArray.h"
#include "mafVMEStorage.h"
#include "mafStorageElement.h"
#include "mafGUI.h"
#include "mafGUIRollOut.h"

#include "vtkDataArray.h"
#include "vtkDataSet.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPointData.h"
#include "vtkImageClip.h"

#include "mafGizmoROI_BES.h"
#ifndef VME_VOLUME_LARGE_EXCLUDE_CROP
#include "vtkMAFLargeImageData.h"
#include "vtkMAFFileDataProvider.h"
//#include "vtkMAFMemMapFileDataProvider.h"
#endif

#include <stack>
#include <wx/busyinfo.h>
#include "vtkMAFSmartPointer.h"

#if defined(_DEBUG) && defined(_WIN32)
//#define _PROFILE_LARGEDATA_UPDATE
#endif

//-------------------------------------------------------------------------
mafCxxTypeMacro(mafVMEVolumeLarge)
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
mafVMEVolumeLarge::mafVMEVolumeLarge()
//-------------------------------------------------------------------------
{
#ifdef VME_VOLUME_VER1
  m_LargeData = NULL;
#else
  m_LargeDataReader = NULL;
#endif

  for (int i = 0; i < 6; i++) 
  {
    m_FullExtent[i] = 0;
    m_VOI[i] = 0;
    m_ROI[i] = 0.0;
  }

  m_SampleInfoGui = m_ShowROIOpt = m_CropEdVxls = NULL;
  m_CropEdMm = m_InfoGui = m_CropGui = NULL;
  m_VOIUnits = 0;

  m_ShowHandles = 1;
  m_ShowAxis = 1;
  m_ShowPlanes = 1;
  m_ShowROI = 0;
  m_GizmoROI = NULL;		

  m_SampleMemLimit = 16;	//16 MB
  m_AutoProof = 0;
  m_AutoProofZone = 32;   //16 voxels => 32 per side => (N+32)*(N+32)*(N + 32) increase
#ifdef VME_VOLUME_VER1
  m_FEOutput = NULL;	
  m_ShowFEOutput = true;

  m_ProgressCallback = vtkMAFLargeDataSetCallback::New();
  m_ProgressCallback->SetListener(this);
#endif
}

//-------------------------------------------------------------------------
mafVMEVolumeLarge::~mafVMEVolumeLarge()
//-------------------------------------------------------------------------
{
  // data pipe destroyed in mafVME
  // data vector destroyed in mafVMEGeneric	
#ifdef VME_VOLUME_VER1
  if (m_LargeData != NULL) {
    m_LargeData->RemoveObserver(m_ProgressCallback);
    vtkDEL(m_LargeData);
  }

  vtkDEL(m_ProgressCallback);
  vtkDEL(m_FEOutput);
#endif

  cppDEL(m_GizmoROI);
  cppDEL(m_SampleInfoGui);
  cppDEL(m_InfoGui);
  cppDEL(m_CropEdVxls);
  cppDEL(m_CropEdMm);
  cppDEL(m_ShowROIOpt);
  cppDEL(m_CropGui);	

#ifndef VME_VOLUME_VER1
  cppDEL(m_LargeDataReader);
#endif
}

//-------------------------------------------------------------------------
char** mafVMEVolumeLarge::GetIcon() 
//-------------------------------------------------------------------------
{
#include "mafVMEVolumeLarge.xpm"
  return mafVMEVolumeLarge_xpm;
}

#include "mmaVolumeMaterial.h"
#include "vtkLookupTable.h"

//-------------------------------------------------------------------------
int mafVMEVolumeLarge::InternalInitialize()
//-------------------------------------------------------------------------
{
//BES: 11.7.2008 - THIS IS HERE BECAUSE OF PAOLO'S BUG IN mafVMEVolume
//THAT PREVENT CORRECT MATERIAL CONSTRUCTION
//WHEN IT IS FIXED, NEITHER InternalInitialize NOR GetMaterial
//IS NEEDED TO BE IN mafVMEVolumeLarge
  if (mafVMEGeneric::InternalInitialize()==MAF_OK)
  {
    // force material allocation   
    GetMaterial();
    return MAF_OK;
  }
  return MAF_ERROR;
}

//-------------------------------------------------------------------------
mmaVolumeMaterial *mafVMEVolumeLarge::GetMaterial()
//-------------------------------------------------------------------------
{
  mmaVolumeMaterial *material = (mmaVolumeMaterial *)GetAttribute("VolumeMaterialAttributes");
  if (material == NULL)
  {
    material = mmaVolumeMaterial::New();
    
    if(GetOutput() && GetOutput()->GetVTKData())
    {
      GetOutput()->GetVTKData()->Update();
      double sr[2];
      GetOutput()->GetVTKData()->GetScalarRange(sr);
      material->m_ColorLut->SetTableRange(sr);
      material->m_ColorLut->SetRange(sr);
      material->UpdateFromTables();
    }

    SetAttribute("VolumeMaterialAttributes", material);
    if (m_Output)
    {
      ((mafVMEOutputVolume *)m_Output)->SetMaterial(material);
    }
  }
  return material;
}


/** called to prepare the update of output */
/*virtual*/ void mafVMEVolumeLarge::InternalPreUpdate()
{
  Superclass::InternalPreUpdate();	//call base

  //This routine is called, when a new data is put into the data pipeline
  //During VTK UpdateInformation - ExecuteInformation chain
  //that propagates extents in the pipeline
  //vtkImageClip, vtkStructuredGridClip and vtkRectilinearGridClip that
  //may be present in the VTK pipeline remember previous WholeExtent 
  //and ignores new values, thus we will need to force them to register
  //the whole extent of new data

  std::stack<vtkDataObject*> stck;
#ifdef VME_VOLUME_VER1
  vtkDataObject* pLargeData = m_LargeData->GetSnapshot();  
#else
  vtkDataObject* pLargeData = 
    m_LargeDataReader->IsRectilinearGrid() ? 
    (vtkDataObject*)m_LargeDataReader->GetOutputRLGDataSet() :
    (vtkDataObject*)m_LargeDataReader->GetOutputDataSet();
#endif
  
  stck.push(pLargeData);
  while (!stck.empty())
  {
    vtkDataObject* pDataObj = stck.top();    
    stck.pop(); //and remove it from the stack

    int nConsumers = pDataObj->GetNumberOfConsumers();
    for (int i = 0; i < nConsumers; i++)
    {
      //get the vtkMAFDataPipe object that takes the data as the input  
      vtkSource* pSource = vtkSource::SafeDownCast(pDataObj->GetConsumer(i));  
      if (pSource == NULL)
        continue; //it is not a source

      //there is some source, push all its outputs
      int nOutputs = pSource->GetNumberOfOutputs();
      for (int j = 0; j < nOutputs; j++) {
        stck.push(pSource->GetOutputs()[j]);
      }

      vtkImageClip* pClip = vtkImageClip::SafeDownCast(pSource);
      if (pClip != NULL) { //reset whole extent
        pClip->SetOutputWholeExtent(pLargeData->GetWholeExtent());
      }

      //NB. vtkStructuredGridClip and vtkRectilinearGridClip are not supported
      //in VTK 4.4 used currently by openMAF
    } //end for   
  } //end while
}

/** Internally called to update the output */
/*virtual*/ void mafVMEVolumeLarge::InternalUpdate()
{
	Superclass::InternalUpdate();	//call base

	unsigned long mtime = this->GetMTime();
	if (mtime > m_UpdateTime)
	{
		m_UpdateTime.Modified();
	
		UpdateGui();
	}
}

#ifdef VME_VOLUME_VER1
/** Overridden to take into consideration the internal m_LargeData time. */
/*virtual*/ unsigned long mafVMEVolumeLarge::GetMTime()
{
	unsigned long mt = Superclass::GetMTime();
	if (m_LargeData != NULL)
	{
		unsigned long ld_mt = m_LargeData->GetMTime();
		if (ld_mt > mt)
			mt = ld_mt;
	}

	return mt;
}
#endif

//-------------------------------------------------------------------------
void mafVMEVolumeLarge::OnEvent(mafEventBase *maf_event)
//-------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch (e->GetId())
		{
		case ID_COMBO_VOI_UNITS:
			UpdateCropGui(m_CropGui);
			m_Gui->Update();
			break;

		case ID_SAMPLE_MEMLIMIT:
			m_SampleInfoGui->Update();
#ifndef VME_VOLUME_VER1
      if (m_AutoProof)
        OnAutoProofROIVolume();
#endif
			break;

		case ID_SHOW_HANDLES: 
			{
				m_GizmoROI->ShowHandles(m_ShowHandles != 0);
				mafEvent ev(this, CAMERA_UPDATE);
				this->ForwardUpEvent(&ev);
			}
			break;

		case ID_SHOW_AXIS: 
			{
				m_GizmoROI->ShowAxis(m_ShowAxis != 0);
				mafEvent ev(this, CAMERA_UPDATE);
				this->ForwardUpEvent(&ev);
			}
			break;

		case ID_SHOW_PLANES: 
			{
				m_GizmoROI->ShowPlanes(m_ShowPlanes != 0);
				mafEvent ev(this, CAMERA_UPDATE);
				this->ForwardUpEvent(&ev);
			}
			break;

		case ID_SHOW_ROI:
			OnToggleGizmo();
			break;		

		case ID_CROP_DIR_X:
		case ID_CROP_DIR_XMM:
		case ID_CROP_DIR_Y:
		case ID_CROP_DIR_YMM:
		case ID_CROP_DIR_Z:
		case ID_CROP_DIR_ZMM:
			{
				//update the gizmo to fit the specified coordinates
				UpdateGizmo();

        if (m_AutoProof)
          OnAutoProofROIVolume();
        else
        {
				  mafEvent ev(this, CAMERA_UPDATE);
				  this->ForwardUpEvent(&ev);
        }
			}
			break;

		case ID_TRANSFORM:			
			//Gizmo -> variables -> GUI controls			
			UpdateCropGui(m_CropGui);
			m_Gui->Update();

      if (m_AutoProof)
        OnAutoProofROIVolume();
			break;			

		case ID_RESET_CROPPING_AREA:
			OnResetROI();			
			break;

    case ID_AUTOPROOF:
      if (m_AutoProof)
        OnViewROIVolume();
      break;
      
		case ID_VIEW_ROI_VOLUME:
			OnViewROIVolume();			
			break;

		case ID_VIEW_ORIGVOI_VOLUME:
			OnViewOrigVOIVolume();
			break;

#ifndef VME_VOLUME_LARGE_EXCLUDE_CROP
		case ID_CROP:
			OnCrop();			
			break;
#endif // VME_VOLUME_LARGE_EXCLUDE_CROP

#ifdef _TEMP_STUFF
		case ID_TEMP_FUNC:
			OnTempFunc();
			break;
#endif

			//default:
			//Superclass::OnEvent(maf_event);
		}
	}

	Superclass::OnEvent(maf_event);
}

//processes the ROI toggle command, creates or destroys gizmo
/*virtual*/ void mafVMEVolumeLarge::OnToggleGizmo()
{
	if (m_ShowROI == 0) 
	{
		if ( m_GizmoROI != NULL)
		{      
      //N.B. both, Show method and the destructor, sends events 
      //that may lead into the processing of destruction request
      //and thus into calling of UnRegister function
      //if m_GizmoROI is not NULL that time, the dtor will be called twice!
      mafGizmoROI_BES* roi = m_GizmoROI;
      m_GizmoROI = NULL;

      roi->Show(false);
      cppDEL(roi);     
		}
	}
	else
	{
		//detect if we have a valid view
		mafEvent ev(this, VIEW_SELECTED);
		this->ForwardUpEvent(&ev);

		if (!ev.GetBool())
		{
			wxMessageBox(_("You must have an opened valid view for this operation."));
			m_ShowROI = 0;
		}
		else
		{	
			double usrBounds[6];
			TransformExtent(m_FullExtent, usrBounds);
			m_GizmoROI = new mafGizmoROI_BES(this, this, mafGizmoROI_BES::USER_BOUNDS, 
        this, /*GetParent(),*/ usrBounds);			
			m_GizmoROI->SetBounds(m_ROI);
			m_GizmoROI->Show(true);

			//probably this is not necessary
			ev.SetId(CAMERA_UPDATE);
			this->ForwardUpEvent(&ev);		
		}
	}
	
	m_ShowROIOpt->Enable(ID_SHOW_HANDLES, m_ShowROI != 0);
	m_ShowROIOpt->Enable(ID_SHOW_AXIS, m_ShowROI != 0);
	m_ShowROIOpt->Enable(ID_SHOW_PLANES, m_ShowROI != 0);
	m_ShowROIOpt->Update();
	m_CropGui->Update();
}


//------------------------------------------------------------------------
//handles the change of ROI in auto proof mode
/*virtual*/ void mafVMEVolumeLarge::OnAutoProofROIVolume()
//------------------------------------------------------------------------
{
#ifdef VME_VOLUME_VER1
  OnViewROIVolume();
#else
  //first, detect the movement of ROI
  int nDirs = 0;
  int* pCurVOI = m_LargeDataReader->GetVOI();
  for (int i = 0; i < 6; i += 2)
  {
    if (m_VOI[i] < pCurVOI[i])
      nDirs |= 1 << i;

    if (m_VOI[i + 1] > pCurVOI[i + 1])
      nDirs |= 1 << (i + 1);
  }


  //next, check whether the new ROI differs in the quality of current ROI    
  int nCurrentLevel = m_LargeDataReader->GetSampleRate();
  unsigned int nMemoryLimit = m_SampleMemLimit*1024;

  int nNewLevel = m_LargeDataReader->ComputeSampleRate(m_VOI, nMemoryLimit);
  if (nCurrentLevel == nNewLevel && nDirs == 0)
    return; //no change in resolution and the new ROI is fully within the current ROI  


  int FullVOI[6];  
  for (int i = 0; i < 6; i++) { //align FullVOI
      FullVOI[i] = (m_FullExtent[i] / nNewLevel) * nNewLevel;
  }
  

  int AutoProofVOI[6];
  memcpy(AutoProofVOI, m_VOI, sizeof(AutoProofVOI));

  //compute new ProofVOI taking the user movement into the consideration
  if (nDirs != 0)
  {
    int nAutoProofZone = m_AutoProofZone;
    while (nAutoProofZone != 0)
    {
      int nExtent = nAutoProofZone*nNewLevel;
      int nMask = 1, nMult = 1;
      for (int i = 0; i < 6; i++)
      {
        //extend VOI in this direction
        if ((nDirs & nMask) != 0)
        {
          AutoProofVOI[i] -= nMult*nExtent;
          if (nMult*AutoProofVOI[i] < nMult*FullVOI[i])
            AutoProofVOI[i] = FullVOI[i];
        }

        nMask *= 2; nMult = -nMult;
      }

      int nNewAPLevel = m_LargeDataReader->ComputeSampleRate(AutoProofVOI, nMemoryLimit);
      if (nNewLevel == nNewAPLevel)
        break;  //this AutoProofVOI is valid

      nAutoProofZone /= 2;  //try smaller zone
      memcpy(AutoProofVOI, m_VOI, sizeof(AutoProofVOI));
    } //int 
  } //endif nDirs

  //and now process the data
  //{ is here to avoid rapid cursor change
  {
    wxBusyCursor busyCursor;

    //set a new VOI
    m_LargeDataReader->SetVOI(AutoProofVOI);
    m_LargeDataReader->SetMemoryLimit(m_SampleMemLimit*1024);
    m_LargeDataReader->Update();
    
    //update VOI to fit the new AutoProofVOI
    pCurVOI = m_LargeDataReader->GetVOI();
    for (int i = 0; i < 3; i++)
    {
      if (m_VOI[i] < pCurVOI[i])
        m_VOI[i] = pCurVOI[i];

      i++;
      if (m_VOI[i] > pCurVOI[i])
        m_VOI[i] = pCurVOI[i];
    }

    UpdateVOI(m_VOI);

    SetData((m_LargeDataReader->IsRectilinearGrid() ?
      (vtkDataSet*)m_LargeDataReader->GetOutputRLGDataSet() :
      (vtkDataSet*)m_LargeDataReader->GetOutputDataSet()), 0, MAF_VME_REFERENCE_DATA);
    this->Modified();
    //force redraw
    mafEvent ev(this, VME_SELECTED,this);
    this->ForwardUpEvent(&ev);
    UpdateGui();
  }
#endif
}


//resets the VOI into the full VOI
/*virtual*/ void mafVMEVolumeLarge::OnResetROI()
{	
	memcpy(m_VOI, m_FullExtent, sizeof(m_VOI));
	TransformExtent(m_VOI, m_ROI);
	
	if (m_GizmoROI != NULL)
		m_GizmoROI->SetBounds(m_ROI);

	UpdateCropGui(m_CropGui);
	m_Gui->Update();

#ifdef VME_VOLUME_VER1
	if (!m_ShowFEOutput)	//switch to the whole volume
		OnViewOrigVOIVolume();
	else
	{
		mafEvent ev(this, CAMERA_UPDATE);
		this->ForwardUpEvent(&ev);
	}
#else
	OnViewOrigVOIVolume();
#endif // VME_VOLUME_VER1
}

//zooms to the selected ROI only
/*virtual*/ void mafVMEVolumeLarge::OnViewROIVolume()
{
	wxBusyCursor busyCursor;

#ifdef VME_VOLUME_VER1
	m_ShowFEOutput = false;
	if (m_LargeData->GetSnapshot() == NULL)
		CreateSnapshotObject();	//make the current item from DataVector to be our snapshot object

	unsigned long t1 = m_LargeData->GetSnapshot()->GetMTime();
	CheckFEOutput();
	
	//set a new VOI
	m_LargeData->SetVOI(m_VOI);
	m_LargeData->SetMemoryLimit(m_SampleMemLimit*1024);
#else
	//set a new VOI
	m_LargeDataReader->SetVOI(m_VOI);
	m_LargeDataReader->SetMemoryLimit(m_SampleMemLimit*1024);
#endif

#ifdef _PROFILE_LARGEDATA_UPDATE
	LARGE_INTEGER liBegin;
	::QueryPerformanceCounter(&liBegin);
#endif

#ifdef VME_VOLUME_VER1
	m_LargeData->Update();
#else
	m_LargeDataReader->Update();
#endif

#ifdef _PROFILE_LARGEDATA_UPDATE
	LARGE_INTEGER liEnd, liFreq;
	::QueryPerformanceCounter(&liEnd);
	::QueryPerformanceFrequency(&liFreq);

	wxMessageBox(wxString::Format("m_LargeData->Update() in %d ms",
		(int)(((liEnd.QuadPart - liBegin.QuadPart)*1000) / liFreq.QuadPart)));

	//NOTE: Snapshot whole extent is destroyed now because of wxMessageBox
#endif

#ifdef VME_VOLUME_VER1
	SetData(m_LargeData->GetSnapshot(), 0, MAF_VME_REFERENCE_DATA);

	unsigned long t2 = m_LargeData->GetSnapshot()->GetMTime();
	if (t1 != t2) 
	{
		this->Modified();
		UpdateOutput();	
	}
#else
  UpdateVOI();
	SetData((m_LargeDataReader->IsRectilinearGrid() ?
    (vtkDataSet*)m_LargeDataReader->GetOutputRLGDataSet() :
    (vtkDataSet*)m_LargeDataReader->GetOutputDataSet()), 0, MAF_VME_REFERENCE_DATA);
	this->Modified();
//	UpdateOutput();	

  //force redraw
  mafEvent ev(this, VME_SELECTED,this);
  this->ForwardUpEvent(&ev);
  UpdateGui();
#endif
}

//returns back to the original ROI only
/*virtual*/ void mafVMEVolumeLarge::OnViewOrigVOIVolume()
{	
	wxBusyCursor busyCursor;	

#ifdef VME_VOLUME_VER1
	m_ShowFEOutput = true;
	if (m_LargeData->GetSnapshot() == NULL)
		CreateSnapshotObject();	//make the current item from DataVector to be our snapshot object

	CheckFEOutput();

	if (m_FEOutput == NULL || 
		m_LargeData->GetMemoryLimit() != m_SampleMemLimit*1024
		)
	{
		//we have to construct FE volume 
		int VOI[6], sample_rate[3];
		m_LargeData->GetVOI(VOI);
		m_LargeData->GetSampleRate(sample_rate);
		vtkDataSet* oldShot = m_LargeData->GetSnapshot();
		oldShot->Register(NULL);	//must prevent the destruction

		if (m_FEOutput)
			m_FEOutput->Delete();

		m_FEOutput = oldShot->NewInstance();
		m_LargeData->SetSnapshot(m_FEOutput);
		m_LargeData->SetVOI(m_FullExtent);
		m_LargeData->SetMemoryLimit(m_SampleMemLimit*1024);
		m_LargeData->Update();

		m_LargeData->GetSampleRate(m_FESampleRate);

		m_LargeData->SetVOI(VOI);	//unfortunately, this makes snapshot obsolete
		m_LargeData->SetSampleRate(sample_rate);
		m_LargeData->SetSnapshot(oldShot);				
		oldShot->UnRegister(NULL);		
	}

	SetData(m_FEOutput, 0, MAF_VME_REFERENCE_DATA);
#else
	m_LargeDataReader->SetVOI(m_FullExtent);
	m_LargeDataReader->SetMemoryLimit(m_SampleMemLimit*1024);
	m_LargeDataReader->Update();
  UpdateVOI();

	SetData((m_LargeDataReader->IsRectilinearGrid() ?
    (vtkDataSet*)m_LargeDataReader->GetOutputRLGDataSet() :
    (vtkDataSet*)m_LargeDataReader->GetOutputDataSet()), 0, MAF_VME_REFERENCE_DATA);
#endif
		
	this->Modified();
//	UpdateOutput();	
    
  //force redraw
  mafEvent ev(this, VME_SELECTED,this);
  this->ForwardUpEvent(&ev);
  UpdateGui();
}

#ifndef VME_VOLUME_LARGE_EXCLUDE_CROP
//performs the cropping
/*virtual*/ void mafVMEVolumeLarge::OnCrop()
{
  ////TODO: remove
  //static int iTest = 0;
  //typedef int VOI_TYPE[6];
  //VOI_TYPE VOIs[] = {
  //  {0,2040,0,1200,0,5160 },
  //  {0,2037,0,1197,0,3885 },
  //  {0,2040,0,1200,0,3120 },
  //  {0,2034,0,1206,0,2340 },
  //  {0,2040,0,1200,1110,2340 },
  //  {1056,2040,0,1200,1104,2340 },
  //  {1053,2034,0,1206,1791,2340 },
  //  {1050,2037,756,1204,1792,2338 },
  //  {1050,1698,756,1206,1794,2340 },
  //  {1308,1700,760,1208,2064,2340 },
  //  {1308,1497,759,1209,2064,2343 },
  //  {1308,1498,824,946,2064,2342 },
  //  {1298,1412,858,946,2064,2342 },
  //  {1298,1413,858,946,2207,2307 },
  //  {-1, -1, -1, -1, -1, -1},
  //};
  //
  //if (VOIs[iTest][0] >= 0)
  //{
  //  OnViewROIVolume();

  //  iTest++;

  //  memcpy(m_VOI, VOIs[iTest], sizeof(VOI_TYPE));
  //  UpdateGizmo();        
  //}
  //else iTest = 0;

  //return;

#if 1
  //BES: 25.6.2008 - changed according to Fulvia's comments
  //the VME now creates a new small VME with the selected ROI
  //in the quality that is currently displayed
  //N.B. this operation may fail, if there is not enough memory!
  if (m_LargeDataReader == NULL || 
    m_LargeDataReader->GetOutputDataSet() == NULL)
  {
    wxMessageBox(wxString::Format(
      _("This operation is invalid in the current context.\nError: %s [%d]"), 
      __FILE__, __LINE__));

    return;	//invalid state
  }

  wxBusyInfo wait(_("Cropping the volume ..."));

  vtkImageData* pCopy = NULL;
  vtkRectilinearGrid* pCopyRLG = NULL;

  bool bRLG = m_LargeDataReader->IsRectilinearGrid();
  try
  {
    if (!bRLG)
    {
      vtkImageData* pData = m_LargeDataReader->GetOutputDataSet();  

      pCopy = vtkImageData::New();    
      pCopy->DeepCopy(pData);
    }
    else
    {
      //rectilinear grid
      vtkRectilinearGrid* pData = m_LargeDataReader->GetOutputRLGDataSet();  

      pCopyRLG = vtkRectilinearGrid::New();
      pCopyRLG->DeepCopy(pData);
    }
  }
  catch (...)
  {
    vtkDEL(pCopy);
    vtkDEL(pCopyRLG);
  	
    wxMessageBox(_("ERROR: Not enough memory to complete the operation."));
    return;	//invalid state
  }
  
  mafVMEVolumeGray* newVME;
  mafNEW(newVME);
  if (bRLG)
    newVME->SetData(pCopyRLG, 0, MAF_VME_REFERENCE_DATA);
  else
    newVME->SetData(pCopy, 0, MAF_VME_REFERENCE_DATA);
  vtkDEL(pCopy);
  vtkDEL(pCopyRLG);

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("NATURAL");

  wxString name = this->GetName();
  newVME->SetName(wxString::Format("%s (OR[%d,%d,%d], SR[%d])", name, 
    m_VOI[0], m_VOI[2], m_VOI[4], m_LargeDataReader->GetSampleRate()));
  newVME->GetTagArray()->SetTag(tag_Nature);
    
  mafEvent ev(this, VME_ADD, newVME);
  this->ForwardUpEvent(&ev);

  mafDEL(newVME);   //VME_ADD increased reference
#else
  //This code saves the highest resolution of the selected ROI
  //into the output ROI

#ifdef VME_VOLUME_VER1
	if (m_LargeData == NULL || 
		m_LargeData->GetPointDataProvider() == NULL ||
		m_LargeData->GetPointDataProvider()->GetScalarsDescriptor() == NULL
		) 
#else
	if (m_LargeDataReader == NULL || 
		m_LargeDataReader->GetLevelFile(1) == NULL)
#endif
	{
		wxMessageBox(wxString::Format(
			_("This operation is invalid in the current context.\nError: %s [%d]"), 
			__FILE__, __LINE__));

		return;	//invalid state
	}	

	std::string fname = mafGetSaveFile(mafGetApplicationDirectory().c_str(), 
		_("RAW files (*.raw)|*.raw"), _("Save Volume File"));

	if (fname.c_str() == NULL || *fname.c_str() == '\0')
		return;	//cancel pressed
	
	wxBusyInfo wait(_("Cropping the volume ..."));

	vtkSmartPointer<vtkMAFFileDataProvider> dst = vtkMAFFileDataProvider::New();
	dst->Delete();	//vtkSmartPointer increased reference counter

	wxString szFName = fname.c_str();
	wxStripExtension(szFName);
	szFName += wxString::Format("_%d_%d_%d.raw", 
		(m_VOI[1] - m_VOI[0] + 1), 
		(m_VOI[3] - m_VOI[2] + 1),
		(m_VOI[5] - m_VOI[4] + 1));

	if (!dst->OpenFile(szFName, false))
		wxMessageBox(_("File could not be created."));
	else
	{
#ifdef VME_VOLUME_VER1
		vtkMAFLargeDataProvider* src = m_LargeData->GetPointDataProvider();	
		vtkMAFDataArrayDescriptor* srcDsc = src->GetScalarsDescriptor();	
#else
		int OrigVOI[6];
		mafBrickedFileReader* srcB = m_LargeDataReader->GetLevelFile(1);
		srcB->GetVOI(OrigVOI);
#endif // VME_VOLUME_VER1
	

		vtkSmartPointer<vtkMAFDataArrayDescriptor> dstDsc = vtkMAFDataArrayDescriptor::New();		
		dstDsc->Delete();	//vtkSmartPointer increased reference counter
#ifdef VME_VOLUME_VER1
		dstDsc->SetDataType(srcDsc->GetDataType());
		dstDsc->SetNumberOfComponents(srcDsc->GetNumberOfComponents());
#else
		dstDsc->SetDataType(srcB->GetDataType());
		dstDsc->SetNumberOfComponents(srcB->GetNumberOfComponents());
#endif // VME_VOLUME_VER1

		//compute new number of tuples
		vtkIdType64 numTup = ((vtkIdType64)(m_VOI[1] - m_VOI[0] + 1))* 
			(m_VOI[3] - m_VOI[2] + 1)*(m_VOI[5] - m_VOI[4] + 1);
		dstDsc->SetNumberOfTuples(numTup);
		
#ifdef VME_VOLUME_VER1
		dst->SetSwapBytes(src->GetSwapBytes());
#else
		//TODO: unsupported, yet
#endif // VME_VOLUME_VER1
		dst->SetScalarsDescriptor(dstDsc.GetPointer());

		vtkSmartPointer<vtkDataArray> buffer = 
			vtkDataArray::CreateDataArray(dstDsc->GetDataType());
		buffer->Delete();	//NB: vtkSmartPointer increases ReferenceCount);
						
		mafEvent ev(this, PROGRESSBAR_SHOW, this);
		this->ForwardUpEvent(&ev);

		ev.SetId(PROGRESSBAR_SET_VALUE);	
		
		vtkIdType64 index = 0;
		int pixelRead = m_VOI[1] - m_VOI[0] + 1;
#ifdef VME_VOLUME_VER1
		for (int zz = m_VOI[4]; zz <= m_VOI[5]; zz++)
		{
			ev.SetArg(100*(zz - m_VOI[4]) / (m_VOI[5] - m_VOI[4] + 1));
			this->ForwardUpEvent(&ev);

			for (int yy = m_VOI[2]; yy <= m_VOI[3]; yy++)
			{
				vtkIdType64 idxSeek	= ((vtkMAFLargeImageData*)m_LargeData)
					->GetDataIndex(m_VOI[0], yy, zz) / srcDsc->GetNumberOfComponents();
					
				src->GetScalars(NULL, buffer.GetPointer(), idxSeek, pixelRead);
				if (pixelRead != buffer->GetNumberOfTuples())
				{
					wxMessageBox(_("Error while reading source volume file."));
					zz = m_VOI[5];	//terminates the outer loop
					break;
				}
				
				dst->SetScalars(NULL, buffer.GetPointer(), index);
				index += pixelRead;				
			}
		}
#else
		//break the VOI into smaller chunk we are able to process
		int nSliceInVoxels = (m_VOI[1] - m_VOI[0] + 1)*(m_VOI[3] - m_VOI[2] + 1);
		int nSliceInB = nSliceInVoxels*dstDsc->GetNumberOfComponents()*
			dstDsc->GetDataTypeSize();

		int nSlicesLim = m_SampleMemLimit*1024*1024 / nSliceInB;
		if (nSlicesLim == 0)
			nSlicesLim = 1;

		//nSlicesLim is the maximal number of slices we can process at once
		int VOI[6];
		memcpy(VOI, m_VOI, sizeof(int)*6);

		mafObserver* pOldListener = srcB->GetListener();
		srcB->SetListener(NULL);

		int nCurPass = 0, nPasses = ((m_VOI[5] - VOI[4] + nSlicesLim) / nSlicesLim) + 1;			
		for (VOI[4] = m_VOI[4]; VOI[4] <= m_VOI[5]; VOI[4] = VOI[5] + 1)
		{
			ev.SetId(PROGRESSBAR_SET_VALUE);
			ev.SetArg(100*(VOI[4] - m_VOI[4]) / (m_VOI[5] - m_VOI[4] + 1));
			this->ForwardUpEvent(&ev);

			 if (m_VOI[5] - VOI[4] + 1 > nSlicesLim)
				 VOI[5] = VOI[4] + nSlicesLim - 1;
			 else
				 VOI[5] = m_VOI[5];

			 mafString szMsg = wxString::Format(
				 _("Reading data (pass %d / %d)..."), ++nCurPass, nPasses);
			 ev.SetId(PROGRESSBAR_SET_TEXT);
			 ev.SetString(&szMsg);
			 this->ForwardUpEvent(&ev);

			 srcB->SetVOI(VOI);			 
			 srcB->Update();
			
			 szMsg = wxString::Format(
				 _("Writing data (pass %d / %d)..."), nCurPass, nPasses);			 
			 ev.SetString(&szMsg);
			 this->ForwardUpEvent(&ev);
			
			 dst->SetScalars(NULL, 
				 srcB->GetOutputDataSet()->GetPointData()->GetScalars(), index);
			 index += ((vtkIdType64)(VOI[5] - VOI[4] + 1))*nSliceInVoxels;
		}


		srcB->SetListener(pOldListener);
#endif // VME_VOLUME_VER1

		dst->CloseFile();

		ev.SetId(PROGRESSBAR_HIDE);
		this->ForwardUpEvent(&ev);
	}
#endif
}
#endif //VME_VOLUME_LARGE_EXCLUDE_CROP

#pragma region LOADING / STORING
//two methods for restoring/storing the content of large data set from MSF  
/*virtual*/ int mafVMEVolumeLarge::InternalStore(mafStorageElement *parent)
{	
#ifdef VME_VOLUME_VER1
	vtkMAFLargeImageData* ds = vtkMAFLargeImageData::SafeDownCast(m_LargeData);	
	if (ds == NULL)
	{
		assert(false);
		return MAF_ERROR;
	}
#else
	if (m_LargeDataReader == NULL)
	{
		assert(false);
		return MAF_ERROR;
	}
#endif

	int ret_val = Superclass::InternalStore(parent);	//stores filename
	if (ret_val != MAF_OK)
		return ret_val;


	mafStorageElement* ds_info = parent->AppendChild("LargeDataSetInfo");

#ifdef VME_VOLUME_VER1
	int AutoSampleRate = (int)ds->GetAutoSampleRate();
	int DataLowerLeft = (int)ds->GetDataLowerLeft();
	int DataMask = ds->GetDataMask();
	int MemoryLimit = ds->GetMemoryLimit();
	int ScalarType = ds->GetScalarType();

	int dims[3], samp[3], wext[6]; 		
	ds->GetDimensions(dims);
	ds->GetSampleRate(samp);
	ds->GetWholeExtent(wext);

	double origin[3], sp[3];
	ds->GetOrigin(origin);
	ds->GetSpacing(sp);

	ds_info->StoreText(_("ClassName"), ds->GetClassName());
	ds_info->StoreInteger(_("AutoSampleRate"), AutoSampleRate);
	ds_info->StoreInteger(_("DataLowerLeft"), DataLowerLeft);
	ds_info->StoreInteger(_("DataMask"), DataMask);
	ds_info->StoreInteger(_("MemoryLimit"), MemoryLimit);
	ds_info->StoreInteger(_("ScalarType"), ScalarType);
	ds_info->StoreInteger(_("DimX"), dims[0]);
	ds_info->StoreInteger(_("DimY"), dims[1]);
	ds_info->StoreInteger(_("DimZ"), dims[2]);
	ds_info->StoreInteger(_("SampX"), samp[0]);
	ds_info->StoreInteger(_("SampY"), samp[1]);
	ds_info->StoreInteger(_("SampZ"), samp[2]);	
	ds_info->StoreInteger(_("WExtXMin"), wext[0]);
	ds_info->StoreInteger(_("WExtXMax"), wext[1]);
	ds_info->StoreInteger(_("WExtYMin"), wext[2]);	
	ds_info->StoreInteger(_("WExtYMax"), wext[3]);
	ds_info->StoreInteger(_("WExtZMin"), wext[4]);
	ds_info->StoreInteger(_("WExtZMax"), wext[5]);	

	ds_info->StoreDouble(_("OrigX"), origin[0]);
	ds_info->StoreDouble(_("OrigX"), origin[1]);
	ds_info->StoreDouble(_("OrigX"), origin[2]);
	ds_info->StoreDouble(_("SpX"), sp[0]);
	ds_info->StoreDouble(_("SpY"), sp[1]);
	ds_info->StoreDouble(_("SpZ"), sp[2]);
#else
	int MemoryLimit = m_SampleMemLimit*1024;
	ds_info->StoreText(_("ClassName"), m_LargeDataReader->GetStaticTypeName());
	ds_info->StoreInteger(_("MemoryLimit"), MemoryLimit);
#endif

	ds_info->StoreInteger(_("FE0"), m_FullExtent[0]);
	ds_info->StoreInteger(_("FE1"), m_FullExtent[1]);
	ds_info->StoreInteger(_("FE2"), m_FullExtent[2]);
	ds_info->StoreInteger(_("FE3"), m_FullExtent[3]);
	ds_info->StoreInteger(_("FE4"), m_FullExtent[4]);
	ds_info->StoreInteger(_("FE5"), m_FullExtent[5]);

#ifndef VME_VOLUME_VER1
	ds_info->StoreInteger(_("V0"), m_VOI[0]);
	ds_info->StoreInteger(_("V1"), m_VOI[1]);
	ds_info->StoreInteger(_("V2"), m_VOI[2]);
	ds_info->StoreInteger(_("V3"), m_VOI[3]);
	ds_info->StoreInteger(_("V4"), m_VOI[4]);
	ds_info->StoreInteger(_("V5"), m_VOI[5]);	

	ds_info->StoreText(_("FileName"), m_LargeDataReader->GetFileName());
#else
	int VOI[6];
	ds->GetVOI(VOI);
	ds_info->StoreInteger(_("V0"), VOI[0]);
	ds_info->StoreInteger(_("V1"), VOI[1]);
	ds_info->StoreInteger(_("V2"), VOI[2]);
	ds_info->StoreInteger(_("V3"), VOI[3]);
	ds_info->StoreInteger(_("V4"), VOI[4]);
	ds_info->StoreInteger(_("V5"), VOI[5]);		

	vtkMAFLargeDataProvider* pp = ds->GetPointDataProvider();
	assert(pp != NULL);

	if (pp != NULL)
	{
		ds_info = ds_info->AppendChild("PointProviderInfo");
		ds_info->StoreText(_("ClassName"), pp->GetClassName());

		vtkMAFFileDataProvider* filepp = vtkMAFFileDataProvider::SafeDownCast(pp);
		if (filepp != NULL)
		{
			ds_info->StoreText(_("FileName"), filepp->GetFileName());
		}

		vtkIdType64 hs = pp->GetHeaderSize();
		int num_desc = pp->GetNumberOfDescriptors();
		int swap = (int)pp->GetSwapBytes();

		int hs_part = (int)(hs & 0xFFFFFFFF);
		ds_info->StoreInteger(_("HdrSz_Lo"), hs_part);
		hs_part = (int)(hs >> 32);
		ds_info->StoreInteger(_("HdrSz_Hi"), hs_part);

		ds_info->StoreInteger(_("SwapBytes"), swap);
		ds_info->StoreInteger(_("NumOfDescs"), num_desc);

		for (int i = 0; i < num_desc; i++)
		{
			vtkMAFDataArrayDescriptor* desc = pp->GetDescriptor(i);

			mafStorageElement* ds_di  = ds_info->AppendChild(
				wxString::Format(_("DSC_%d"), i));

			int DataType = desc->GetDataType();
			const char* Name = desc->GetName();
			int NumberOfComponents = desc->GetNumberOfComponents();
			vtkIdType64 NumberOfTuples = desc->GetNumberOfTuples();

			ds_di->StoreText(_("Name"), Name == NULL ? _("(NULL)"): Name);
			ds_di->StoreInteger(_("DType"), DataType);
			ds_di->StoreInteger(_("NumOfComps"), NumberOfComponents);

			int tup = (int)(NumberOfTuples & 0xFFFFFFFF);
			ds_di->StoreInteger(_("NumOfTuples_Lo"), tup);
			tup = (int)(NumberOfTuples >> 32);
			ds_di->StoreInteger(_("NumOfTuples_Hi"), tup);
		}

		//store indices of special descriptors
		int idx = pp->GetIndexOfScalarsDescriptor();
		ds_info->StoreInteger("IdxScalars", idx);
		pp->GetIndexOfVectorsDescriptor();
		ds_info->StoreInteger("IdxVectors", idx);
		pp->GetIndexOfNormalsDescriptor();
		ds_info->StoreInteger("IdxNormals", idx);
		pp->GetIndexOfTCoordsDescriptor();
		ds_info->StoreInteger("IdxTCoords", idx);
		pp->GetIndexOfTensorsDescriptor();
		ds_info->StoreInteger("IdxTensors", idx);
	}
#endif
	return ret_val;
}

/*virtual*/ int mafVMEVolumeLarge::InternalRestore(mafStorageElement *node)
{
	int ret_val = Superclass::InternalRestore(node);	//restores filename
	if (ret_val != MAF_OK)
		return ret_val;	

	//Restore m_LargeData
	mafStorageElement* ds_info = node->FindNestedElement("LargeDataSetInfo");
	if (ds_info != NULL)
	{
		//Oops. It was not stored.
		mafString clsname;
		ds_info->RestoreText(_("ClassName"), clsname);
#ifdef VME_VOLUME_VER1
		assert(m_LargeData == NULL);
		if (clsname.Compare(_("vtkMAFLargeImageData")) != 0) 
#else
		assert(m_LargeDataReader == NULL);
		if (clsname.Compare(_("mafVolumeLargeReader")) != 0) 		
#endif
		{
			wxMessageBox(wxString::Format(_("Class '%s' is unsupported in the current release."),
				clsname.GetCStr()), _("Error while restoring Large Data"), 
				wxOK | wxCENTER | wxICON_EXCLAMATION);

			return ret_val;
		}

#ifndef VME_VOLUME_VER1
		m_LargeDataReader = new mafVolumeLargeReader();
		m_LargeDataReader->SetListener(this);
#else
		m_LargeData = vtkMAFLargeImageData::New();
		m_LargeData->AddObserver(vtkCommand::ProgressEvent, m_ProgressCallback);
		m_LargeData->AddObserver(vtkCommand::StartEvent, m_ProgressCallback);
		m_LargeData->AddObserver(vtkCommand::EndEvent, m_ProgressCallback);		

		vtkMAFLargeImageData* ds = vtkMAFLargeImageData::SafeDownCast(m_LargeData);	
		assert(ds != NULL);

		if (ds != NULL)
		{
			//restore vtkMAFLargeImageData settings
			int AutoSampleRate, DataLowerLeft, DataMask, ScalarType;
			int dims[3], samp[3], wext[6]; 		
			double origin[3], sp[3];

			ds_info->RestoreInteger(_("AutoSampleRate"), AutoSampleRate);
			ds_info->RestoreInteger(_("DataLowerLeft"), DataLowerLeft);
			ds_info->RestoreInteger(_("DataMask"), DataMask);
#endif

			int MemoryLimit;
			ds_info->RestoreInteger(_("MemoryLimit"), MemoryLimit);
#ifdef VME_VOLUME_VER1
			ds_info->RestoreInteger(_("ScalarType"), ScalarType);
			ds_info->RestoreInteger(_("DimX"), dims[0]);
			ds_info->RestoreInteger(_("DimY"), dims[1]);
			ds_info->RestoreInteger(_("DimZ"), dims[2]);
			ds_info->RestoreInteger(_("SampX"), samp[0]);
			ds_info->RestoreInteger(_("SampY"), samp[1]);
			ds_info->RestoreInteger(_("SampZ"), samp[2]);	
			ds_info->RestoreInteger(_("WExtXMin"), wext[0]);
			ds_info->RestoreInteger(_("WExtXMax"), wext[1]);
			ds_info->RestoreInteger(_("WExtYMin"), wext[2]);	
			ds_info->RestoreInteger(_("WExtYMax"), wext[3]);
			ds_info->RestoreInteger(_("WExtZMin"), wext[4]);
			ds_info->RestoreInteger(_("WExtZMax"), wext[5]);	

			ds_info->RestoreDouble(_("OrigX"), origin[0]);
			ds_info->RestoreDouble(_("OrigX"), origin[1]);
			ds_info->RestoreDouble(_("OrigX"), origin[2]);
			ds_info->RestoreDouble(_("SpX"), sp[0]);
			ds_info->RestoreDouble(_("SpY"), sp[1]);
			ds_info->RestoreDouble(_("SpZ"), sp[2]);
#endif

			ds_info->RestoreInteger(_("FE0"), m_FullExtent[0]);
			ds_info->RestoreInteger(_("FE1"), m_FullExtent[1]);
			ds_info->RestoreInteger(_("FE2"), m_FullExtent[2]);
			ds_info->RestoreInteger(_("FE3"), m_FullExtent[3]);
			ds_info->RestoreInteger(_("FE4"), m_FullExtent[4]);
			ds_info->RestoreInteger(_("FE5"), m_FullExtent[5]);

			ds_info->RestoreInteger(_("V0"), m_VOI[0]);
			ds_info->RestoreInteger(_("V1"), m_VOI[1]);
			ds_info->RestoreInteger(_("V2"), m_VOI[2]);
			ds_info->RestoreInteger(_("V3"), m_VOI[3]);
			ds_info->RestoreInteger(_("V4"), m_VOI[4]);
			ds_info->RestoreInteger(_("V5"), m_VOI[5]);

#ifdef VME_VOLUME_VER1		
			ds->SetVOI(m_VOI);			
			ds->SetAutoSampleRate(AutoSampleRate != 0);
			ds->SetDataLowerLeft(DataLowerLeft != 0);
			ds->SetDataMask(DataMask);
			ds->SetMemoryLimit(MemoryLimit);
			ds->SetScalarType(ScalarType);
			ds->SetOrigin(origin);
			ds->SetSpacing(sp);
			ds->SetDimensions(dims);
			ds->SetSampleRate(samp);
			ds->SetWholeExtent(wext);
#else
			m_SampleMemLimit = MemoryLimit / 1024;

			mafString szStr;
			ds_info->RestoreText(_("FileName"), szStr);			
			m_LargeDataReader->SetFileName(szStr);
			
//			int emptyVOI[6];
//			memset(emptyVOI, 0, sizeof(int)*6);
			m_LargeDataReader->SetVOI(m_VOI);
			m_LargeDataReader->Update();	//force the connection to files
#endif

			//now we can transform our extent
			TransformExtent(m_VOI, m_ROI);

#ifdef VME_VOLUME_VER1
			//now restore point provider
			ds_info = ds_info->FindNestedElement("PointProviderInfo");
			if (ds_info != NULL)
			{
				ds_info->RestoreText(_("ClassName"), clsname);
				if (m_LargeData->GetPointDataProvider() == NULL)
				{
					//TODO: create the proper object

					assert(clsname.Compare(_("vtkMAFFileDataProvider")) == 0 ||
						clsname.Compare(_("vtkMemMapFileDataProvider")) == 0);
#if defined(_WIN32) && defined(_USE_MEMMAPFILE_DATAPROVIDER)
					m_LargeData->SetPointDataProvider(vtkMemMapFileDataProvider::New());
#else
					m_LargeData->SetPointDataProvider(vtkMAFFileDataProvider::New());
#endif
					m_LargeData->GetPointDataProvider()->Delete();	//SetPointDataProvider increases Reference counter
				}

				mafString szStr;

				vtkMAFLargeDataProvider* pp = m_LargeData->GetPointDataProvider();
				vtkMAFFileDataProvider* filepp = vtkMAFFileDataProvider::SafeDownCast(pp);
				if (filepp != NULL)
				{
					ds_info->RestoreText(_("FileName"), szStr);
					int res = filepp->OpenFile(szStr);
					assert(res != 0);
				}

				int hs_lo, hs_hi, num_desc, swap;
				ds_info->RestoreInteger(_("HdrSz_Lo"), hs_lo);
				ds_info->RestoreInteger(_("HdrSz_Hi"), hs_hi);
				ds_info->RestoreInteger(_("SwapBytes"), swap);
				ds_info->RestoreInteger(_("NumOfDescs"), num_desc);

				pp->SetHeaderSize(((vtkIdType64)hs_hi << 32) | (unsigned int)hs_lo);
				pp->SetSwapBytes(swap != 0);

				//restore descriptors of the provider
				for (int i = 0; i < num_desc; i++)
				{
					vtkMAFDataArrayDescriptor* desc = vtkMAFDataArrayDescriptor::New();

					mafStorageElement* ds_di  = ds_info->FindNestedElement(wxString::Format(_("DSC_%d"), i));
					assert(ds_di != NULL);

					int DataType, NumberOfComponents, NumTup_lo, NumTup_hi;
					ds_di->RestoreText(_("Name"), szStr); 
					ds_di->RestoreInteger(_("DType"), DataType);
					ds_di->RestoreInteger(_("NumOfComps"), NumberOfComponents);
					ds_di->RestoreInteger(_("NumOfTuples_Lo"), NumTup_lo);
					ds_di->RestoreInteger(_("NumOfTuples_Hi"), NumTup_hi);

					if (szStr.Compare( _("(NULL)")) != 0)
						desc->SetName(szStr);

					desc->SetDataType(DataType);
					desc->SetNumberOfComponents(NumberOfComponents);
					desc->SetNumberOfTuples(((vtkIdType64)NumTup_hi << 32) | 
						(unsigned int)NumTup_lo);

					pp->AddDescriptor(desc);
					desc->Delete();	//desc is no longer needed, AddDescriptor increased RefCount
				}

				int idx;
				ds_info->RestoreInteger("IdxScalars", idx);
				pp->SetIndexOfScalarsDescriptor(idx);				
				ds_info->RestoreInteger("IdxVectors", idx);
				pp->SetIndexOfVectorsDescriptor(idx);				
				ds_info->RestoreInteger("IdxNormals", idx);
				pp->SetIndexOfNormalsDescriptor(idx);				
				ds_info->RestoreInteger("IdxTCoords", idx);
				pp->SetIndexOfTCoordsDescriptor(idx);				
				ds_info->RestoreInteger("IdxTensors", idx);
				pp->SetIndexOfTensorsDescriptor(idx);

				//TODO: CellPointProvider
			}

			//snapshot is in DataVector and should be loaded on demand => no need
			//for internal snapshot
			m_LargeData->SetSnapshot(NULL);
			m_ShowFEOutput = true;
		} //ds != NULL
#endif
	}

	if (m_Gui)
		m_Gui->Update();

	return ret_val;
}

#pragma endregion


//this method stores information about the large data set
//it calls SetData to set it to the Snapshot of the give data
//Note: SetData is still provided
#ifdef VME_VOLUME_VER1
/*virtual*/ int mafVMEVolumeLarge::SetLargeData(vtkMAFLargeDataSet *data, mafTimeStamp t, int mode)
{
	assert(data);
	assert(mode != MAF_VME_COPY_DATA);		//it would be too slow

	if (data->IsA("vtkMAFLargeImageData"))
	{		
		if (m_LargeData != NULL) {
			m_LargeData->RemoveObserver(m_ProgressCallback);
			vtkDEL(m_LargeData);
		}

		m_LargeData = data;
		m_LargeData->Register(NULL);
		m_LargeData->AddObserver(vtkCommand::ProgressEvent, m_ProgressCallback);
		m_LargeData->AddObserver(vtkCommand::StartEvent, m_ProgressCallback);
		m_LargeData->AddObserver(vtkCommand::EndEvent, m_ProgressCallback);
		m_LargeData->Update();	//make sure we have the current data
		m_ShowFEOutput = true;

		if (mode == MAF_VME_DETACH_DATA)
			m_LargeData->SetSource(NULL);		

		m_LargeData->GetVOI(m_FullExtent);
		memcpy(m_VOI, m_FullExtent, sizeof(m_FullExtent));
		TransformExtent(m_VOI, m_ROI);
				
		return Superclass::SetData(m_LargeData->GetSnapshot(), t, MAF_VME_REFERENCE_DATA);
	}

	mafErrorMacro("Trying to set the wrong type of data inside a "<<
		GetTypeName()<<" :"<< (data?data->GetClassName():"NULL"));
	return MAF_ERROR;
}
#else
/*virtual*/ int mafVMEVolumeLarge::SetLargeData(mafVolumeLargeReader *data)
{
	assert(data);
  if (data == NULL)
    return MAF_ERROR;
	
	data->Update();	//make sure we have the current data
	int ret = Superclass::SetData((data->IsRectilinearGrid() ? 
    (vtkDataSet*)data->GetOutputRLGDataSet() : (vtkDataSet*)data->GetOutputDataSet()),
    0, MAF_VME_REFERENCE_DATA);
	if (ret != MAF_OK)
		return ret;

	cppDEL(m_LargeDataReader);
	m_LargeDataReader = data;
	
	m_LargeDataReader->GetVOI(m_FullExtent);
	memcpy(m_VOI, m_FullExtent, sizeof(m_FullExtent));
	TransformExtent(m_VOI, m_ROI);	

	m_SampleMemLimit = m_LargeDataReader->GetMemoryLimit() / 1024;
	return MAF_OK;
}
#endif

//////////////////////////////////////////////////////////////////////////
#pragma region Getters / Setters

//Set the pathname of the large data file (that is used)
void mafVMEVolumeLarge::SetFileName(const char *filename)
{
	mafTagItem item;

	item.SetName("EXTDATA_FILENAME");
	item.SetValue(filename);
	this->GetTagArray()->SetTag(item);
}

//Get the pathname of the large data file (that is used)
const char* mafVMEVolumeLarge::GetFileName()
{
	mafTagItem *item=this->GetTagArray()->GetTag("EXTDATA_FILENAME");
  if (item != NULL)
	  return item->GetValue();
  else
  {
    assert(false);
    return "";
  }
}

#pragma endregion

#pragma region MISC
#ifdef VME_VOLUME_VER1
//this routine creates snapshot object for m_LargeDataSet
//from the current DataVector item
void mafVMEVolumeLarge::CreateSnapshotObject()
{
	assert(m_LargeData != NULL);
	assert(m_DataVector != NULL);

	//force the loading of stored data
	mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(m_DataVector->GetItem(0));
	assert(item != NULL);

	if (item != NULL)
		m_LargeData->SetSnapshot(item->GetData());	
	else if (m_LargeData->GetSnapshot() == NULL)
		m_LargeData->SetSnapshot(vtkImageData::New());
}


//if m_FullExtent sampled version does not exist and
//the current snapshot contains it, it will be stored
void mafVMEVolumeLarge::CheckFEOutput()
{
	//get current VOI
	int VOI[6];
	m_LargeData->GetVOI(VOI);

	//set a new data	
	if (m_FEOutput == NULL)
	{
		//check if the current output is our full volume				
		bool bOutputIsFE = true;
		for (int i = 0; i < 6; i++) 
		{
			if (VOI[i] != m_FullExtent[i]) {
				bOutputIsFE = false; break;
			}
		}		

		if (bOutputIsFE)
		{				
			m_LargeData->GetSampleRate(m_FESampleRate);
			m_FEOutput = m_LargeData->GetSnapshot();						
			m_FEOutput->Register(NULL);

			vtkDataSet* newSnapshot = m_FEOutput->NewInstance();
			m_LargeData->SetSnapshot(newSnapshot);
			newSnapshot->Delete();	//SetSnapshot addrefed it

			SetData(newSnapshot, 0, MAF_VME_REFERENCE_DATA);
		}
	}
}
#endif // VME_VOLUME_VER1

/*
//Updates the output of data pipeline
//Supposed to be called after VME generates a new data
void mafVMEVolumeLarge::UpdateOutput()
{
	//BES: 1.2.2008 - trick to reverse propagate UpdateExtent
	//GetOutput()->Update() otherwise propagates old UpdateExtent
	//from output to input (VME), thus the subsampled data 
	//is shown incorrectly
	int wext[6];
	vtkDataSet* ds = GetOutput()->GetVTKData();
#ifdef VME_VOLUME_VER1
	if (m_ShowFEOutput)
		m_FEOutput->GetWholeExtent(wext);
	else
		m_LargeData->GetSnapshot()->GetWholeExtent(wext);
#else
	m_LargeDataReader->GetOutputDataSet()->GetWholeExtent(wext);
#endif // VME_VOLUME_VER1
	ds->SetWholeExtent(wext);
	ds->SetUpdateExtentToWholeExtent();
	//end BES	

	GetOutput()->Update();

	//BES: this resets the currently opened view 
	//unfortunately, MAF logic does not update all views
	mafEvent ev(this, ds->GetNumberOfConsumers() == 0 ?
			VME_SELECTED : VME_VISUAL_MODE_CHANGED,this);
	this->ForwardUpEvent(&ev);		
}
*/


//transforms the extent given in units into extent in mm
void mafVMEVolumeLarge::TransformExtent(int extUn[6], double outMm[6])
{
#ifdef VME_VOLUME_VER1
	vtkMAFLargeImageData* ds = vtkMAFLargeImageData::SafeDownCast(m_LargeData);
#else
	mafBrickedFileReader* ds = (m_LargeDataReader != NULL) ? 
		m_LargeDataReader->GetLevelFile(1) : NULL;
#endif
	assert(ds != NULL);

	if (ds == NULL) {
		for (int i = 0; i < 6; i++){
			outMm[i] = 0.0;
		}
	}
	else
	{		
		double orig[3], sp[3];

#ifdef VME_VOLUME_VER1
    int wext[6];
		ds->GetWholeExtent(wext);
		ds->GetOrigin(orig);
		ds->GetSpacing(sp);

    outMm[0] = orig[0] + (extUn[0] - wext[0])*sp[0];
    outMm[1] = orig[0] + (extUn[1] - wext[0])*sp[0];
    outMm[2] = orig[1] + (extUn[2] - wext[2])*sp[1];
    outMm[3] = orig[1] + (extUn[3] - wext[2])*sp[1];
    outMm[4] = orig[2] + (extUn[4] - wext[4])*sp[2];
    outMm[5] = orig[2] + (extUn[5] - wext[4])*sp[2];
#else
    if (!ds->IsRectilinearGrid())
    {
      ds->GetDataOrigin(orig);
		  ds->GetDataSpacing(sp);

      for (int i = 0, j = 0; i < 3; i++, j += 2)
      {
        outMm[j] = orig[i] + extUn[j]*sp[i];
        outMm[j+1] = orig[i] + extUn[j+1]*sp[i];
      }		  
    }
    else
    {
      //rectilinear grid => it will be more interesting
      double* pXYZ = ds->GetXCoordinates()->GetPointer(0);
      outMm[0] = pXYZ[extUn[0]]; outMm[1] = pXYZ[extUn[1]];

      pXYZ = ds->GetYCoordinates()->GetPointer(0);
      outMm[2] = pXYZ[extUn[2]]; outMm[3] = pXYZ[extUn[3]];

      pXYZ = ds->GetZCoordinates()->GetPointer(0);
      outMm[4] = pXYZ[extUn[4]]; outMm[5] = pXYZ[extUn[5]];
    }
#endif //VME_VOLUME_VER1
	}
}

//transforms the extent given in mm into extent in units
void mafVMEVolumeLarge::InverseTransformExtent(double extMm[6], int outUn[6])
{
#ifdef VME_VOLUME_VER1
	vtkMAFLargeImageData* ds = vtkMAFLargeImageData::SafeDownCast(m_LargeData);
#else
	mafBrickedFileReader* ds = (m_LargeDataReader != NULL) ? 
		m_LargeDataReader->GetLevelFile(1) : NULL;
#endif // VME_VOLUME_VER1
	assert(ds != NULL);

	if (ds == NULL) {
		for (int i = 0; i < 6; i++){
			outUn[i] = 0;
		}
	}
	else
	{		
		double orig[3], sp[3];

#ifdef VME_VOLUME_VER1
    int wext[6];
		ds->GetWholeExtent(wext);
		ds->GetOrigin(orig);
		ds->GetSpacing(sp);

    outUn[0] = wext[0] + (extMm[0] - orig[0]) / sp[0];
    outUn[1] = wext[0] + (extMm[1] - orig[0]) / sp[0];
    outUn[2] = wext[2] + (extMm[2] - orig[1]) / sp[1];
    outUn[3] = wext[2] + (extMm[3] - orig[1]) / sp[1];
    outUn[4] = wext[4] + (extMm[4] - orig[2]) / sp[2];
    outUn[5] = wext[4] + (extMm[5] - orig[2]) / sp[2];
#else		
    if (!ds->IsRectilinearGrid())
    {
		  ds->GetDataOrigin(orig);
		  ds->GetDataSpacing(sp);

      for (int i = 0, j = 0; i < 3; i++, j += 2)
      {
        outUn[j] = (extMm[j] - orig[i]) / sp[i];
        outUn[j+1] = (extMm[j+1] - orig[i]) / sp[i];        
      }	
    }
    else
    {
      //rectilinear grid => it will be more interesting
      vtkDoubleArray* pXYZCoords[3];
      pXYZCoords[0] = ds->GetXCoordinates();
      pXYZCoords[1] = ds->GetYCoordinates();
      pXYZCoords[2] = ds->GetZCoordinates();
      for (int i = 0; i < 3; i++)
      {         
        double* pXYZ = pXYZCoords[i]->GetPointer(0);
        for (int j = 0; j < 2; j++)
        {
          int a = 0, b = pXYZCoords[i]->GetMaxId();
          bool bAsc = pXYZ[a] < pXYZ[b];
          
          //binary search
          while (a < b)
          {
            int t = (a + b) / 2;
            if (extMm[2*i + j] == pXYZ[t]) {
              a = t; break;
            }
            else if (extMm[2*i + j] < pXYZ[t] == bAsc)            
              b = t - 1;  //the value is in the area <a..t>            
            else
              a = t + 1;  //the value is in the area <t..b>            
          }

          outUn[2*i + j] = a; //the nearest pos
        }
      }     
    }
#endif // VME_VOLUME_VER1
	}
}

#pragma endregion

#pragma region GUI
//-------------------------------------------------------------------------
/*virtual*/ mafGUI* mafVMEVolumeLarge::CreateGui()
//-------------------------------------------------------------------------
{	
	assert(m_Gui == NULL);
	m_Gui = Superclass::CreateGui();	// Called to show info about vmes' type and name
	m_Gui->SetListener(this);			// Messages will be intercept by this VME
	m_Gui->Divider(3);	//3 should be for sunken, no constants defined

	m_InfoGui = CreateInfoGui();
	m_Gui->RollOut(ID_ROLLOUT_INFO, _("Original data info"), m_InfoGui, false);

	m_SampleInfoGui = CreateSampleInfoGui();
	m_Gui->RollOut(ID_ROLLOUT_SAMPLE_INFO, _("Sample data info"), m_SampleInfoGui, false);

	m_CropGui = CreateCropGui();
	m_Gui->RollOut(ID_ROLLOUT_CROP, _("Cropping"), m_CropGui, false);
	
	//this trick is here to place both voxels and mm controls 
	//at the same location (i.e., there is no gap in gui)	
	//for (int i = 0; i < 2; i++) {
		m_CropEdVxls->Show(false);
		m_CropEdMm->Show(true);
		m_CropGui->FitGui();
	//}	

#ifdef _TEMP_STUFF 
		m_Gui->Button(ID_TEMP_FUNC, "", _("Temp"));
#endif	

	m_Gui->Divider(0);
	m_Gui->Label(" ");

	OnViewROIVolume();
	//UpdateGui();	//fill controls
	return m_Gui;
}

//creates the GUI with information about the large volume data
/*virtual*/ mafGUI* mafVMEVolumeLarge::CreateInfoGui()
{
	mafGUI* gui = new mafGUI(this);
	gui->Show(true);

	gui->Divider(0);
	gui->Label(_("source: "), &m_SourceFile, true);
	gui->Label(_("path: "), &m_SourcePath, true);
	gui->Label(_("dims: "), &m_SourceDimensions, true);
	gui->Label(_("size: "), &m_SourceSize, true);

	gui->Divider(0);

	gui->Label(_("original spacing: "), true);
	gui->Label(&m_SourceSpacing);		
	gui->Label(_("original bounds: "), true);
	gui->Label(&m_SourceBounds[0]);
	gui->Label(&m_SourceBounds[1]);
	gui->Label(&m_SourceBounds[2]);
	
	gui->Divider(0);
	return gui;
}

//creates the GUI with information about the sampled output
/*virtual*/ mafGUI* mafVMEVolumeLarge::CreateSampleInfoGui()
{
	mafGUI* gui = new mafGUI(this);
	gui->Show(true);
	gui->Label(_("sample dims:"), true);
	gui->Label(&m_SampleDimensions);
	gui->Label(_("sample bounds:"), true);
	gui->Label(&m_SampleBounds[0]);
	gui->Label(&m_SampleBounds[1]);
	gui->Label(&m_SampleBounds[2]);
	gui->Label(_("sample size: "), true);
	gui->Label(&m_SampleSize);
	gui->Label(_("memory limit: "), true);
	//gui->Label(&m_SampleMemLimit);

	gui->Slider(ID_SAMPLE_MEMLIMIT, "", &m_SampleMemLimit, 1, 512, 
		_("specifies memory limit for subsampled versions of data"));

	gui->Label(_("sample rate: "), true);
	gui->Label(&m_SampleRate);

	gui->Divider(0);
	return gui;
}


//updates the existing Info gui
/*virtual*/ mafGUI* mafVMEVolumeLarge::CreateCropGui()
{
#pragma warning(suppress: 6211) // warning C6211: Leaking memory 'gui' due to an exception. Consider using a local catch block to clean up memory:
	mafGUI* gui = new mafGUI(this);
	gui->Show(true);

	gui->Divider(0);
	gui->Bool(ID_SHOW_ROI, "show ROI", &m_ShowROI, 0, 
		_("toggle region of interest visibility"));

	m_ShowROIOpt = new mafGUI(this);
	m_ShowROIOpt->Divider(0);

	m_ShowROIOpt->Bool(ID_SHOW_HANDLES, _("show handles"), &m_ShowHandles, 1, 
		_("toggle gizmo handles visibility"));
	m_ShowROIOpt->Bool(ID_SHOW_AXIS, _("show axis"), &m_ShowAxis, 1, 
		_("toggle gizmo translation axis visibility"));
	m_ShowROIOpt->Bool(ID_SHOW_PLANES, _("show planes"), &m_ShowPlanes, 1, 
		_("toggle gizmo translation planes visibility"));

	m_ShowROIOpt->Enable(ID_SHOW_HANDLES, false);
	m_ShowROIOpt->Enable(ID_SHOW_AXIS, false);
	m_ShowROIOpt->Enable(ID_SHOW_PLANES, false);
		
	gui->RollOut(ID_ROLLOUT_SHOW_ROI_OPT, _("show ROI options"), m_ShowROIOpt, false);

	gui->Divider(3);
	gui->Label(_("selected ROI:"), true);
	AddVoxelsMmCombo(gui, ID_COMBO_VOI_UNITS, &m_VOIUnits);

	m_CropEdVxls = new mafGUI(this);	
//	m_CropEdVxls->Show(m_VOIUnits == 0);
	m_CropEdVxls->Show(true);
	m_CropEdVxls->VectorN(ID_CROP_DIR_X, _("range x"), &m_VOI[0], 2, m_FullExtent[0], m_FullExtent[1]);
	m_CropEdVxls->VectorN(ID_CROP_DIR_Y, _("range y"), &m_VOI[2], 2, m_FullExtent[2], m_FullExtent[3]);
	m_CropEdVxls->VectorN(ID_CROP_DIR_Z, _("range z"), &m_VOI[4], 2, m_FullExtent[4], m_FullExtent[5]);
	gui->AddGui(m_CropEdVxls);	
	
	double bounds[6];
#ifdef VME_VOLUME_VER1
	m_LargeData->GetBounds(bounds);	
#else
	mafBrickedFileReader* ds = m_LargeDataReader->GetLevelFile(1);
	if (ds == NULL) 
	{
		bounds[0] = bounds[2] = bounds[4] = -DBL_MAX;
		bounds[1] = bounds[3] = bounds[5] = DBL_MAX;
	}
	else if (ds->IsRectilinearGrid())
  {
    vtkDoubleArray* pXYZCoords = ds->GetXCoordinates();
    bounds[0] = *pXYZCoords->GetPointer(0);
    bounds[1] = *pXYZCoords->GetPointer(pXYZCoords->GetMaxId());

    pXYZCoords = ds->GetYCoordinates();
    bounds[2] = *pXYZCoords->GetPointer(0);
    bounds[3] = *pXYZCoords->GetPointer(pXYZCoords->GetMaxId());

    pXYZCoords = ds->GetZCoordinates();
    bounds[4] = *pXYZCoords->GetPointer(0);
    bounds[5] = *pXYZCoords->GetPointer(pXYZCoords->GetMaxId());
  }
  else
	{
		ds->GetDataOrigin(bounds[0], bounds[2], bounds[4]);

		double sp[3];
		int dims[3];
		ds->GetDataDimensions(dims);
		ds->GetDataSpacing(sp);
		for (int i = 0; i < 3; i++) {
			bounds[2*i + 1] = bounds[2*i] + dims[i]*sp[i];
		}
	}
#endif // VME_VOLUME_VER1

	m_CropEdMm = new mafGUI(this);	
//	m_CropEdMm->Show(m_VOIUnits != 0);	
	m_CropEdMm->Show(true);
	m_CropEdMm->VectorN(ID_CROP_DIR_XMM, _("range x"), &m_ROI[0], 2, bounds[0], bounds[1]);
	m_CropEdMm->VectorN(ID_CROP_DIR_YMM, _("range y"), &m_ROI[2], 2, bounds[2], bounds[3]);
	m_CropEdMm->VectorN(ID_CROP_DIR_ZMM, _("range z"), &m_ROI[4], 2, bounds[4], bounds[5]);
	gui->AddGui(m_CropEdMm);
	
  gui->Bool(ID_AUTOPROOF, _("auto proof"), &m_AutoProof, 1, 
    _("if checked, the resolution of displayed volume increases progresivelly "
     "without the necessity to press 'view ROI volume' button. N.B. the application may "
     "display larger VOI than requested in this mode."));

	gui->Button(ID_RESET_CROPPING_AREA, _("reset ROI"), "", 
		_("reset the cropping area"));
	gui->Button(ID_VIEW_ROI_VOLUME, _("view ROI volume"),"",
		_("displays the data in the selected ROI only, using as high quality as possible"));
	gui->Button(ID_VIEW_ORIGVOI_VOLUME, _("view original volume"),"",
		_("displays the data in the original VOI, using as high quality as possible"));
#ifndef VME_VOLUME_LARGE_EXCLUDE_CROP
	gui->Button(ID_CROP, _("crop ..."),"",
		_("performs the cropping operation"));
#endif // VME_VOLUME_LARGE_EXCLUDE_CROP
	
	//gui->Enable(ID_VIEW_ORIGVOI_VOLUME, !m_ShowFEOutput);
	gui->Divider(0);
	return gui;
}


//updates all existing GUIs
/*virtual*/ void mafVMEVolumeLarge::UpdateGui()
{
	if(m_Gui)
	{
		//update info
		if (m_InfoGui != NULL)
			UpdateInfoGui(m_InfoGui);

		if (m_SampleInfoGui != NULL)
			UpdateSampleInfoGui(m_SampleInfoGui);

		if (m_CropGui != NULL)
			UpdateCropGui(m_CropGui);

		m_Gui->Update();
	}
}


//updates the existing gui
/*virtual*/ void mafVMEVolumeLarge::UpdateInfoGui(mafGUI* gui)
{
	assert(gui != NULL);
  if (gui == NULL)
    return;

	//reset variables
	m_SourceFile = m_SourcePath = m_SourceDimensions = 
		m_SourceSpacing = m_SourceSize = "";
		

	for (int i = 0; i < 3; i++){
		m_SourceBounds[i] = "";
	}


	//and refill them
#ifdef VME_VOLUME_VER1
	if (m_LargeData != NULL)	
	{
		//name
		wxString szPath, szFile, szExt;
		wxSplitPath(this->GetFileName(), &szPath, &szFile, &szExt);
		m_SourcePath = szPath;
		m_SourceFile = szFile + "." + szExt;

		//dims
		int wext[6];
		m_LargeData->GetWholeExtent(wext);
		m_SourceDimensions = wxString::Format("%d x %d x %d", wext[1] - wext[0] + 1,
			wext[3] - wext[2] + 1, wext[5] - wext[4] + 1);

		//size		
    mafVMEVolumeLargeUtils::FormatDataSize(((vtkIdType64)wext[1] - wext[0] + 1)*
			(wext[3] - wext[2] + 1)*(wext[5] - wext[4] + 1), m_SourceSize);		

		//bounds
		double b[6];
		m_LargeData->GetBounds(b);		
		m_SourceBounds[0] = wxString::Format("  xmin: %.2f  xmax: %.2f [mm]", b[0], b[1]);
		m_SourceBounds[1] = wxString::Format("  ymin: %.2f  ymax: %.2f [mm]", b[2], b[3]);
		m_SourceBounds[2] = wxString::Format("  zmin: %.2f  zmax: %.2f [mm]", b[4], b[5]);
		
		vtkMAFLargeImageData* ds = vtkMAFLargeImageData::SafeDownCast(m_LargeData);
		if (ds != NULL)		
		{
			//spacing
			double sp[3];
			ds->GetSpacing(sp);
			m_SourceSpacing = wxString::Format("  %.2f x %.2f x %.2f [mm]", 
				sp[0], sp[1], sp[2]);			
		}		
	}
#else
	if (m_LargeDataReader != NULL)
	{
		//name
		wxString szPath, szFile, szExt;
		wxSplitPath(this->GetFileName(), &szPath, &szFile, &szExt);
		m_SourcePath = szPath;
		m_SourceFile = szFile + "." + szExt;

		mafBrickedFileReader* ds = m_LargeDataReader->GetLevelFile(1);
		assert(ds != NULL);

		if (ds != NULL)
		{
			//dims
			int dims[3];
			ds->GetDataDimensions(dims);
			m_SourceDimensions = wxString::Format("%d x %d x %d", dims[0], dims[1], dims[2]);

			//size		
			mafFormatDataSize(((vtkIdType64)dims[0])*dims[1]*dims[2], m_SourceSize);		

      double b[6];
      if (ds->IsRectilinearGrid())
      {
        //spacing
        m_SourceSpacing = "N/A (rectilinear grid)";

        vtkDoubleArray* pXYZCoords = ds->GetXCoordinates();
        b[0] = *pXYZCoords->GetPointer(0);
        b[1] = *pXYZCoords->GetPointer(pXYZCoords->GetMaxId());

        pXYZCoords = ds->GetYCoordinates();
        b[2] = *pXYZCoords->GetPointer(0);
        b[3] = *pXYZCoords->GetPointer(pXYZCoords->GetMaxId());

        pXYZCoords = ds->GetZCoordinates();
        b[4] = *pXYZCoords->GetPointer(0);
        b[5] = *pXYZCoords->GetPointer(pXYZCoords->GetMaxId());
      }
      else
      {
			  //spacing
			  double sp[3];
			  ds->GetDataSpacing(sp);
			  m_SourceSpacing = wxString::Format("  %.2f x %.2f x %.2f [mm]", 
				  sp[0], sp[1], sp[2]);

			  //bounds			  
			  ds->GetDataOrigin(b[0], b[2], b[4]);
			  for (int i = 0; i < 3; i++) {
				  b[2*i + 1] = b[2*i] + dims[i]*sp[i];
			  }
      }
			
      m_SourceBounds[0] = wxString::Format("  xmin: %.2f  xmax: %.2f [mm]", b[0], b[1]);
			m_SourceBounds[1] = wxString::Format("  ymin: %.2f  ymax: %.2f [mm]", b[2], b[3]);
			m_SourceBounds[2] = wxString::Format("  zmin: %.2f  zmax: %.2f [mm]", b[4], b[5]);					      
		}
	}
#endif

	gui->Update();
}

//updates the existing sample Info gui
/*virtual*/ void mafVMEVolumeLarge::UpdateSampleInfoGui(mafGUI* gui)
{
	assert(gui != NULL);
  if (gui == NULL)
    return;

	m_SampleDimensions = m_SampleSize = m_SampleRate = "";
	for (int i = 0; i < 3; i++){
		m_SampleBounds[i] = "";
	}

	//Sample VOI
	double b[6];
	GetOutput()->GetVMELocalBounds(b);
	m_SampleBounds[0] = wxString::Format("  xmin: %.2f  xmax: %.2f [mm]", b[0], b[1]);
	m_SampleBounds[1] = wxString::Format("  ymin: %.2f  ymax: %.2f [mm]", b[2], b[3]);
	m_SampleBounds[2] = wxString::Format("  zmin: %.2f  zmax: %.2f [mm]", b[4], b[5]);

	vtkDataSet* ds = GetOutput()->GetVTKData();				
	
	//dims
	int wext[6];
	ds->GetWholeExtent(wext);
	m_SampleDimensions = wxString::Format("%d x %d x %d", wext[1] - wext[0] + 1,
		wext[3] - wext[2] + 1, wext[5] - wext[4] + 1);

	//size		
  int nBytesPerVoxel = 0;
  if (vtkImageData::SafeDownCast(ds) != NULL)
    nBytesPerVoxel = ((vtkImageData*)ds)->GetScalarSize();
  else if (ds->GetPointData()->GetScalars() != NULL)
    nBytesPerVoxel = ds->GetPointData()->GetScalars()->GetDataTypeSize();
	mafFormatDataSize(
    ((vtkIdType64)wext[1] - wext[0] + 1)*
		(wext[3] - wext[2] + 1)*(wext[5] - wext[4] + 1)*nBytesPerVoxel, m_SampleSize);	
		
#ifdef VME_VOLUME_VER1
	//sample rate
	int a[3];	
	if (m_ShowFEOutput && m_FEOutput != NULL)
		m_SampleRate = wxString::Format("%d : %d : %d", 
		m_FESampleRate[0], m_FESampleRate[1], m_FESampleRate[2]);
	else		
	{
		m_LargeData->GetSampleRate(a);
		m_SampleRate = wxString::Format("%d : %d : %d", a[0], a[1], a[2]);

	}

	//memory limit
	if (m_LargeData->GetAutoSampleRate()) {
		//mafVMEVolumeLargeUtils::FormatDataSize(((vtkIdType64) m_LargeData->GetMemoryLimit()) * 1024,
		//	m_SampleMemLimit);
		m_SampleMemLimit = m_LargeData->GetMemoryLimit() / 1024;
		gui->Enable(ID_SAMPLE_MEMLIMIT, true);
	}
	else
	{
		m_SampleRate += _(" (fixed)");
		//m_SampleMemLimit = _("N/A");
		gui->Enable(ID_SAMPLE_MEMLIMIT, false);
	}
#else	
	//sample rate and memory limit
	m_SampleRate = wxString::Format("%d", m_LargeDataReader->GetSampleRate());
	m_SampleMemLimit = m_LargeDataReader->GetMemoryLimit() / 1024;
	gui->Enable(ID_SAMPLE_MEMLIMIT, true);
#endif

	gui->Update();
}

//updates the existing Crop gui
/*virtual*/ void mafVMEVolumeLarge::UpdateCropGui(mafGUI* gui)
{
	assert(gui != NULL);
  if (gui == NULL)
    return;
	
	m_CropEdVxls->Show(m_VOIUnits == 0);
	m_CropEdMm->Show(m_VOIUnits != 0);

	//gui->Enable(ID_VIEW_ORIGVOI_VOLUME, !m_ShowFEOutput);

	if (m_GizmoROI != NULL)
	{
		m_GizmoROI->GetBounds(m_ROI);
		InverseTransformExtent(m_ROI, m_VOI);
	}	

	m_CropEdVxls->Update();
	m_CropEdMm->Update();
	gui->Update();
}

#pragma region GUI MISC
//adds a new combobox (identified by id) with "voxels", "mm" options
//onto the given gui; pvar is the combo data variable 
void mafVMEVolumeLarge::AddVoxelsMmCombo(mafGUI* gui, int id, int* pvar)
{
	const int UNITCHOISE_NUM = 2;
	const wxString UNITCHOISE_STR[UNITCHOISE_NUM] = {_("voxels"), _("mm")};

	gui->Combo(id, _("units:"), pvar, UNITCHOISE_NUM, UNITCHOISE_STR);
}

//updates the gizmo
void mafVMEVolumeLarge::UpdateGizmo()
{
	if (m_VOIUnits == 0)	//voxels
	{
		TransformExtent(m_VOI, m_ROI);
		m_CropEdMm->Update();
	}
	else
	{
		InverseTransformExtent(m_ROI, m_VOI);
		m_CropEdVxls->Update();
	}

	if (m_GizmoROI != NULL)
		m_GizmoROI->SetBounds(m_ROI);
}

#ifndef VME_VOLUME_VER1
//------------------------------------------------------------------------
// Gets VOI from the underlaying large reader and updates GUI
void mafVMEVolumeLarge::UpdateVOI(int VOI[6])
//------------------------------------------------------------------------
{
  memcpy(m_VOI, VOI, sizeof(VOI));
  TransformExtent(m_VOI, m_ROI);
  m_CropEdMm->Update();
  m_CropEdVxls->Update();

  if (m_GizmoROI != NULL)
    m_GizmoROI->SetBounds(m_ROI);
}
#endif

#pragma endregion //GUI MISC
#pragma endregion //GUI

/*virtual*/ void mafVMEVolumeLarge::UnRegister(void *obj)
{
	//NB. if ROI gizmo is on, it keeps some references to this VME,
	//which prevents the VME from its destruction, thus also GizmoROI 
	//is not deleted => memory leaks. Therefore, if our parent does not exist, i.e.,
	//the VME is disconnected from the tree and the gizmo is on,
	//we need to destroy the gizmo first to allow the destruction of VME
	if (m_GizmoROI != NULL && GetParent() == NULL)
	{
		mafGizmoROI_BES* roi = m_GizmoROI;
		m_GizmoROI = NULL;

		roi->Show(false);
		cppDEL(roi);
	}

	Superclass::UnRegister(obj);
}

#ifdef _TEMP_STUFF
#pragma region Temporary Stuff

#include "../IO/mafVolumeLargeWriter.h"

//temporary function
void mafVMEVolumeLarge::OnTempFunc()
{
/*if (wxMessageBox("Skip to recreate LODs?", "", wxYES_NO) == wxNO)
{
	mafVolumeLargeWriter bp;
	bp.SetInputDataSet((vtkMAFLargeImageData*)m_LargeData);
	bp.SetOutputFileName("g:\\bricks.vbf");	
	bp.SetListener(this);
	bp.Update();
}*/	

	wxBusyCursor busyCursor;
		
	if (m_LargeDataReader == NULL)
	{
		m_LargeDataReader = new mafVolumeLargeReader();
		m_LargeDataReader->SetFileName("g:\\bricks.bbf");
		m_LargeDataReader->SetListener(this);
	}
	 
	

	//set a new VOI
	m_LargeDataReader->SetVOI(m_VOI);
	m_LargeDataReader->SetMemoryLimit(m_SampleMemLimit*1024);	

#ifdef _PROFILE_LARGEDATA_UPDATE
	LARGE_INTEGER liBegin;
	::QueryPerformanceCounter(&liBegin);
#endif

	m_LargeDataReader->Update();

#ifdef _PROFILE_LARGEDATA_UPDATE
	LARGE_INTEGER liEnd, liFreq;
	::QueryPerformanceCounter(&liEnd);
	::QueryPerformanceFrequency(&liFreq);

	wxMessageBox(wxString::Format("m_LargeData->Update() in %d ms",
		(int)(((liEnd.QuadPart - liBegin.QuadPart)*1000) / liFreq.QuadPart)));

	//NOTE: Snapshot whole extent is destroyed now because of wxMessageBox
#endif

	SetData(m_LargeDataReader->GetOutputDataSet(), 0, MAF_VME_REFERENCE_DATA);
	m_LargeDataReader->GetVOI(m_VOI);

	this->Modified();
	

	int wext[6];
	vtkDataSet* ds = GetOutput()->GetVTKData();	
	m_LargeDataReader->GetOutputDataSet()->GetWholeExtent(wext);
	
	ds->SetWholeExtent(wext);
	ds->SetUpdateExtentToWholeExtent();
	//end BES	

	GetOutput()->Update();

	//BES: this resets the currently opened view 
	//unfortunately, MAF logic does not update all views
	mafEvent ev(this, ds->GetNumberOfConsumers() == 0 ?
		VME_SELECTED : VME_VISUAL_MODE_CHANGED,this);
	this->ForwardUpEvent(&ev);
}

#pragma endregion
#endif