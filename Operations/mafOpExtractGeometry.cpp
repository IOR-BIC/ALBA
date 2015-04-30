/*=========================================================================

 Program: MAF2
 Module: mafOpExtractGeometry
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafOpExtractGeometry.h"

#include "wx/busyinfo.h"

#include "mafGUI.h"
#include "mafGUIDialog.h"
#include "mafGUIFloatSlider.h"
#include "mafGUIValidator.h"
#include "mafNode.h"
#include "mafTagArray.h"
#include "mafVMESurface.h"
#include "mafVMEVolumeGray.h"

#include "medOpVolumeResample.h"

#include "vtkStructuredPoints.h"
#include "vtkCleanPolyData.h"
#include "vtkDecimatePro.h"
#include "vtkMAFFixTopology.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkMAFVolumeToClosedSmoothSurface.h"
#include "vtkMAFSmartPointer.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkRectilinearGrid.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkTriangleFilter.h"
#include "vtkUnsignedCharArray.h"

#include "itkImage.h"
#include "itkImageToVTKImageFilter.h"
#include "itkVTKImageToImageFilter.h"
#include "itkBinomialBlurImageFilter.h"


typedef  itk::Image< unsigned char, 3> UCharImage;

#define SPACING_PERCENTAGE_BOUNDS 0.1

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpExtractGeometry);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpExtractGeometry::mafOpExtractGeometry(const wxString &label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType	= OPTYPE_OP;
  m_Canundo	= true;

  m_VolumeSmoothing = 0;
  m_Connectivity = 1;
  m_CleanSurface = 1;
  m_SmoothSurface = 1;
  m_DecimateSurface = 0;

  m_DecimateReductionRate = 50;
  m_DecimatePreserveTopology = 0;

  m_VolumeSmoothingRepetitions = 1;
  m_AutoSurfaceContourValue = 1;
  m_SurfaceContourValue = 0;
  m_SmoothSurfaceIterationsNumber = 50;

  m_ProcessingType=1;

  m_VolumeInput = NULL;
  m_ResampledVolume = NULL;
  m_SurfaceOutput = NULL;
  m_OriginalData = NULL;
  m_SurfaceData = NULL;

  m_SurfaceExtractor = NULL;

  m_ScalarRange[0] = m_ScalarRange[1] = 0.0;
  m_VolumeSpacing[0] = m_VolumeSpacing[1] = m_VolumeSpacing[2] = 0;

  m_ResampleGui = NULL;
  m_ExtractSurfaceGui = NULL;

  m_SurfaceContourValueSlider = NULL;
}
//----------------------------------------------------------------------------
mafOpExtractGeometry::~mafOpExtractGeometry()
//----------------------------------------------------------------------------
{
  mafDEL(m_SurfaceOutput);
  if(m_ResampledVolume)
    mafDEL(m_ResampledVolume);
  vtkDEL(m_SurfaceExtractor);
}
//----------------------------------------------------------------------------
bool mafOpExtractGeometry::Accept(mafNode *node)
//----------------------------------------------------------------------------
{
  return ( node != NULL && node->IsA("mafVMEVolumeGray") );
}
//----------------------------------------------------------------------------
mafOp *mafOpExtractGeometry::Copy()   
//----------------------------------------------------------------------------
{
  return (new mafOpExtractGeometry(m_Label));
}
//----------------------------------------------------------------------------
void mafOpExtractGeometry::OpRun()   
//----------------------------------------------------------------------------
{
  m_VolumeInput = mafVMEVolumeGray::SafeDownCast(m_Input);

  vtkImageData *imageData = NULL;
  if ( m_VolumeInput->GetOutput()->GetVTKData()->IsA("vtkImageData") )
    imageData = (vtkImageData *)(m_VolumeInput->GetOutput()->GetVTKData());
  
  m_VolumeInput->Update();

  if(imageData)
  {
    m_OriginalData = (vtkImageData *)m_VolumeInput->GetOutput()->GetVTKData();

    m_OriginalData->GetScalarRange(m_ScalarRange);
    m_SurfaceContourValue = m_ScalarRange[1];
  }
 
  CreateGui();
}
//----------------------------------------------------------------------------
void mafOpExtractGeometry::CreateGui()
//----------------------------------------------------------------------------
{
  // interface:
  m_Gui = new mafGUI(this);

  if(m_VolumeInput->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
  {
    CreateResampleGui();
    m_Gui->AddGui(m_ResampleGui);
  }

  CreateExtractSurfaceGui();
  m_Gui->AddGui(m_ExtractSurfaceGui);

  //m_Gui->OkCancel();
  m_Gui->TwoButtons(ID_CANCEL, ID_OK, "Cancel", "Ok");

  if(m_VolumeInput->GetOutput()->GetVTKData()->IsA("vtkRectilinearGrid"))
  {
    m_Gui->Enable(ID_OK, false);
  } 

  ShowGui();
}

//----------------------------------------------------------------------------
void mafOpExtractGeometry::CreateExtractSurfaceGui()
//----------------------------------------------------------------------------
{
  m_ExtractSurfaceGui = new mafGUI(this);

  m_ExtractSurfaceGui->Label(mafString("Pre-processing volume"), true);
  m_ExtractSurfaceGui->Bool(ID_VOLUME_SMOOTHING, "Volume Smoothing", &m_VolumeSmoothing, 1);
  m_ExtractSurfaceGui->Slider(ID_VOLUME_SMOOTHING_REPETITIONS, wxString("Iterations"), &m_VolumeSmoothingRepetitions, 1, 5);
  m_ExtractSurfaceGui->Enable(ID_VOLUME_SMOOTHING_REPETITIONS, m_VolumeSmoothing>0);
  m_ExtractSurfaceGui->Divider();

  m_ExtractSurfaceGui->Bool(ID_AUTO_CONTOUR_VALUE, _("Auto contour value"), &m_AutoSurfaceContourValue, 1);
  m_SurfaceContourValueSlider = m_ExtractSurfaceGui->FloatSlider(ID_CONTOUR_VALUE, _("Contour value"), &m_SurfaceContourValue, m_ScalarRange[0], m_ScalarRange[1]);
  m_ExtractSurfaceGui->Enable(ID_CONTOUR_VALUE, m_AutoSurfaceContourValue<=0);

  //////////////////////////////////////////////////////////////////////////
  // Surface Processing
  //////////////////////////////////////////////////////////////////////////
  m_ExtractSurfaceGui->Label(mafString("Filtering Method"), true);
  m_ExtractSurfaceGui->Divider();

  wxString processingType[3] = {"Poisson", "Taubin and Fill Holes","Taubin"};
  m_ExtractSurfaceGui->Combo(ID_PROCESSING_TYPE, "", &m_ProcessingType, 3, processingType);


  //////////////////////////////////////////////////////////////////////////
  // Surface optimization
  //////////////////////////////////////////////////////////////////////////
  m_ExtractSurfaceGui->Label(mafString("Surface Optimization"), true);
  m_ExtractSurfaceGui->Divider();

  m_ExtractSurfaceGui->Bool(ID_CONNECTIVITY, _("Connectivity"), &m_Connectivity, 1);

  m_ExtractSurfaceGui->Bool(ID_CLEAN_SURFACE, _("Clean surface"), &m_CleanSurface, 1);

  m_ExtractSurfaceGui->Bool(ID_SMOOTH_SURFACE, _("Smooth Surface"), &m_SmoothSurface, 1);
  m_ExtractSurfaceGui->Slider(ID_SMOOTH_SURFACE_ITERATIONS, _("Iterations"), &m_SmoothSurfaceIterationsNumber, 25, 75);

  CreateSurfaceDecimationGui();
  //////////////////////////////////////////////////////////////////////////

}

//----------------------------------------------------------------------------
void mafOpExtractGeometry::CreateSurfaceDecimationGui()
//----------------------------------------------------------------------------
{
  if(!m_ExtractSurfaceGui)
    return;

  wxStaticBoxSizer *boxsizer = new wxStaticBoxSizer(wxVERTICAL, m_ExtractSurfaceGui, "Surface Decimation");

  int w_id = m_ExtractSurfaceGui->GetWidgetId(ID_DECIMATE_SURFACE);

  //////////////////////////////////////////////////////////////////////////
  //BOOL

  wxCheckBox *check = new wxCheckBox(m_ExtractSurfaceGui, w_id, _("On/Off"), wxDefaultPosition,
    wxSize(-1,20), 0 );
  check->SetValidator( mafGUIValidator(m_ExtractSurfaceGui,w_id,check,&m_DecimateSurface) );
  boxsizer->Add(check,0,wxALL, 1);
  //////////////////////////////////////////////////////////////////////////


  w_id = m_ExtractSurfaceGui->GetWidgetId(ID_DECIMATE_SURFACE_RATE);

  wxStaticText *lab = new wxStaticText(m_ExtractSurfaceGui, w_id, "Reduction rate", wxDefaultPosition, 
    wxDefaultSize, wxALIGN_RIGHT | wxST_NO_AUTORESIZE );
  wxTextCtrl  *text = new wxTextCtrl  (m_ExtractSurfaceGui, w_id, ""   , wxDefaultPosition,
    wxSize(60, 18), wxSUNKEN_BORDER   );
  text->SetValidator( mafGUIValidator(m_Gui,w_id,text,&m_DecimateReductionRate,0,100) );
  wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
  sizer->Add( text, 0, wxRIGHT, 5);
  sizer->Add( lab,  0, wxRIGHT, 2);
  boxsizer->Add(sizer, 0, wxALL, 1);


  w_id = m_ExtractSurfaceGui->GetWidgetId(ID_DECIMATE_SURFACE_TOPOLOGY);
  wxCheckBox *checkTopology = new wxCheckBox(m_ExtractSurfaceGui, w_id, _("Preserve Topology"), wxDefaultPosition,
    wxSize(-1,20), 0 );
  checkTopology->SetValidator( mafGUIValidator(m_ExtractSurfaceGui,w_id,checkTopology,&m_DecimatePreserveTopology) );
  boxsizer->Add(checkTopology,0,wxALL, 1);


  m_ExtractSurfaceGui->Enable(ID_DECIMATE_SURFACE_RATE, false);
  m_ExtractSurfaceGui->Enable(ID_DECIMATE_SURFACE_TOPOLOGY, false);


  m_ExtractSurfaceGui->Add(boxsizer);

}

//----------------------------------------------------------------------------
void mafOpExtractGeometry::CreateResampleGui()
//----------------------------------------------------------------------------
{
  m_ResampleGui = new mafGUI(this);

  m_ResampleGui->Label( _("Resample volume"), true );
  m_ResampleGui->Label( _("Volume Spacing") ,false );

  medOpVolumeResample *op = new medOpVolumeResample();
  op->SetInput(m_VolumeInput);
  op->TestModeOn();
  op->AutoSpacing();
  op->GetSpacing(m_VolumeSpacing);
  mafDEL(op);

  m_ResampleGui->Vector(ID_RESAMPLE_VOLUME_SPACING, "", this->m_VolumeSpacing,MINFLOAT,MAXFLOAT,4,"output volume spacing");

  m_ResampleGui->Button(ID_RESAMPLE_OK, _("Resample"), "" );

  m_ResampleGui->Divider(1);

}

//----------------------------------------------------------------------------
void mafOpExtractGeometry::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_RESAMPLE_OK:
      {
        if( Resample() == MAF_OK)
        {
          m_OriginalData = (vtkImageData *)m_VolumeInput->GetOutput()->GetVTKData();

          m_OriginalData->GetScalarRange(m_ScalarRange);
          m_SurfaceContourValue = m_ScalarRange[1];

          m_ResampleGui->Show(false);

          m_SurfaceContourValueSlider->SetMax(m_ScalarRange[1]);
          m_SurfaceContourValueSlider->SetMin(m_ScalarRange[0]);

          m_ExtractSurfaceGui->Update();
          m_Gui->Enable(ID_OK, true);

          m_Gui->Update();

        }
        break;
      }
    case ID_VOLUME_SMOOTHING:
      {
        m_ExtractSurfaceGui->Enable(ID_VOLUME_SMOOTHING_REPETITIONS, m_VolumeSmoothing>0);
        break;
      }
    case ID_AUTO_CONTOUR_VALUE:
      {
        m_ExtractSurfaceGui->Enable(ID_CONTOUR_VALUE, m_AutoSurfaceContourValue<=0);
      }
    case ID_SMOOTH_SURFACE:
      {
        m_ExtractSurfaceGui->Enable(ID_SMOOTH_SURFACE_ITERATIONS, m_SmoothSurface>0);
        break;
      }
    case ID_DECIMATE_SURFACE:
      {
        m_ExtractSurfaceGui->Enable(ID_DECIMATE_SURFACE_RATE, m_DecimateSurface>0);
        m_ExtractSurfaceGui->Enable(ID_DECIMATE_SURFACE_TOPOLOGY, m_DecimateSurface>0);
        break;
      }
    //case wxOK:
    case ID_OK:
      {
        int result = GenerateIsosurface();
        OpStop(result);  
        //OpStop(OP_RUN_OK);
      }
      break;
    //case wxCANCEL:
    case ID_CANCEL:
      OpStop(OP_RUN_CANCEL);        
      break;
    }
  }
}
//----------------------------------------------------------------------------
void mafOpExtractGeometry::OpStop(int result)
//----------------------------------------------------------------------------
{
  if(m_Gui)
    HideGui();
  mafEventMacro(mafEvent(this,result));  
}
//----------------------------------------------------------------------------
void mafOpExtractGeometry::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_SurfaceOutput != NULL)
  {
    m_SurfaceOutput->ReparentTo(NULL);
  }
}
//----------------------------------------------------------------------------
void mafOpExtractGeometry::OpDo()
//----------------------------------------------------------------------------
{
  m_VolumeInput->ReparentTo(m_Input->GetParent());

  if (m_SurfaceOutput != NULL)
  {
    m_SurfaceOutput->ReparentTo(m_Input);
  }
}
//----------------------------------------------------------------------------
void mafOpExtractGeometry::VolumeSmoothing()
//----------------------------------------------------------------------------
{
  //////////////////////////////////////////////////////////////////////////
  // Image smoothing filter
  //////////////////////////////////////////////////////////////////////////
  typedef itk::BinomialBlurImageFilter<UCharImage, UCharImage> ITKBinomialBlurImageFilter;
  ITKBinomialBlurImageFilter::Pointer smoothingFilter = ITKBinomialBlurImageFilter::New();
  //////////////////////////////////////////////////////////////////////////

  vtkDataArray *originalScalars = m_OriginalData->GetPointData()->GetScalars();

  int dim[3];
  double spacing[3];
  m_OriginalData->GetDimensions(dim);
  m_OriginalData->GetSpacing(spacing);


  //////////////////////////////////////////////////////////////////////////
  // scalars to replace the original ones.
  //////////////////////////////////////////////////////////////////////////

  vtkMAFSmartPointer<vtkUnsignedCharArray> smoothedVolumeScalars;
  smoothedVolumeScalars->SetName("SCALARS");
  smoothedVolumeScalars->SetNumberOfTuples(originalScalars->GetNumberOfTuples());
  //////////////////////////////////////////////////////////////////////////


 
  //////////////////////////////////////////////////////////////////////////
  // Iteration to process every single slice scalars
  //////////////////////////////////////////////////////////////////////////

  vtkMAFSmartPointer<vtkUnsignedCharArray> sliceScalars;
  sliceScalars->SetName("SCALARS");
  sliceScalars->SetNumberOfTuples(dim[0]*dim[1]);

  for (int i= 0;i<dim[2];i++)
  {
    for (int k=0;k<(dim[0]*dim[1]);k++)
    {
      unsigned char value = originalScalars->GetTuple1(k+i*(dim[0]*dim[1]));
      sliceScalars->SetTuple1(k,value);
    }

    vtkMAFSmartPointer<vtkImageData> im;
    im->SetDimensions(dim[0],dim[1],1);
    im->SetSpacing(spacing[0],spacing[1],0.0);
    im->GetPointData()->AddArray(sliceScalars);
    im->GetPointData()->SetActiveScalars("SCALARS");
    im->SetScalarTypeToUnsignedChar();
    im->Update();

    vtkMAFSmartPointer<vtkImageData> filteredImage;

    vtkMAFSmartPointer<vtkImageCast> vtkImageToUnsignedChar;
    vtkImageToUnsignedChar->SetOutputScalarTypeToUnsignedChar();
    vtkImageToUnsignedChar->SetInput(im);
    vtkImageToUnsignedChar->Modified();
    vtkImageToUnsignedChar->Update();

    typedef itk::VTKImageToImageFilter< UCharImage > ConvertervtkTOitk;
    ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
    vtkTOitk->SetInput( vtkImageToUnsignedChar->GetOutput() );
    vtkTOitk->Update();

    smoothingFilter->SetRepetitions(m_VolumeSmoothingRepetitions);
    smoothingFilter->SetInput( ((UCharImage*)vtkTOitk->GetOutput()) );

    try
    {
      smoothingFilter->Update();
    }
    catch ( itk::ExceptionObject &err )
    {
      std::cout << "ExceptionObject caught !" << std::endl; 
      std::cout << err << std::endl; 
    }

    typedef itk::ImageToVTKImageFilter< UCharImage > ConverteritkTOvtk;
    ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
    itkTOvtk->SetInput( smoothingFilter->GetOutput() ); 

    filteredImage = ((vtkImageData*)itkTOvtk->GetOutput());
    filteredImage->Update();

    vtkDataArray *binaryScalars = filteredImage->GetPointData()->GetScalars();


    for (int k=0;k<filteredImage->GetNumberOfPoints();k++)
    {
      unsigned char value = binaryScalars->GetTuple1(k);
      smoothedVolumeScalars->SetTuple1(k+i*(dim[0]*dim[1]),value);
    }
  }
  //////////////////////////////////////////////////////////////////////////

  vtkMAFSmartPointer<vtkImageData> newImageData;
  newImageData->CopyStructure(m_OriginalData);
  newImageData->Update();
  newImageData->GetPointData()->AddArray(smoothedVolumeScalars);
  newImageData->GetPointData()->SetActiveScalars("SCALARS");
  newImageData->SetScalarTypeToUnsignedChar();
  newImageData->Update();

  m_ResampledVolume->SetData(newImageData,mafVME::SafeDownCast(m_ResampledVolume)->GetTimeStamp());
}

//----------------------------------------------------------------------------
void mafOpExtractGeometry::SurfaceCleaning()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkCleanPolyData>clearFilter;

  clearFilter->SetInput(m_SurfaceData);
  clearFilter->ConvertLinesToPointsOff();
  clearFilter->ConvertPolysToLinesOff();
  clearFilter->ConvertStripsToPolysOff();
  clearFilter->Update();
  m_SurfaceData->DeepCopy(clearFilter->GetOutput());

}

//----------------------------------------------------------------------------
void mafOpExtractGeometry::SurfaceDecimation()
//----------------------------------------------------------------------------
{
  // triangle
  vtkMAFSmartPointer<vtkTriangleFilter> triangleFilter;
  triangleFilter->SetInput(m_SurfaceData);
  triangleFilter->Update();
  m_SurfaceData->DeepCopy(triangleFilter->GetOutput());

  //decimate
  vtkMAFSmartPointer<vtkDecimatePro> decimate;
  decimate->SetInput(m_SurfaceData);
  decimate->SetPreserveTopology(1); 
  int m_Reduction = 50;
  decimate->SetTargetReduction(m_Reduction/100.0);
  decimate->Update();

  m_SurfaceData->DeepCopy(decimate->GetOutput());


}

//----------------------------------------------------------------------------
void mafOpExtractGeometry::SurfaceSmoothing()
//----------------------------------------------------------------------------
{

  vtkMAFSmartPointer<vtkSmoothPolyDataFilter> smoothFilter;
  smoothFilter->SetInput(m_SurfaceData);
  smoothFilter->SetNumberOfIterations(m_SmoothSurfaceIterationsNumber);
  smoothFilter->FeatureEdgeSmoothingOn();
  smoothFilter->Update();

  m_SurfaceData->DeepCopy(smoothFilter->GetOutput());
}

//----------------------------------------------------------------------------
void mafOpExtractGeometry::SurfaceConnectivity()
//----------------------------------------------------------------------------
{
  vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
  connectivityFilter->SetInput(m_SurfaceData);
  connectivityFilter->Update();

  m_SurfaceData->DeepCopy((vtkPolyData*)(connectivityFilter->GetOutput()));

}

//----------------------------------------------------------------------------
int mafOpExtractGeometry::GenerateIsosurface()
//----------------------------------------------------------------------------
{
  if(m_VolumeSmoothing)
  {
    // smoothing volume with some itk filters

    mafString smoothedVolumeName = "smoothed_";
    smoothedVolumeName += m_Input->GetName();

    m_ResampledVolume = (mafVMEVolumeGray *)m_VolumeInput->NewInstance();
    m_ResampledVolume->Register(m_ResampledVolume);
    m_ResampledVolume->GetTagArray()->DeepCopy(m_VolumeInput->GetTagArray());

    mafTagItem *ti = NULL;
    ti = m_ResampledVolume->GetTagArray()->GetTag("VME_NATURE");
    if(ti)
    {
      ti->SetValue("SYNTHETIC");
    }
    else
    {
      mafTagItem tag_Nature;
      tag_Nature.SetName("VME_NATURE");
      tag_Nature.SetValue("SYNTHETIC");

      m_ResampledVolume->GetTagArray()->SetTag(tag_Nature);
    }

    m_ResampledVolume->SetName(smoothedVolumeName);
    m_ResampledVolume->ReparentTo(m_VolumeInput->GetParent());
    m_ResampledVolume->ReparentTo(m_VolumeInput);


    VolumeSmoothing();

    m_ResampledVolume->Update();
    m_OriginalData = vtkImageData::SafeDownCast(mafVMEVolumeGray::SafeDownCast(m_ResampledVolume)->GetOutput()->GetVTKData());
    m_OriginalData->Update();

    m_VolumeInput = m_ResampledVolume;
    m_VolumeInput->Update();
  }

  //////////////////////////////////////////////////////////////////////////
  // generate isosurface with vtkMAFContourVolumeMapper + vtkFixTopology
  //////////////////////////////////////////////////////////////////////////

  // VTKmafContourVolumeMapper

  m_SurfaceExtractor = vtkMAFVolumeToClosedSmoothSurface::New();
  m_SurfaceExtractor->SetInput(m_OriginalData);
  m_SurfaceExtractor->AutoLODRenderOn();
  m_SurfaceExtractor->AutoLODCreateOn();

  m_SurfaceExtractor->SetEnableContourAnalysis(0);

  m_SurfaceExtractor->SetFillHoles(m_ProcessingType==1);
  

  // IMPORTANT, extract the isosurface from m_ContourVolumeMapper in this way
  // and then call surface->Delete() when the VME is created

  if(m_AutoSurfaceContourValue<0)
  {
    float value = 0.5f * (m_ScalarRange[0] + m_ScalarRange[1]);
    while (value < m_ScalarRange[1] && m_SurfaceExtractor->EstimateRelevantVolume(value) > 0.3f)
      value += 0.05f * (m_ScalarRange[1] + m_ScalarRange[0]) + 1.f;

    m_SurfaceContourValue = value;

  }
  m_SurfaceExtractor->SetContourValue(m_SurfaceContourValue);

  m_SurfaceExtractor->Update();
  
  m_SurfaceData = m_SurfaceExtractor->GetOutput();

  if(m_Connectivity)
    SurfaceConnectivity();
  m_SurfaceData->Update();

  wxBusyInfo wait(_("Extracting Isosurface: please wait ..."));

  if (m_ProcessingType==0)
  {
    vtkMAFSmartPointer<vtkMAFFixTopology> fixTopologyFilter;
    fixTopologyFilter->SetInput(m_SurfaceData);
    fixTopologyFilter->Update();
    m_SurfaceData->DeepCopy(fixTopologyFilter->GetOutput());
  }
  
  

  //////////////////////////////////////////////////////////////////////////

  if(m_CleanSurface)
  {
    // vtkCleanPolyData
    SurfaceCleaning();

    m_SurfaceData->Update();
  }

  if(m_SmoothSurface)
  {
    //vtkSmoothPolyDataFilter
    SurfaceSmoothing();

    m_SurfaceData->Update();
  }

  if(m_DecimateSurface)
  {
    // (vtkTriangleFilter) + vtkDecimate 
    SurfaceDecimation();
    m_SurfaceData->Update();
  }


  mafNEW(m_SurfaceOutput);
  m_SurfaceOutput->SetData(m_SurfaceData,mafVMEVolumeGray::SafeDownCast(m_Input)->GetTimeStamp());

  mafTagItem tag_Nature;
  tag_Nature.SetName("VME_NATURE");
  tag_Nature.SetValue("SYNTHETIC");

  m_SurfaceOutput->GetTagArray()->SetTag(tag_Nature);

  m_SurfaceOutput->SetName("Surface");
  m_SurfaceOutput->ReparentTo(m_Input);
  m_SurfaceOutput->Update();

  m_Output = m_SurfaceOutput;

  m_SurfaceData->Delete();

  return OP_RUN_OK;

}

//----------------------------------------------------------------------------
int mafOpExtractGeometry::Resample()
//----------------------------------------------------------------------------
{
  int answer = wxMessageBox(_("The data will be resampled! Proceed?"),_("Confirm"), wxYES_NO|wxICON_EXCLAMATION , NULL);
  
  if(answer == wxNO)
  {
    OpStop(OP_RUN_CANCEL);
    return MAF_ERROR;
  }
  
  wxBusyInfo wait_info1("Resampling...");
  medOpVolumeResample *op = new medOpVolumeResample();
  op->SetInput(m_VolumeInput);
  op->TestModeOn();
  op->OpRun();

  //////////////////////////////////////////////////////////////////////////
  // check if spacing values are too little
  //////////////////////////////////////////////////////////////////////////
  bool checkSpacing = true;
  double m_VolumeBounds[6];

  vtkDataSet *vme_data = ((mafVME *)m_VolumeInput)->GetOutput()->GetVTKData();
  m_VolumeInput->GetOutput()->GetBounds(m_VolumeBounds);

  m_VolumeSpacing[0] = VTK_DOUBLE_MAX;
  m_VolumeSpacing[1] = VTK_DOUBLE_MAX;
  m_VolumeSpacing[2] = VTK_DOUBLE_MAX;

  if (vtkImageData *image = vtkImageData::SafeDownCast(vme_data))
  {
    image->GetSpacing(m_VolumeSpacing);
  }
  else if (vtkRectilinearGrid *rgrid = vtkRectilinearGrid::SafeDownCast(vme_data))
  {
    for (int xi = 1; xi < rgrid->GetXCoordinates()->GetNumberOfTuples (); xi++)
    {
      double spcx = rgrid->GetXCoordinates()->GetTuple1(xi)-rgrid->GetXCoordinates()->GetTuple1(xi-1);
      if (m_VolumeSpacing[0] > spcx && spcx != 0.0)
        m_VolumeSpacing[0] = spcx;
    }

    for (int yi = 1; yi < rgrid->GetYCoordinates()->GetNumberOfTuples (); yi++)
    {
      double spcy = rgrid->GetYCoordinates()->GetTuple1(yi)-rgrid->GetYCoordinates()->GetTuple1(yi-1);
      if (m_VolumeSpacing[1] > spcy && spcy != 0.0)
        m_VolumeSpacing[1] = spcy;
    }

    for (int zi = 1; zi < rgrid->GetZCoordinates()->GetNumberOfTuples (); zi++)
    {
      double spcz = rgrid->GetZCoordinates()->GetTuple1(zi)-rgrid->GetZCoordinates()->GetTuple1(zi-1);
      if (m_VolumeSpacing[2] > spcz && spcz != 0.0)
        m_VolumeSpacing[2] = spcz;
    }
  }

  if ((m_VolumeSpacing[0]/(m_VolumeBounds[1] - m_VolumeBounds[0]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    checkSpacing = false;
  }
  if ((m_VolumeSpacing[1]/(m_VolumeBounds[3] - m_VolumeBounds[2]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    checkSpacing = false;
  }
  if ((m_VolumeSpacing[2]/(m_VolumeBounds[5] - m_VolumeBounds[4]))*100 < SPACING_PERCENTAGE_BOUNDS)
  {
    checkSpacing = false;
  }

  if (!checkSpacing)
  {
    answer = wxMessageBox( "Spacing values are too little and could generate memory problems - Continue?", "Warning", wxYES_NO, NULL);
    if (answer == wxNO)
    {
      return MAF_ERROR;
    }
  }
  //////////////////////////////////////////////////////////////////////////

  op->Resample();
  mafVMEVolumeGray *volOut=mafVMEVolumeGray::SafeDownCast(op->GetOutput());
  volOut->GetOutput()->Update();
  volOut->Update();

  mafDEL(op);

  m_VolumeInput=volOut;

  m_VolumeInput->Update();

  return MAF_OK;

}