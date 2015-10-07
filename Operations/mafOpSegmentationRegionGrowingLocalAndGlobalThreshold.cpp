/*=========================================================================

 Program: MAF2
 Module: mafOpSegmentationRegionGrowingLocalAndGlobalThreshold
 Authors: Matteo Giacomoni
 
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

#include "mafOpSegmentationRegionGrowingLocalAndGlobalThreshold.h"
#include "wx/busyinfo.h"

#include "mafDecl.h"
#include "mafGUI.h"
#include "mafNode.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafGUILutSlider.h"
#include "mafGUIHistogramWidget.h"
#include "mafGUIDialog.h"
#include "medOpVolumeResample.h"
#include "mafDeviceButtonsPad.h"
#include "mafEventInteraction.h"

#include "vtkMath.h"
#include "vtkMAFSmartPointer.h"
#include "vtkMAFRegionGrowingLocalGlobalThreshold.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkStructuredPoints.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkImageMedian3D.h"
#include "vtkImageAccumulate.h"
#include "vtkImageCast.h"
#include "vtkMAFContourVolumeMapper.h"
#include "vtkMAFHistogram.h"

#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkAdaptiveHistogramEqualizationImageFilter.h"
#include "mafProgressBarHelper.h"

const unsigned int Dimension = 3;

typedef unsigned char InputPixelType;
typedef unsigned char OutputPixelType;
typedef float InputPixelTypeFloat;

typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
typedef itk::Image< InputPixelTypeFloat,  Dimension >   InputImageTypeFloat;
typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;

typedef itk::BinaryBallStructuringElement< InputPixelType,Dimension  > StructuringElementType;

typedef itk::BinaryDilateImageFilter< InputImageType,OutputImageType,StructuringElementType >  DilateFilterType;
typedef itk::BinaryErodeImageFilter< InputImageType,OutputImageType,StructuringElementType >  ErodeFilterType;

typedef itk::AdaptiveHistogramEqualizationImageFilter<InputImageTypeFloat> HistogramEqualizationType;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafOpSegmentationRegionGrowingLocalAndGlobalThreshold);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::mafOpSegmentationRegionGrowingLocalAndGlobalThreshold(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_InputPreserving = true;

  m_LowerLabel = -1;
  m_UpperLabel = 1;
  m_SphereRadius = 1;
  m_ApplyConnectivityFilter = FALSE;

  m_VolumeOutputMorpho = NULL;
  m_VolumeOutputRegionGrowing = NULL;
  m_SurfaceOutput = NULL;

  m_SegmentedImage = NULL;
  m_MorphoImage = NULL;

  m_Histogram = NULL;
  m_Dialog = NULL;

  m_ComputedMedianFilter = false;

  m_Point1 = _("Bone Peak ");
  m_Point2 = _("Soft Peak ");
  m_Point3 = _("Bone Base ");
  m_Point4 = _("Soft Base ");

  m_CurrentPoint = 0;

  m_EliminateHistogramValues = TRUE;
  m_ValuesToEliminate = -500;

  m_Threshold = 0.0;
  
}
//----------------------------------------------------------------------------
mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::~mafOpSegmentationRegionGrowingLocalAndGlobalThreshold()
//----------------------------------------------------------------------------
{
  mafDEL(m_VolumeOutputMorpho);
  mafDEL(m_VolumeOutputRegionGrowing);
  mafDEL(m_SurfaceOutput);
  vtkDEL(m_SegmentedImage);
  vtkDEL(m_MorphoImage);

  if (m_ComputedMedianFilter)
  {
    mafDEL(m_VolumeInput);
  }

  cppDEL(m_Dialog);
}
//----------------------------------------------------------------------------
mafOp* mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::Copy()
//----------------------------------------------------------------------------
{
  /** return a copy of itself, needs to put it into the undo stack */
  return new mafOpSegmentationRegionGrowingLocalAndGlobalThreshold(m_Label);
}
//----------------------------------------------------------------------------
bool mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return vme && vme->IsA("mafVMEVolumeGray");
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpRun()
//----------------------------------------------------------------------------
{
  m_VolumeInput = mafVMEVolumeGray::SafeDownCast(m_Input);

  vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  vtkImageData *im = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  if (sp == NULL && im == NULL)
  {
    int answer = wxMessageBox(_("The data will be resampled! Proceed?"),_("Confirm"), wxYES_NO|wxICON_EXCLAMATION , NULL);
    if(answer == wxNO)
    {
      OpStop(OP_RUN_CANCEL);
      return;
    }
    wxBusyInfo wait_info1("Resampling...");
    medOpVolumeResample *op = new medOpVolumeResample();
    op->SetInput(m_VolumeInput);
    op->TestModeOn();
    op->OpRun();
    op->AutoSpacing();
    op->Resample();
    mafVMEVolumeGray *volOut=mafVMEVolumeGray::SafeDownCast(op->GetOutput());
    volOut->GetOutput()->Update();
    volOut->Update();

    mafDEL(op);

    m_VolumeInput=volOut;
  }

  m_VolumeInput->Update();

  int answer = wxMessageBox(_("Would you like to apply median filter to the data?"),_("Confirm"), wxYES_NO|wxICON_EXCLAMATION , NULL);
  if(answer == wxYES)
  {
    m_ComputedMedianFilter = true;

    vtkMAFSmartPointer<vtkImageMedian3D> median;
    median->SetInput(vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData()));
    median->SetKernelSize(3,3,3);
    median->Update();

    mafVMEVolumeGray *volMediano;
    mafNEW(volMediano);
    vtkDataSet *d = median->GetOutput();
    d->Update();
    int k = d->GetNumberOfPoints();
    double sr[2];
    d->GetPointData()->GetScalars()->GetRange(sr);
    mafString name = m_Input->GetName();
    name<<" - Applied Median Filter";
    volMediano->SetName(name);
    vtkMAFSmartPointer<vtkImageToStructuredPoints> f;
    f->SetInput(median->GetOutput());
    f->Update();
    volMediano->SetData(f->GetOutput(),m_VolumeInput->GetTimeStamp());
    volMediano->ReparentTo(m_VolumeInput);
    volMediano->Update();

    m_VolumeInput = volMediano;
  }

  mafNEW(m_VolumeOutputMorpho);
  mafNEW(m_VolumeOutputRegionGrowing);
  mafNEW(m_SurfaceOutput);

  vtkNEW(m_SegmentedImage);
  vtkNEW(m_MorphoImage);
  
  //HistogramEqualization();
  CreateGui();
  CreateHistogramDialog();
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::MorphologicalMathematics()
//----------------------------------------------------------------------------
{
  //Perform the morphological closing operation
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Please wait, morphological mathematics...");
	
	typedef itk::VTKImageToImageFilter< InputImageType > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( m_SegmentedImage );
  vtkTOitk->Update();
  progressHelper.UpdateProgressBar(10);

  //Structuring element is a sphere
  StructuringElementType  structuringElement;
  structuringElement.SetRadius( m_SphereRadius );  // 3x3 structuring element
  structuringElement.CreateStructuringElement(); 

  DilateFilterType::Pointer binaryDilate = DilateFilterType::New();
  binaryDilate->SetKernel( structuringElement );
  binaryDilate->SetInput( vtkTOitk->GetOutput() );
  binaryDilate->SetDilateValue( m_LowerLabel );
  binaryDilate->Update();
  progressHelper.UpdateProgressBar(50);

  ErodeFilterType::Pointer  binaryErode  = ErodeFilterType::New();
  binaryErode->SetKernel(  structuringElement );
  binaryErode->SetInput( binaryDilate->GetOutput() );
  binaryErode->SetErodeValue( m_LowerLabel );
  binaryErode->Update();
  progressHelper.UpdateProgressBar(90);

  typedef itk::ImageToVTKImageFilter< OutputImageType > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  itkTOvtk->SetInput( binaryErode->GetOutput() );
  itkTOvtk->Update();
  progressHelper.UpdateProgressBar(100);

  m_MorphoImage->DeepCopy(itkTOvtk->GetOutput());
  m_MorphoImage->Update();
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::RegionGrowing()
//----------------------------------------------------------------------------
{
	mafProgressBarHelper progressHelper(m_Listener);
	progressHelper.SetTextMode(m_TestMode);
	progressHelper.InitProgressBar("Please wait, region growing...");
	
  //Get the vtk data from the input
  vtkImageData *imageData = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  imageData->Update();

  //Get the thresholds values selected by the user
  //double lower,upper;
  //m_Histogram->GetThresholds(&lower,&upper);
  
  //Apply the region growing filter
  vtkMAFSmartPointer<vtkMAFRegionGrowingLocalGlobalThreshold> localFilter;
  localFilter->SetInput(imageData);
  localFilter->SetLowerLabel(m_LowerLabel);
  localFilter->SetLowerThreshold(m_Threshold);
  localFilter->SetUpperLabel(m_UpperLabel);
  localFilter->SetUpperThreshold(m_Threshold+400);
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,localFilter));
  localFilter->Update();

  //Save the result of the region growing
  m_SegmentedImage->DeepCopy(localFilter->GetOutput());
  m_SegmentedImage->Update();

}
//----------------------------------------------------------------------------
// widget ID's
//----------------------------------------------------------------------------
enum REGION_GROWING_ID
{
  ID_TO_START = MINID,
  ID_SLIDER_LABELS,
  ID_ROLLOUT_LABELS,
  ID_SPHERE_RADIUS,
  ID_APPLY_CONNECTIVITY_FILTER,
  ID_REGION_GROWING,
  ID_MORPHOLOGICAL,
  ID_APPLY_MEDIAN_FILTER,
  ID_DIALOG_HISTOGRAM,
  ID_FITTING,
  ID_DIALOG_OK,
  ID_ELIMINATE_HISTOGRAM_VALUES,
  ID_VALUES_TO_ELIMINATE,
  ID_THRESHOLD,
};
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

//   wxBoxSizer *sizer3 = new wxBoxSizer(wxHORIZONTAL);
//   m_Histogram = new mafGUIHistogramWidget(m_Gui,-1,wxPoint(0,0),wxSize(20,200),wxTAB_TRAVERSAL,true);
//   m_Histogram->SetListener(m_Gui);
//   m_Histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
//   vtkImageData *hd = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
//   hd->Update();
//   m_Histogram->SetData(hd->GetPointData()->GetScalars());
// 
//   sizer3->Add(m_Histogram,wxALIGN_CENTER|wxRIGHT);
//   m_Gui->Add(sizer3,1);

  m_Gui->Button(ID_DIALOG_HISTOGRAM,_("Select points"));
  m_Gui->Label(&m_Point1,false,true);
  m_Gui->Label(&m_Point2,false,true);
  m_Gui->Label(&m_Point3,false,true);
  m_Gui->Label(&m_Point4,false,true);
  m_BoneParam1 = "";
  m_BoneParam2 = "";
  m_BoneParam3 = "";
  m_Gui->Label(&m_BoneParam1,false,true);
  m_Gui->Label(&m_BoneParam2,false,true);
  m_Gui->Label(&m_BoneParam3,false,true);
  m_SoftParam1 = "";
  m_SoftParam2 = "";
  m_SoftParam3 = "";
  m_Gui->Label(&m_SoftParam1,false,true);
  m_Gui->Label(&m_SoftParam2,false,true);
  m_Gui->Label(&m_SoftParam3,false,true);
  m_Gui->Bool(ID_ELIMINATE_HISTOGRAM_VALUES,_("Eliminate Values"),&m_EliminateHistogramValues,1);
  m_Gui->Double(ID_VALUES_TO_ELIMINATE,_(""),&m_ValuesToEliminate);
  m_Gui->Enable(ID_VALUES_TO_ELIMINATE,m_EliminateHistogramValues == TRUE);
  //m_Gui->Button(ID_FITTING,_("Fitting"));
  m_Gui->Divider(1);

  m_Gui->Double(ID_THRESHOLD,_("Threshold"),&m_Threshold);

  m_GuiLabels = new mafGUI(this);
  m_Gui->RollOut(ID_ROLLOUT_LABELS, _("Labels"), m_GuiLabels, false);

  m_LowerLabel = VTK_UNSIGNED_CHAR_MIN;
  m_UpperLabel = VTK_UNSIGNED_CHAR_MAX;

  wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
  m_SliderLabels = new mafGUILutSlider(m_GuiLabels,ID_SLIDER_LABELS ,wxPoint(0,0),wxSize(10,24), 0, "Label");
  m_SliderLabels->SetListener(this);
  m_SliderLabels->SetSize(5,24);
  m_SliderLabels->SetMinSize(wxSize(5,24));
  m_SliderLabels->SetRange(m_LowerLabel,m_UpperLabel);
  m_SliderLabels->SetSubRange(m_LowerLabel,m_UpperLabel);

  sizer2->Add(m_SliderLabels,wxALIGN_CENTER|wxRIGHT);

  m_GuiLabels->Add(sizer2);
  m_GuiLabels->FitGui();

  m_Gui->Divider(1);
  m_Gui->Integer(ID_SPHERE_RADIUS,_("Radius"),&m_SphereRadius);
  m_Gui->Enable(ID_SPHERE_RADIUS,false);
  m_Gui->Divider(1);
  m_Gui->Bool(ID_APPLY_CONNECTIVITY_FILTER,_("Extract biggest region"),&m_ApplyConnectivityFilter,1);
  m_Gui->Enable(ID_APPLY_CONNECTIVITY_FILTER,false);
  m_Gui->Divider(1);
  m_Gui->Button(ID_REGION_GROWING,_("Region growing"));
  m_Gui->Button(ID_MORPHOLOGICAL,_("Morphological closing"));
  m_Gui->Enable(ID_MORPHOLOGICAL,false);
  m_Gui->OkCancel();
  m_Gui->Enable(wxOK,false);
  m_Gui->Divider(1);

  m_Gui->FitGui();
  ShowGui();
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::HistogramEqualization()
//----------------------------------------------------------------------------
{
  vtkImageData *im = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  im->Update();

  vtkMAFSmartPointer<vtkImageCast> vtkImageToFloat;
  vtkImageToFloat->SetOutputScalarTypeToFloat ();
  vtkImageToFloat->SetInput(im);
  vtkImageToFloat->Modified();
  vtkImageToFloat->Update();

  typedef itk::VTKImageToImageFilter< InputImageTypeFloat > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( vtkImageToFloat->GetOutput() );
  vtkTOitk->Update();

  HistogramEqualizationType::Pointer histeqFilter = HistogramEqualizationType::New();

  //provide minimum info
  histeqFilter->GetOutput()->ReleaseDataFlagOn(); 
  HistogramEqualizationType::ImageSizeType radius;
  radius[0] = 100;
  radius[1] = 100;
  radius[2] = 100;
  radius.Fill(0);
  histeqFilter->SetRadius(radius);   
  histeqFilter->SetAlpha(0);
//   histeqFilter->SetBeta(0); 
  histeqFilter->SetInput(vtkTOitk->GetOutput());
  histeqFilter->Update();
  //   rescaleFilter->SetInput(histeqFilter->GetOutput());
  //   rescaleFilter->Update();

  typedef itk::ImageToVTKImageFilter< InputImageTypeFloat > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  itkTOvtk->SetInput( histeqFilter->GetOutput() );
  itkTOvtk->Update();

  vtkImageData *imout;
  vtkNEW(imout);
  imout->DeepCopy(itkTOvtk->GetOutput());
  imout->Update();

  mafVMEVolumeGray *v;
  mafNEW(v);
  v->SetData(imout,0.0);
  v->ReparentTo(m_VolumeInput);
  v->Update();

//   m_VolumeInput->SetData(itkTOvtk->GetOutput(),0.0);
//   m_VolumeInput->Update();
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::CreateHistogramDialog()
//----------------------------------------------------------------------------
{
  m_Dialog = new mafGUIDialog("Histogram", mafCLOSEWINDOW | mafRESIZABLE);

  m_Histogram = new mafGUIHistogramWidget(m_Gui,-1,wxPoint(0,0),wxSize(400,500),wxTAB_TRAVERSAL);
  m_Histogram->SetListener(this);
  m_Histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
  vtkImageData *hd = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  hd->Update();
  m_Histogram->SetData(hd->GetPointData()->GetScalars());

  mafGUI *gui = new mafGUI(this);
  gui->Add(m_Histogram,1);
  gui->AddGui(m_Histogram->GetGui());
  gui->Button(ID_DIALOG_OK,_("OK"));
  gui->FitGui();
  gui->Update();

  m_Dialog->Add(gui,1);
  m_Dialog->SetMinSize(wxSize(600,600));
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::ComputeParam()
//----------------------------------------------------------------------------
{
  double boneParameters[3],softIssueParameters[3];
  double boneMean = (double)m_Point1Value;
  //double boneStDev = (double)abs(m_Point3Value-boneMean)/3;
  //double boneStDev = (double)1/(m_Point1HistogramValue*sqrt(vtkMath::DoublePi()));
  //double softIssueStDev = (double)1/(m_Point2HistogramValue*sqrt(vtkMath::DoublePi()));
  double softIssueMean = (double)m_Point2Value;
  //double softIssueStDev = (double)abs(m_Point4Value-softIssueMean)/3;
  boneParameters[0] = (double)m_Point1HistogramValue;
  boneParameters[1] = (double)boneMean;
  boneParameters[2] = (double)abs(m_Point3Value-boneMean)/3;

  softIssueParameters[0] = (double)m_Point2HistogramValue;
  softIssueParameters[1] = (double)softIssueMean;
  softIssueParameters[2] = (double)abs(m_Point4Value-softIssueMean)/3;

  m_BoneParam1 = "bone " + mafString(boneParameters[0]);
  m_BoneParam2 = "bone " + mafString(boneParameters[1]);
  m_BoneParam3 = "bone " + mafString(boneParameters[2]);

  m_SoftParam1 = "soft " + mafString(softIssueParameters[0]);
  m_SoftParam2 = "soft " + mafString(softIssueParameters[1]);
  m_SoftParam3 = "soft " + mafString(softIssueParameters[2]);

  m_Gui->Update();
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::WriteHistogramFiles()
//----------------------------------------------------------------------------
{
  vtkImageData *hd = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  hd->Update();
  double sr[2];
  hd->GetScalarRange(sr);
  double srw = sr[1]-sr[0];

  vtkMAFSmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(hd->GetPointData()->GetScalars()->GetNumberOfTuples(),1,1);
  imageData->SetScalarType(hd->GetPointData()->GetScalars()->GetDataType());
  imageData->GetPointData()->SetScalars(hd->GetPointData()->GetScalars());
  imageData->Update();
  imageData->GetScalarRange(sr);

  vtkMAFSmartPointer<vtkImageAccumulate> accumulate;
  accumulate->SetInput(imageData);
  accumulate->SetComponentOrigin(sr[0],0,0);  
  accumulate->SetComponentExtent(0,srw,0,0,0,0);
  accumulate->SetComponentSpacing(1,0,0); // bins maps all the Scalars Range
  accumulate->Update();

  wxString newDir = mafGetAppDataDirectory().c_str();
  wxSetWorkingDirectory(newDir);

  double val = accumulate->GetOutput()->GetPointData()->GetScalars()->GetTuple1(178);
  int numOfValues = 0;
  int startIndex = -1;
  int numOfTuples = accumulate->GetOutput()->GetPointData()->GetScalars()->GetNumberOfTuples();
  double step = (double)(srw+1)/numOfTuples;
  if (m_EliminateHistogramValues == TRUE)
  {
    for (int i=0;i<accumulate->GetOutput()->GetPointData()->GetScalars()->GetNumberOfTuples();i++)
    {
      if (((step*i) + sr[0])>m_ValuesToEliminate)
      {
        if (startIndex == -1)
        {
          startIndex = i;
        }
        numOfValues++;
      }
    }
  }
  else
  {
    startIndex = 0;
    numOfValues = accumulate->GetOutput()->GetPointData()->GetScalars()->GetNumberOfTuples();
  }
  double *x = new double[numOfValues];
  double *y = new double[numOfValues];

  ofstream xFile;
  xFile.open("x.txt");
  ofstream yFile;
  yFile.open("y.txt");
  xFile<<"[";
  yFile<<"[";
  for (int j=startIndex, i=0;j<accumulate->GetOutput()->GetPointData()->GetScalars()->GetNumberOfTuples();j++,i++)
  {
    x[i] = floor((step*j) + sr[0]);
    if (x[i] == 0) //&& (i>0 && j+1<accumulate->GetOutput()->GetPointData()->GetScalars()->GetNumberOfTuples()))//To delete a peak in the zero value
    {
      y[i] = (y[i-1] + accumulate->GetOutput()->GetPointData()->GetScalars()->GetTuple1(j+1))/2;
    }
    else
    {
      y[i] = accumulate->GetOutput()->GetPointData()->GetScalars()->GetTuple1(j);
    }
    

    xFile<<x[i];
    yFile<<y[i];
    if (i != numOfValues-1)
    {
      xFile<<",";
      yFile<<",";
    }
  }

  xFile<<"]";
  xFile.close();

  yFile<<"]";
  yFile.close();
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::FittingLM()
//----------------------------------------------------------------------------
{
  double boneParameters[3],softIssueParameters[3];
  double boneStDev = (double)(m_Point1Value-m_Point2Value)/3;
  double boneMean = (double)m_Point1Value;
  double softIssueStDev = (double)(m_Point3Value-m_Point4Value)/3;
  double softIssueMean = (double)m_Point3Value;
  boneParameters[0] = 1/(boneStDev*sqrt(2*vtkMath::Pi()));
  boneParameters[1] = boneMean;
  boneParameters[2] = sqrt(2.0)*boneStDev;

  softIssueParameters[0] = 1/(softIssueStDev*sqrt(2*vtkMath::Pi()));
  softIssueParameters[1] = softIssueMean;
  softIssueParameters[2] = sqrt(2.0)*softIssueStDev;

  vtkImageData *hd = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  hd->Update();
  double sr[2];
  hd->GetScalarRange(sr);
  double srw = sr[1]-sr[0];

  vtkMAFSmartPointer<vtkImageData> imageData;
  imageData->SetDimensions(hd->GetPointData()->GetScalars()->GetNumberOfTuples(),1,1);
  imageData->SetScalarType(hd->GetPointData()->GetScalars()->GetDataType());
  imageData->GetPointData()->SetScalars(hd->GetPointData()->GetScalars());
  imageData->Update();
  imageData->GetScalarRange(sr);

  vtkMAFSmartPointer<vtkImageAccumulate> accumulate;
  accumulate->SetInput(imageData);
  accumulate->SetComponentOrigin(sr[0],0,0);  
  accumulate->SetComponentExtent(0,srw,0,0,0,0);
  accumulate->SetComponentSpacing(1,0,0); // bins maps all the Scalars Range
  accumulate->Update();

  wxString newDir = mafGetAppDataDirectory().c_str();
  wxSetWorkingDirectory(newDir);

  wxString command = "python.exe lm.py";
  command.Append(" ");
  command.Append(wxString::Format("%.3f",boneParameters[0]));
  command.Append(" ");
  command.Append(wxString::Format("%.3f",boneParameters[1]));
  command.Append(" ");
  command.Append(wxString::Format("%.3f",boneParameters[2]));
  
  WriteHistogramFiles();

  mafLogMessage(command.c_str());
  wxExecute(command,wxEXEC_SYNC);

  command = "python.exe lm.py";
  command.Append(" ");
  command.Append(wxString::Format("%.3f",softIssueParameters[0]));
  command.Append(" ");
  command.Append(wxString::Format("%.3f",softIssueParameters[1]));
  command.Append(" ");
  command.Append(wxString::Format("%.3f",softIssueParameters[2]));

  mafLogMessage(command.c_str());
  //wxExecute(command,wxEXEC_SYNC);
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEventInteraction *ei = mafEventInteraction::SafeDownCast(maf_event))
  {
    if (ei->GetId() == mafDeviceButtonsPad::GetButtonDownId() && ei->GetButton() == MAF_LEFT_BUTTON && ei->GetModifier(MAF_CTRL_KEY))
    {
      //         if(m_Histogram->GetInputData() == NULL) return;
      double pos[2];
      ei->Get2DPosition(pos);
      int hisctogramValue = m_Histogram->GetHistogramValue(pos[0],pos[1]);
      double scalar = m_Histogram->GetHistogramScalarValue(pos[0],pos[1]);

      if (m_CurrentPoint % 4 == 0)
      {
        m_Point1 = wxString::Format("Bone Peak : %d",hisctogramValue);
        m_Point1Value = scalar;
        m_Point1HistogramValue = hisctogramValue;
      }
      if (m_CurrentPoint % 4 == 1)
      {
        m_Point2 = wxString::Format("Soft Peak : %d",hisctogramValue);
        m_Point2Value = scalar;
        m_Point2HistogramValue = hisctogramValue;
        //ComputeParam();
        //WriteHistogramFiles();
      }
      if (m_CurrentPoint % 4 == 2)
      {
        m_Point3 = wxString::Format("Bone Base : %.2f",scalar);
        m_Point3Value = scalar;
      }
      if (m_CurrentPoint % 4 == 3)
      {
        m_Point4 = wxString::Format("Soft Base : %.2f",scalar);
        m_Point4Value = scalar;

        ComputeParam();
        WriteHistogramFiles();
      }

      m_CurrentPoint++;

      m_Gui->Update();

      return;
    }
  }

  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_ELIMINATE_HISTOGRAM_VALUES:
      {
        m_Gui->Enable(ID_VALUES_TO_ELIMINATE,m_EliminateHistogramValues == TRUE);
      }
      break;
    case ID_DIALOG_OK:
      {
        m_Dialog->EndModal(wxID_OK);
      }
      break;
    case ID_FITTING:
      {
        FittingLM();
      }
      break;
    case ID_DIALOG_HISTOGRAM:
      {
        m_Dialog->ShowModal();
      }
      break;
    case ID_REGION_GROWING:
      {
        RegionGrowing();

        //Convert the output of the region growing into structured points
        vtkMAFSmartPointer<vtkImageToStructuredPoints> filter;
        filter->SetInput(m_SegmentedImage);
        filter->Update();

        //Generate the vme output of the region growing
        m_VolumeOutputRegionGrowing->SetData(filter->GetOutput(),m_VolumeInput->GetTimeStamp());
        m_VolumeOutputRegionGrowing->SetName(_("Segmentation Output - first step"));
        m_VolumeOutputRegionGrowing->ReparentTo(m_VolumeInput);
        m_VolumeOutputRegionGrowing->Update();

        mafEventMacro(mafEvent(this,VME_SHOW,m_VolumeOutputRegionGrowing,true));

        if (m_SegmentedImage != NULL)
        {
          m_Gui->Enable(ID_SPHERE_RADIUS,true);
          m_Gui->Enable(ID_APPLY_CONNECTIVITY_FILTER,true);
          m_Gui->Enable(ID_MORPHOLOGICAL,true);
          m_Gui->Update();
        }
      }
      break;
    case ID_MORPHOLOGICAL:
      {
        MorphologicalMathematics();

        //Convert the output of the region growing into structured points
        vtkMAFSmartPointer<vtkImageToStructuredPoints> filter;
        filter->SetInput(m_MorphoImage);
        filter->Update();

        //Generate the vme output of the morphological closing
        m_VolumeOutputMorpho->SetData(filter->GetOutput(),m_VolumeInput->GetTimeStamp());
        m_VolumeOutputMorpho->SetName(_("Segmentation Output - second step"));
        m_VolumeOutputMorpho->ReparentTo(m_VolumeInput);
        m_VolumeOutputMorpho->Update();

        vtkMAFSmartPointer<vtkMAFContourVolumeMapper> extractIsosurface;
        extractIsosurface->SetInput(filter->GetOutput());
        extractIsosurface->SetContourValue(m_UpperLabel);
        extractIsosurface->Update();

        vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
        int result = MAF_OK;
        if (m_ApplyConnectivityFilter == TRUE)
        {
          connectivityFilter->SetInput(extractIsosurface->GetOutput());
          connectivityFilter->SetExtractionModeToLargestRegion();
          connectivityFilter->Update();

          result = m_SurfaceOutput->SetData(connectivityFilter->GetOutput(),m_VolumeInput->GetTimeStamp());
        }
        else
        {
          result = m_SurfaceOutput->SetData(extractIsosurface->GetOutput(),m_VolumeInput->GetTimeStamp());
        }

        if (result == MAF_ERROR)
        {
          wxMessageBox("There was an error during extracting the surface!");
          m_Gui->Enable(wxOK,false);
        }
        else
        {
        	m_SurfaceOutput->SetName(_("Segmentation Output - extract isosurface"));
        	m_SurfaceOutput->ReparentTo(m_VolumeInput);
        	m_SurfaceOutput->Update();
  
        	mafEventMacro(mafEvent(this,VME_SHOW,m_VolumeOutputMorpho,true));
          m_Gui->Enable(wxOK,true);
        }


      }
      break;
    case ID_RANGE_MODIFIED:
      {
        if (e->GetSender() == m_SliderLabels)
        {
          //Get the label values modified by the user
          m_SliderLabels->GetSubRange(&m_LowerLabel,&m_UpperLabel);
        }
      }
      break;
    case wxOK:
      {
        this->OpStop(OP_RUN_OK);
        return;
      }
      break;
    case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
        return;
      }
      break;
    default:
      mafEventMacro(*e);
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
	  if (m_VolumeOutputRegionGrowing)
	  {
	    m_VolumeOutputRegionGrowing->ReparentTo(NULL);
	  }
	  if (m_VolumeOutputMorpho)
	  {
	    m_VolumeOutputMorpho->ReparentTo(NULL);
	  }
	  if (m_SurfaceOutput)
	  {
	    m_SurfaceOutput->ReparentTo(NULL);
	  }

    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }

  if (m_Gui)
  {
  	HideGui();
  }
  mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpDo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutputRegionGrowing )
  {
    m_VolumeOutputRegionGrowing->ReparentTo(m_Input);
  }
  if (m_VolumeOutputMorpho)
  {
    m_VolumeOutputMorpho->ReparentTo(m_Input);
  }
  if (m_SurfaceOutput)
  {
    m_SurfaceOutput->ReparentTo(m_Input);
  }

  if (m_VolumeOutputMorpho == NULL && m_VolumeOutputRegionGrowing == NULL && m_SurfaceOutput == NULL)
  {
    return;
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutputRegionGrowing)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_VolumeOutputRegionGrowing));
  }
  if (m_VolumeOutputMorpho)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_VolumeOutputMorpho));
  }
  if (m_SurfaceOutput)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_SurfaceOutput));
  }

  if (m_VolumeOutputMorpho == NULL && m_VolumeOutputRegionGrowing == NULL && m_SurfaceOutput == NULL)
  {
    return;
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
