/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoDICOMImporter.cpp,v $
  Language:  C++
  Date:      $Date: 2005-10-10 13:05:40 $
  Version:   $Revision: 1.1 $
  Authors:   Paolo Quadrani    Stefano Perticoni
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

#include "mmoDICOMImporter.h"

#include <wx/listimpl.cpp>
#include "wx/busyinfo.h"

#include "mafEvent.h"
#include "mafRWIBase.h"
#include "mmgValidator.h"
#include "mmgGui.h"
#include "mafRWI.h"
#include "mmgDialog.h"

#include "mmdMouse.h"
#include "mmiDICOMImporterInteractor.h"

#include "mafVMEVolumeGray.h"
#include "mafTagArray.h"
#include "mafVMEItem.h"

#include "vtkMAFSmartPointer.h"
#include "vtkRendererCollection.h"
#include "vtkCamera.h"
#include "vtkPlaneSource.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkDicomUnPacker.h"
#include "vtkDirectory.h"
#include "vtkTexture.h"
#include "vtkWindowLevelLookupTable.h"
#include "vtkProbeFilter.h"
#include "vtkStructuredPoints.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkOutlineFilter.h"
#include "vtkRGSliceAccumulate.h"

int compareX(const mmoDICOMImporterListElement **arg1,const mmoDICOMImporterListElement **arg2);
int compareY(const mmoDICOMImporterListElement **arg1,const mmoDICOMImporterListElement **arg2);
int compareZ(const mmoDICOMImporterListElement **arg1,const mmoDICOMImporterListElement **arg2);
WX_DEFINE_LIST(ListDicomFiles);
//----------------------------------------------------------------------------
mmoDICOMImporter::mmoDICOMImporter(wxString label) : mafOp(label)
//----------------------------------------------------------------------------
{
	m_OpType	= OPTYPE_IMPORTER;
	m_Canundo	= true;

	for (int i = 0; i < 6; i++) 
    m_DicomBounds[i] = 0;
	m_Volume	= NULL;
	m_SortAxes						= 2;
	m_NumberOfStudy			= 0;
	m_NumberOfSlices		= 0;

  m_DictionaryFilename	= "";
  wxString dictionary = mafGetApplicationDirectory().c_str();
  dictionary += "\\Config\\Dictionary\\";
	dictionary.Append("dicom3.dic");
	if(wxFileExists(dictionary)) 
		m_DictionaryFilename = dictionary;

	m_CTDir						= mafGetApplicationDirectory().c_str();
  m_CTDir += "\\Data\\External\\";
	m_CurrentSlice			= 0;
  m_BuildStepValue				= 0;
	m_BuildStepChoices[0]		= "1x";
	m_BuildStepChoices[1]		= "2x";
	m_BuildStepChoices[2]		= "4x";
	m_PatientName							= " ";
	m_SurgeonName      = " ";
	m_Identifier				= 0;
  m_slice_text				= NULL;
	m_SliceScanner			= NULL;
	m_SliceLabel					= NULL;

	m_CropMode					= false;
	m_CropFlag					= false;

	m_FilesList				= NULL;
	m_DicomDialog								= NULL;
	m_RWI								= NULL;
	m_TagArray 				= NULL;

  m_SliceTexture						= NULL;
  m_CTDirectoryReader			= NULL;
	m_DicomReader			= NULL;
	m_CropPlane		 						= NULL;
  m_SlicePlane 						= NULL;
	m_SliceMapper						= NULL;
	m_SliceActor							= NULL;
	m_SliceLookupTable				= NULL;
	
	m_GizmoStatus = GIZMO_NOT_EXIST;
	m_SideToBeDragged = 0;
}
//----------------------------------------------------------------------------
mmoDICOMImporter::~mmoDICOMImporter()
//----------------------------------------------------------------------------
{
	vtkDEL(m_Volume);
}
//----------------------------------------------------------------------------
mafOp *mmoDICOMImporter::Copy()
//----------------------------------------------------------------------------
{
	return new mmoDICOMImporter(m_Label);
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::OpRun()   
//----------------------------------------------------------------------------
{
	CreatePipeline();
	CreateGui();
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::OpStop(int result)
//----------------------------------------------------------------------------
{
  m_Mouse->RemoveObserver(m_DicomInteractor);

  //close dialog
	for (int i=0; i < m_NumberOfStudy;i++)
	{
		((ListDicomFiles *)m_StudyListbox->GetClientData(i))->DeleteContents(TRUE);
		((ListDicomFiles *)m_StudyListbox->GetClientData(i))->Clear();
	}

  m_RWI->m_RenFront->RemoveActor(m_SliceActor);   //SIL. 28-11-2003: - you must always remove actors from the Renderer before cleaning
  m_RWI->m_RenFront->RemoveActor(m_CropActor);   
	
	vtkDEL(m_SliceTexture);
	vtkDEL(m_CTDirectoryReader);
	vtkDEL(m_DicomReader);
	vtkDEL(m_SliceLookupTable);
	vtkDEL(m_SlicePlane);
	vtkDEL(m_SliceMapper);
	vtkDEL(m_SliceActor);
	vtkDEL(m_CropPlane);
	vtkDEL(m_CropActor);

	mafDEL(m_TagArray);
  mafDEL(m_DicomInteractor);

  cppDEL(m_RWI);
	cppDEL(m_DicomDialog);
	cppDEL(m_FilesList);

	mafEventMacro(mafEvent(this,result));
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::OpDo()
//----------------------------------------------------------------------------
{
	assert(m_Volume != NULL);
	mafEventMacro(mafEvent(this,VME_ADD,m_Volume));
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::OpUndo()
//----------------------------------------------------------------------------
{   
	assert(m_Volume != NULL);
  mafEventMacro(mafEvent(this,VME_REMOVE,m_Volume));
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::CreatePipeline()
//----------------------------------------------------------------------------
{
	vtkNEW(m_CTDirectoryReader);

	vtkNEW(m_DicomReader);
	m_DicomReader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
  m_DicomReader->UseDefaultDictionaryOff();

	vtkNEW(m_SliceLookupTable);

	vtkNEW(m_SliceTexture);
	m_SliceTexture->InterpolateOn();

	vtkNEW(m_SlicePlane);
	
	vtkNEW(m_SliceMapper);
	m_SliceMapper->SetInput(m_SlicePlane->GetOutput());
		
	vtkNEW(m_SliceActor);
	m_SliceActor->SetMapper(m_SliceMapper);
	m_SliceActor->SetTexture(m_SliceTexture);
	m_SliceActor->VisibilityOff();

	vtkNEW(m_CropPlane);

	vtkMAFSmartPointer<vtkOutlineFilter>	of;
	of->SetInput(((vtkDataSet *)(m_CropPlane->GetOutput())));

	vtkMAFSmartPointer<vtkPolyDataMapper> pdm;
	pdm->SetInput(of->GetOutput());

	vtkNEW(m_CropActor);
	m_CropActor->GetProperty()->SetColor(0.8,0,0);
	m_CropActor->VisibilityOff();
	m_CropActor->SetMapper(pdm);

	m_RWI = new mafRWI(mafGetFrame(),ONE_LAYER,true);
  m_RWI->SetListener(this);
  m_RWI->SetSize(0,0,500,500);
	m_RWI->CameraSet(CAMERA_CT);
  m_RWI->m_RenFront->AddActor(m_SliceActor);
  m_RWI->m_RenFront->AddActor(m_CropActor);
  m_RWI->m_RwiBase->SetMouse(m_Mouse);
	
	mafNEW(m_DicomInteractor);
	m_DicomInteractor->SetListener(this);
  m_Mouse->AddObserver(m_DicomInteractor, MCH_INPUT);
}
//----------------------------------------------------------------------------
// Constants :
//----------------------------------------------------------------------------
enum DICOM_IMPORTER_ID
{
	ID_FIRST = MINID,
	ID_DICTIONARY,
	ID_OPEN_DIR,
	ID_SCAN_SLICE,
	ID_CROP_MODE_BUTTON,
	ID_BUILDVOLUME_MODE_BUTTON,
	ID_STUDY,
	ID_CROP_BUTTON,
	ID_UNDO_CROP_BUTTON,
	ID_BUILD_STEP,
	ID_BUILD_BUTTON,
	ID_CANCEL,
	ID_PATIENT_NAME,
	ID_PATIENT_ID,
	ID_SURGEON_NAME,
};
//----------------------------------------------------------------------------
void mmoDICOMImporter::CreateGui()
//----------------------------------------------------------------------------
{
	m_DicomDialog = new mmgDialog("dicom importer", mafCLOSEWINDOW | mafRESIZABLE);
	int x_init,y_init;
	x_init = mafGetFrame()->GetPosition().x;
	y_init = mafGetFrame()->GetPosition().y;

	mafString wildcard = "DICT files (*.dic)|*.dic|All Files (*.*)|*.*";

	m_Gui = new mmgGui(this);
	m_Gui->SetListener(this);

	m_Gui->Label("data files:",true);
	m_Gui->FileOpen (ID_DICTIONARY,	"dictionary",	&m_DictionaryFilename, wildcard);
	m_Gui->DirOpen(ID_OPEN_DIR, "ct folder",	&m_CTDir);
	m_Gui->Divider();
	m_Gui->Label("patient info:",true);		
	m_Gui->String(ID_PATIENT_NAME,"name ",&m_PatientName);
	m_Gui->Double(ID_PATIENT_ID,"id ",&m_Identifier);
	m_StudyListbox = m_Gui->ListBox(ID_STUDY,"study id",50);
	m_Gui->Label("surgeon info:",true);
	m_Gui->String(ID_SURGEON_NAME,"name ",&m_SurgeonName);
	m_Gui->Label("mode",true);
	m_Gui->Button(ID_CROP_MODE_BUTTON,"crop mode", "");
	m_Gui->Button(ID_BUILDVOLUME_MODE_BUTTON,"build volume mode", "");
	m_Gui->Label("crop",true);
	m_Gui->Button(ID_CROP_BUTTON,"crop", "");
	m_Gui->Button(ID_UNDO_CROP_BUTTON,"undo crop", "");
	m_Gui->Label("build volume",true);
	m_Gui->Combo(ID_BUILD_STEP, "step", &m_BuildStepValue, 3, m_BuildStepChoices);
	m_Gui->Button(ID_BUILD_BUTTON,"build", "");
	m_Gui->Divider();
	m_Gui->Button(ID_CANCEL,"cancel");

	m_Gui->Enable(ID_OPEN_DIR,m_DictionaryFilename != "");
	m_Gui->Enable(ID_CROP_MODE_BUTTON,0);
	m_Gui->Enable(ID_BUILDVOLUME_MODE_BUTTON,0);
	m_Gui->Enable(ID_CROP_BUTTON,0);
	m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
	m_Gui->Enable(ID_BUILD_STEP,0);
	m_Gui->Enable(ID_BUILD_BUTTON,0);
	
	m_Gui->Show(true);
	m_Gui->Reparent(m_DicomDialog);
	m_Gui->FitGui();
	m_Gui->SetSize(220, 430);
	m_Gui->Update();

	wxPoint dp = wxDefaultPosition;
	m_SliceLabel     = new wxStaticText(m_DicomDialog, -1, " slice num. ",dp, wxSize(-1,16));
	m_slice_text    = new wxTextCtrl  (m_DicomDialog, -1, "",					   dp, wxSize(30,16), wxNO_BORDER);
	m_SliceScanner = new wxSlider    (m_DicomDialog, -1,0,0,100,			   dp, wxSize(200,22));
	m_SliceLabel     ->Enable(false);
	m_slice_text    ->Enable(false);
	m_SliceScanner ->Enable(false);

	m_SliceScanner->SetValidator(mmgValidator(this,ID_SCAN_SLICE,m_SliceScanner,&m_CurrentSlice,m_slice_text));
  m_slice_text->   SetValidator(mmgValidator(this,ID_SCAN_SLICE,m_slice_text,&m_CurrentSlice,m_SliceScanner,0,100));

	wxBoxSizer *slice_sizer = new wxBoxSizer(wxHORIZONTAL);
	slice_sizer->Add(m_SliceLabel,      0, wxALIGN_CENTER|wxRIGHT, 5);
	slice_sizer->Add(m_slice_text,		 0, wxALIGN_CENTER|wxRIGHT, 5);
	slice_sizer->Add(m_SliceScanner,  1, wxALIGN_CENTER|wxEXPAND);

	m_RWI->SetSize(0,0,380,200);
	m_RWI->m_RwiBase->Reparent(m_DicomDialog);
	m_RWI->m_RwiBase->Show(true);

	wxBoxSizer *v_sizer = new wxBoxSizer(wxVERTICAL);
	v_sizer->Add(m_RWI->m_RwiBase,1, wxEXPAND);
	v_sizer->Add(slice_sizer, 0, wxEXPAND);
	
	wxBoxSizer *h_sizer = new wxBoxSizer(wxHORIZONTAL);
	h_sizer->Add(v_sizer, 1, wxEXPAND);
	h_sizer->Add(m_Gui,   0, wxLEFT, 5);

	//m_DicomDialog->SetSizer(h_sizer);
	//m_DicomDialog->SetAutoLayout(TRUE);
	//h_sizer->Fit(m_DicomDialog);
  m_DicomDialog->Add(h_sizer);
  
  int w,h;
  m_DicomDialog->GetSize(&w,&h);
  m_DicomDialog->SetSize(x_init+10,y_init+10,w,h);

	m_DicomDialog->ShowModal();
	int res = (m_DicomDialog->GetReturnCode() == wxID_OK) ? OP_RUN_OK : OP_RUN_CANCEL;

	OpStop(res);
	return;
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::BuildDicomFileList(const char *dir)
//----------------------------------------------------------------------------
{
	int row, i;
	double slice_pos[3];
	if (m_CTDirectoryReader->Open(dir) == 0)
	{
		wxMessageBox(wxString::Format("Directory <%s> can not be opened",dir),"Warning!!");
		return;
	}
	// get the dicom files from the directory
	wxBusyInfo wait_info("Reading CT directory: please wait");
	
	for (i=0; i < m_CTDirectoryReader->GetNumberOfFiles(); i++)
	{
		if ((strcmp(m_CTDirectoryReader->GetFile(i),".") == 0) || (strcmp(m_CTDirectoryReader->GetFile(i),"..") == 0)) 
		{
			continue;
		}
		else
		{
			m_current_slice_name = m_CTDirectoryReader->GetFile(i);
			// Append of the path at the dicom file
			wxString str_tmp, ct_mode;
			str_tmp.Append(dir);
			str_tmp.Append("\\");
			str_tmp.Append(m_current_slice_name);
			
      vtkDicomUnPacker *reader = vtkDicomUnPacker::New();
			reader->SetFileName((char *)str_tmp.c_str());
      reader->UseDefaultDictionaryOff();
			reader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
			reader->UpdateInformation();

			ct_mode = reader->GetCTMode();
			ct_mode.MakeUpper();
			ct_mode.Trim(FALSE);
			ct_mode.Trim();

			//if (strcmp(reader->GetCTMode(),"SCOUT MODE") == 0 || reader->GetStatus() == -1)
			if(ct_mode.Find("SCOUT") != -1 || reader->GetStatus() == -1)
			{
				reader->Delete();
				continue;
			}
			
			//row = m_StudyListbox->FindString(reader->GetStudy());
      row = m_StudyListbox->FindString(reader->GetStudyUID());
			if (row == -1)
			{
				// the study is not present into the listbox, so need to create new
				// list of files related to the new studyID
				m_FilesList = new ListDicomFiles;
				//m_StudyListbox->Append(reader->GetStudy());
        m_StudyListbox->Append(reader->GetStudyUID());
				m_StudyListbox->SetClientData(m_NumberOfStudy,(void *)m_FilesList);
				reader->GetSliceLocation(slice_pos);
				//mmoDICOMImporterListElement *element = new mmoDICOMImporterListElement(str_tmp,slice_pos);
				//m_FilesList->Append(element);
				m_FilesList->Append(new mmoDICOMImporterListElement(str_tmp,slice_pos));
				m_NumberOfStudy++;
			}
			else 
			{
				reader->GetSliceLocation(slice_pos);
				//mmoDICOMImporterListElement *element = new mmoDICOMImporterListElement(str_tmp,SlicePos);
				//((ListDicomFiles *)m_StudyListbox->GetClientData(row))->Append(element);
				((ListDicomFiles *)m_StudyListbox->GetClientData(row))->Append(new mmoDICOMImporterListElement(str_tmp,slice_pos));
			}
      reader->Delete();
		}
	}
	if(m_NumberOfStudy == 0)
	{
		wxString msg = "No study found!";
		wxMessageBox(msg,"Confirm", wxOK , NULL);
	}
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::BuildVolume()
//----------------------------------------------------------------------------
{
	int step;

	if(m_BuildStepValue == 0)
		step = 1;
	else 
		step = m_BuildStepValue << 1;

	int n_slices = m_NumberOfSlices / step;

	wxBusyInfo wait_info("Building volume: please wait");

	vtkMAFSmartPointer<vtkRGSliceAccumulate> accumulate;
	accumulate->SetNumberOfSlices(n_slices);
	accumulate->BuildVolumeOnAxes(m_SortAxes);
	
	for (int count = 0, s_count = 0; count < m_NumberOfSlices; count += step)
	{
		if (s_count == n_slices) {break;}
    ShowSlice(count);
    accumulate->SetSlice(s_count, m_SliceTexture->GetInput());
    s_count++;
	}
	
  ImportDicomTags();

	//Copy inside the first VME item of m_Volume the CT volume and Dicom's tags
	mafNEW(m_Volume);
	m_Volume->GetTagArray()->DeepCopy(m_TagArray);
	mafDEL(m_TagArray);

	accumulate->Update();
  m_Volume->SetDataByDetaching(accumulate->GetOutput(),0);
	
	mafTagItem tag_Nature;
	tag_Nature.SetName("VME_NATURE");
	tag_Nature.SetValue("NATURAL");
	m_Volume->GetTagArray()->SetTag(tag_Nature);

	mafTagItem tag_Surgeon;
	tag_Surgeon.SetName("SURGEON_NAME");
	tag_Surgeon.SetValue(m_SurgeonName.GetCStr());
	m_Volume->GetTagArray()->SetTag(tag_Surgeon);

	//Nome VME = CTDir + IDStudio
	wxString name = m_CTDir + " - " + m_StudyListbox->GetString(m_StudyListbox->GetSelection());			
	m_Volume->SetName(name.c_str());
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::ImportDicomTags()
//----------------------------------------------------------------------------
{
  if (m_TagArray == NULL) 
    mafNEW(m_TagArray);

  for (int i=0;i<m_DicomReader->GetNumberOfTags();i++)
  {
	  char tmp[256];
	  const char *keyword;

	  const vtkDicomUnPacker::DICOM *tag = m_DicomReader->GetTag(i);

	  if (!strcmp(tag->Keyword,""))
	  {
		  if (tag->intoDictionary == vtkDicomUnPacker::no)
		  {
			  int group=tag->Group;
			  int element=tag->Element;
			  sprintf(tmp,"G%d_E%d",group,element);
			  keyword=tmp;
		  }
	  }
	  else
	  {
		  keyword=(char *)tag->Keyword;
		  if (tag->intoDictionary==vtkDicomUnPacker::no)
		  {
			  wxLogMessage("Hmmm Keyword not in dictionary???");
		  }
	  }

    int mult=m_DicomReader->GetTagElement(i)->mult;

	  const vtkDicomUnPacker::VALUE *tagElement=m_DicomReader->GetTagElement(i);
    
    if (tagElement->type == vtkDicomUnPacker::string)
    {
      // String TAG
      m_TagArray->SetTag(mafTagItem(keyword,""));
      if (mult>=0)
      {
        // Fill the Element components
        m_TagArray->GetTag(keyword)->SetNumberOfComponents(mult+1);

        for (int j=0;j<=mult;j++)
        {
          const char *value = tagElement->stringa[j];
          m_TagArray->GetTag(keyword)->SetValue(value,j);
        }
      }
      else
      {
        m_TagArray->GetTag(keyword)->SetNumberOfComponents(0);
        cerr << "empty string...\n";
      }
    }
    else
    {
      // NUMERIC TAG
	    double *value = (double *)tagElement->num;
      
      if (mult>0)
      {
        m_TagArray->SetTag(mafTagItem(keyword,value,mult+1));
      }
	    else
	    {
        // This is a atg with an emptt value...
        mafTagItem empty(keyword,"",MAF_MISSING_TAG);
        //empty.SetNumberOfComponents(0);
        m_TagArray->SetTag(empty);
	    }
    }
  }
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::ShowSlice(int slice_num)
//----------------------------------------------------------------------------
{
	// Description:
	// read the slice number 'slice_num' and generate the texture
  double spacing[3], crop_bounds[6], range[2], loc[3];

  m_DicomReader->SetFileName((char *)m_ListSelected->Item(slice_num)->GetData()->GetFileName());
	m_DicomReader->Modified();
	m_DicomReader->Update();
	
	m_DicomReader->GetSliceLocation(loc);
	//double bounds[6];
  
	//m_DicomReader->GetOutput()->GetBounds(bounds);
	m_DicomReader->GetOutput()->GetBounds(m_DicomBounds);

	// switch from m_DicomReader and v_dicom_probe on m_CropFlag
	if (m_CropFlag) 
	{
		m_DicomReader->GetOutput()->GetSpacing(spacing);
		m_CropPlane->GetOutput()->GetBounds(crop_bounds);
		crop_bounds[4] = m_DicomBounds[4];
		crop_bounds[5] = m_DicomBounds[5];

		if(crop_bounds[1] > m_DicomBounds[1]) 
			crop_bounds[1] = m_DicomBounds[1];
		if(crop_bounds[3] > m_DicomBounds[3]) 
			crop_bounds[3] = m_DicomBounds[3];
        
    double dim_x_clip = ceil((double)(((crop_bounds[1] - crop_bounds[0]) / spacing[0]) + 1));
    double dim_y_clip = ceil((double)(((crop_bounds[3] - crop_bounds[2]) / spacing[1]) + 1));
    
		vtkMAFSmartPointer<vtkStructuredPoints> clip;
    clip->SetOrigin(crop_bounds[0], crop_bounds[2], loc[m_SortAxes]);	//modified by Paolo 12-11-2003
		clip->SetSpacing(spacing[0], spacing[1], 0);
		clip->SetDimensions(dim_x_clip, dim_y_clip, 1);
		clip->Update();

		vtkMAFSmartPointer<vtkProbeFilter> probe;
    probe->SetInput(clip);
		probe->SetSource(m_DicomReader->GetOutput());
		probe->Update();
		probe->GetOutput()->GetBounds(m_DicomBounds);
		probe->GetOutput()->GetScalarRange(range);
		
		m_SliceTexture->SetInput((vtkImageData *)probe->GetOutput());
	} 
	else 
	{
    m_DicomReader->GetOutput()->GetScalarRange(range);
		m_SliceTexture->SetInput(m_DicomReader->GetOutput());
	}

	m_SliceLookupTable->SetTableRange(range);
	m_SliceLookupTable->SetWindow(range[1] - range[0]);
	m_SliceLookupTable->SetLevel((range[1] + range[0]) / 2.0);
	m_SliceLookupTable->Build();
	
	m_SliceTexture->MapColorScalarsThroughLookupTableOn();
	m_SliceTexture->SetLookupTable((vtkLookupTable *)m_SliceLookupTable);
	
	m_SlicePlane->SetOrigin(m_DicomBounds[0],m_DicomBounds[2],0);
	m_SlicePlane->SetPoint1(m_DicomBounds[1],m_DicomBounds[2],0);
	m_SlicePlane->SetPoint2(m_DicomBounds[0],m_DicomBounds[3],0);
	m_SliceActor->VisibilityOn();
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::ResetSliders()
//----------------------------------------------------------------------------
{
	m_SliceScanner->SetRange(0,m_NumberOfSlices - 1);
	m_Gui->Update();
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::ResetStructure()
//----------------------------------------------------------------------------
{
	// disable the scan slider
	//m_Gui->Enable(ID_SCAN_SLICE,0);
	m_SliceLabel->Enable(false);
	m_slice_text->Enable(false);
	m_SliceScanner->Enable(false);
	
	// disable the button modality
	m_Gui->Enable(ID_CROP_MODE_BUTTON,0);
	m_Gui->Enable(ID_BUILDVOLUME_MODE_BUTTON,0);
	m_Gui->Enable(ID_CROP_BUTTON,0);
	m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
	// disable the build volume modality widgets
	m_Gui->Enable(ID_BUILD_STEP,0);	
	m_Gui->Enable(ID_BUILD_BUTTON,0);

	// delete the previous studyes detected and reset the related variables
	for (int i=0; i < m_NumberOfStudy;i++)
	{
		((ListDicomFiles *)m_StudyListbox->GetClientData(i))->DeleteContents(true);
		((ListDicomFiles *)m_StudyListbox->GetClientData(i))->Clear();
	}
	m_StudyListbox->Clear();
	m_NumberOfStudy		= 0;
	m_NumberOfSlices	= 0;
	m_CurrentSlice		= 0;

	m_CropFlag				= false;
	
	m_Gui->Update();
	mafYield();
}
//----------------------------------------------------------------------------
int compareX(const mmoDICOMImporterListElement **arg1,const mmoDICOMImporterListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the x coordinate of both arguments
	// return:
	double x1 = (*(mmoDICOMImporterListElement **)arg1)->GetCoordinate(0);
	double x2 = (*(mmoDICOMImporterListElement **)arg2)->GetCoordinate(0);
	if (x1 > x2)
		return 1;
	if (x1 < x2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareY(const mmoDICOMImporterListElement **arg1,const mmoDICOMImporterListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the y coordinate of both arguments
	// return:
	double y1 = (*(mmoDICOMImporterListElement **)arg1)->GetCoordinate(1);
	double y2 = (*(mmoDICOMImporterListElement **)arg2)->GetCoordinate(1);
	if (y1 > y2)
		return 1;
	if (y1 < y2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
int compareZ(const mmoDICOMImporterListElement **arg1,const mmoDICOMImporterListElement **arg2)
//----------------------------------------------------------------------------
{
	// compare the z coordinate of both arguments
	// return:
	double z1 = (*(mmoDICOMImporterListElement **)arg1)->GetCoordinate(2);
	double z2 = (*(mmoDICOMImporterListElement **)arg2)->GetCoordinate(2);
	if (z1 > z2)
		return 1;
	if (z1 < z2)
		return -1;
	else
		return 0;
}
//----------------------------------------------------------------------------
void mmoDICOMImporter::	OnEvent(mafEventBase *maf_event) 
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_CANCEL:
        m_DicomDialog->EndModal(wxID_CANCEL);
        //OpStop(OP_RUN_CANCEL); //mafEventMacro(mafEvent(this,OP_RUN_CANCEL));
      break;
      case ID_DICTIONARY:
        m_DicomReader->SetDictionaryFileName(m_DictionaryFilename.GetCStr());
        m_Gui->Enable(ID_OPEN_DIR,1);
      break;
      case ID_OPEN_DIR:
        m_Gui->Update();
        ResetStructure();
        // scan dicom directory
        BuildDicomFileList(m_CTDir.GetCStr());
        if(m_NumberOfStudy == 1)
        {
          m_StudyListbox->SetSelection(0);
          OnEvent(&mafEvent(this, ID_STUDY));
        }
      break;
      case ID_STUDY:
      {
        //m_Gui->Enable(ID_SCAN_SLICE,1);
        m_SliceLabel->Enable(true);
        m_slice_text->Enable(true);
        m_SliceScanner->Enable(true);

        m_Gui->Enable(ID_CROP_MODE_BUTTON,1);
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
        m_Gui->Enable(ID_BUILDVOLUME_MODE_BUTTON,1);
        int sel = m_StudyListbox->GetSelection();
        m_ListSelected = (ListDicomFiles *)m_StudyListbox->GetClientData(sel);
        // sort dicom slices
        if(m_ListSelected->Number() > 1)
        {
          double item1_pos[3],item2_pos[3],d[3];
          mmoDICOMImporterListElement *element1;
          mmoDICOMImporterListElement *element2;
          element1 = (mmoDICOMImporterListElement *)m_ListSelected->Item(0)->GetData();
          element2 = (mmoDICOMImporterListElement *)m_ListSelected->Item(1)->GetData();
          item1_pos[0] = element1->GetCoordinate(0);
          item1_pos[1] = element1->GetCoordinate(1);
          item1_pos[2] = element1->GetCoordinate(2);
          item2_pos[0] = element2->GetCoordinate(0);
          item2_pos[1] = element2->GetCoordinate(1);
          item2_pos[2] = element2->GetCoordinate(2);
          d[0] = fabs(item1_pos[0] - item2_pos[0]);
          d[1] = fabs(item1_pos[1] - item2_pos[1]);
          d[2] = fabs(item1_pos[2] - item2_pos[2]);
          if(d[0] > d[1] && d[0] > d[2])
            m_SortAxes = 0;
          else if(d[1] > d[0] && d[1] > d[2])
            m_SortAxes = 1;
          else if(d[2] > d[0] && d[2] > d[1])
            m_SortAxes = 2;
        }
        switch (m_SortAxes)
        {
        case 0:
          m_ListSelected->Sort(compareX);
          break;
        case 1:
          m_ListSelected->Sort(compareY);
          break;
        case 2:
          m_ListSelected->Sort(compareZ);
          break;
        }
        m_NumberOfSlices = m_ListSelected->Number();
        // reset the current slice number to view the first slice
        m_CurrentSlice = 0;
        m_CropFlag = false;
        // show the selected slice
        ShowSlice(m_CurrentSlice);

        m_RWI->CameraReset();
        ResetSliders();
        m_RWI->CameraUpdate();

        //modified by STEFY 9-7-2003(begin)
        ImportDicomTags();
        mafTagItem *patient_name;
        mafTagItem *patient_id;

        const char* p_name;
        double p_id = 0;

        bool position = m_TagArray->IsTagPresent("PatientName");
        if (position)
        {
          patient_name = m_TagArray->GetTag("PatientName");
          p_name = patient_name->GetValue();
        }
        else 
          p_name = NULL;

        position = m_TagArray->IsTagPresent("PatientID");
        if (position)
        {
          patient_id = m_TagArray->GetTag("PatientID");
          p_id = patient_id->GetValueAsDouble();
          m_Identifier = p_id;
        }
        if (p_name)
          m_PatientName = p_name;

        int tmp = m_PatientName.FindChr('^');
        if(tmp != -1)
          m_PatientName[tmp] = ' ';

        m_Gui->Update();
      }
      break;
      case ID_SCAN_SLICE:
        // show the current slice
        ShowSlice(m_CurrentSlice);
        m_RWI->CameraUpdate();
      break;
      case ID_CROP_MODE_BUTTON:
        m_CropMode = true;		
        m_Gui->Enable(ID_CROP_BUTTON,1);
        if(m_CropFlag)
          m_Gui->Enable(ID_UNDO_CROP_BUTTON,1);
        else
          m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
        m_Gui->Enable(ID_BUILD_STEP,0);
        m_Gui->Enable(ID_BUILD_BUTTON,0);
        m_RWI->CameraUpdate();
      break;
      case ID_CROP_BUTTON:
        m_CropFlag = true;
        ShowSlice(m_CurrentSlice);
        m_RWI->CameraUpdate();
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,1);
      break;
      case ID_UNDO_CROP_BUTTON:
        m_CropFlag = false;
        ShowSlice(m_CurrentSlice);
        m_RWI->CameraUpdate();
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
      break;
      case ID_BUILDVOLUME_MODE_BUTTON:
        m_CropMode = false;
        m_CropActor->VisibilityOff();
        m_RWI->CameraUpdate();
        m_Gui->Enable(ID_CROP_BUTTON,0);
        m_Gui->Enable(ID_UNDO_CROP_BUTTON,0);
        m_Gui->Enable(ID_BUILD_STEP,1);
        m_Gui->Enable(ID_BUILD_BUTTON,1);
      break;
      case ID_BUILD_BUTTON:
        BuildVolume();
        m_DicomDialog->EndModal(wxID_OK);
        //OpStop(OP_RUN_OK); 
      break;	
      case MOUSE_DOWN:
      {
        if (m_CropMode == true)
        {
          //long handle_id = e->GetArg();
          double pos[3];
          vtkPoints *p = (vtkPoints *)e->GetVtkObj();
          p->GetPoint(0,pos);

          //calcola altezza rettangolo
          double b[6];
          m_CropPlane->GetOutput()->GetBounds(b);
          double dx = (b[1] - b[0]) / 10;
          double dy = (b[3] - b[2]) / 10;

          double O[3], P1[3], P2[3];
          m_CropPlane->GetOrigin(O);
          m_CropPlane->GetPoint1(P1);
          m_CropPlane->GetPoint2(P2);

          if (m_GizmoStatus == GIZMO_NOT_EXIST)
          {
            m_GizmoStatus = GIZMO_RESIZING;
            m_CropActor->VisibilityOn();

            pos[2] = 0;
            m_CropPlane->SetOrigin(pos);
            m_CropPlane->SetPoint1(pos[0], pos[1], pos[2]);
            m_CropPlane->SetPoint2(pos[0], pos[1], pos[2]);
          }
          else if (m_GizmoStatus == GIZMO_DONE)
          {
            //	  6------------5----------4--->x
            //		|												|
            //		7												3
            //		|												|
            //		8------------1----------2
            //		|
            //	  v y

            if (P1[0] + dx/2 <= pos[0] &&  pos[0] <= P2[0] - dx/2 &&
              P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
            {
              m_SideToBeDragged = 1;
            }
            else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
              P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
            {
              m_SideToBeDragged = 2;
            }
            else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
              P2[1] + dy/2 <= pos[1] && pos[1] <= P1[1] - dy/2)
            {
              m_SideToBeDragged = 3;
            }
            else if (P2[0] - dx/2 <= pos[0] && pos[0] <= P2[0] + dx/2 &&
              P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] + dy/2)
            {
              m_SideToBeDragged = 4;
            }
            else if (P1[0] + dx/2 <= pos[0] && pos[0] <= P2[0] - dx/2 &&
              P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] + dy/2)
            {
              m_SideToBeDragged = 5;
            }
            else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
              P2[1] - dy/2 <= pos[1] && pos[1] <= P2[1] +dy/2)
            {
              m_SideToBeDragged = 6;
            }
            else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
              P2[1] + dy/2 <= pos[1] && pos[1] <= P1[1] - dy/2)
            {
              m_SideToBeDragged = 7;
            }
            else if (P1[0] - dx/2 <= pos[0] && pos[0] <= P1[0] + dx/2 &&
              P1[1] - dy/2 <= pos[1] && pos[1] <= P1[1] + dy/2)
            {
              m_SideToBeDragged = 8;
            }	
            else
              //hai pickato in un punto che non corrisponde a nessun lato
              // => crea un nuovo gizmo
            {
              m_GizmoStatus = GIZMO_RESIZING;
              m_CropActor->VisibilityOn();

              pos[2] = 0;
              m_CropPlane->SetOrigin(pos);
              m_CropPlane->SetPoint1(pos[0], pos[1], pos[2]);
              m_CropPlane->SetPoint2(pos[0], pos[1], pos[2]);
            }
          }
          m_RWI->CameraUpdate();
        }
      }
      break;
      case MOUSE_MOVE:  //ridimensiona il gizmo
      {
        //long handle_id = e->GetArg();
        double pos[3], oldO[3], oldP1[3], oldP2[3];
        vtkPoints *p = (vtkPoints *)e->GetVtkObj();
        p->GetPoint(0,pos);

        m_CropPlane->GetOrigin(oldO);
        m_CropPlane->GetPoint1(oldP1);
        m_CropPlane->GetPoint2(oldP2);

        if (m_GizmoStatus == GIZMO_RESIZING)
        {
          m_CropPlane->SetPoint1(oldO[0], pos[1], oldP1[2]);
          m_CropPlane->SetPoint2(pos[0], oldO[1], oldP1[2]);
        }
        else if (m_GizmoStatus == GIZMO_DONE)
        {
          if (m_SideToBeDragged == 0)
          {
          }
          else if (m_SideToBeDragged == 1)
            m_CropPlane->SetPoint1(oldP1[0], pos[1], oldP1[2]);
          else if (m_SideToBeDragged == 2)
          {
            m_CropPlane->SetPoint1(oldP1[0], pos[1], oldP1[2]);
            m_CropPlane->SetPoint2(pos[0], oldP2[1], oldP2[2]);
          }
          else if (m_SideToBeDragged == 3)
            m_CropPlane->SetPoint2(pos[0], oldP2[1], oldP2[2]);
          else if (m_SideToBeDragged == 4)
          {
            m_CropPlane->SetOrigin(oldO[0], pos[1], oldO[2]);
            m_CropPlane->SetPoint2(pos[0], pos[1], oldP2[2]);
          }
          else if (m_SideToBeDragged == 5)
          {
            m_CropPlane->SetOrigin(oldO[0], pos[1], oldO[2]);
            m_CropPlane->SetPoint2(oldP2[0], pos[1], oldP2[2]);
          }
          else if (m_SideToBeDragged == 6)
          {
            m_CropPlane->SetOrigin(pos[0], pos[1], oldO[2]);
            m_CropPlane->SetPoint1(pos[0], oldP1[1], oldP2[2]);
            m_CropPlane->SetPoint2(oldP2[0], pos[1], oldP2[2]);
          }
          else if (m_SideToBeDragged == 7)
          {
            m_CropPlane->SetOrigin(pos[0], oldO[1], oldO[2]);
            m_CropPlane->SetPoint1(pos[0], oldP1[1], oldP1[2]);
          }
          else if (m_SideToBeDragged == 8)
          {
            m_CropPlane->SetOrigin(pos[0], oldO[1], oldO[2]);
            m_CropPlane->SetPoint1(pos[0], pos[1], oldP1[2]);
          }
        }
        m_RWI->CameraUpdate();
      }
      break;
      case MOUSE_UP:  //blocca il gizmo
        if (m_CropMode == true)
          if (m_GizmoStatus == GIZMO_RESIZING)
            m_GizmoStatus = 	GIZMO_DONE;
          else if (m_GizmoStatus == GIZMO_DONE)
            m_SideToBeDragged = 0;
      break;
      default:
        mafEventMacro(*e);
      break;
    }	
  }
}


