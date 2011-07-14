/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILutHistogramEditor.cpp,v $
  Language:  C++
  Date:      $Date: 2011-07-14 08:23:37 $
  Version:   $Revision: 1.1.2.3 $
  Authors:   Silvano Imboden
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
#include <wx/image.h>
#include <math.h>

#include "medGUILutHistogramEditor.h"
#include "mafGUIValidator.h"
#include "mafGUIButton.h"
#include "mafGUIFloatSlider.h"
#include "mafGUILutPreset.h"
#include "mafGUILutSlider.h"
#include "mafGUIDialog.h"
#include "vtkTransform.h"
#include "mafTransformFrame.h"
#include "vtkDataSet.h"
#include "mafVME.h"
#include "mafVMELandmark.h"
#include "mafTransform.h"
#include "vtkPointData.h"
#include "vtkMAFVolumeResample.h"
#include "vtkStructuredPoints.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafGUIHistogramWidget.h"
#include "vtkMAFHistogram.h"

//----------------------------------------------------------------------------
// const
//----------------------------------------------------------------------------
const int  M	= 1;											// margin all around a row
const int LM	= 5;											// label margin
const int HM	= 2*M;										// horizontal margin

const int LH	= 16;											// label/entry height
const int BH	= 20;											// button height

const int LW	= 128; 									  // label width
const int EW	= 50;											// entry width  - era 45
const int FW	= LW+LM+EW+HM+EW+HM+EW;		// full width
const int DW	= EW+HM+EW+HM+EW;					// Data Width - Full Width without the Label

static wxPoint dp = wxDefaultPosition; 
  
//----------------------------------------------------------------------------
// Widgets ID's
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// mafGUILutEditor
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medGUILutHistogramEditor::medGUILutHistogramEditor(mafVME *vme, vtkLookupTable *lut, char *name, mafObserver *Listener, int id)
:mafGUIDialog(name)         
//----------------------------------------------------------------------------
{
	  
  m_LutSwatch = NULL;// new medGUILutHistogramSwatch(this, -1, dp,wxSize(286,16));
  m_Lut = vtkLookupTable::New();
  m_Lut->Build();

  SetVolume(vme);
  SetListener(Listener);
  SetLut(lut);
  SetId(id);
  

  ///END OLD CODE
  mafVMEVolumeGray *resampled = NULL;
  resampled = mafVMEVolumeGray::New();

  wxBusyCursor wait;
  
  //mafGUIDialog dlg(_("Histogram & Windowing"));
  //resaple
  Resample(m_Volume, resampled);

  double sr[2], srR[2];
  mmaVolumeMaterial *material = ((mafVMEVolume *)m_Volume)->GetMaterial();
  
  //resampling data beacouse full histograms
  //take long time
  vtkDataSet *data, *dataResampled;
  data = m_Volume->GetOutput()->GetVTKData();
  data->Update();
  data->GetScalarRange(sr);

  dataResampled = resampled->GetOutput()->GetVTKData();
  dataResampled->Update();
  dataResampled->GetScalarRange(srR);

  this->SetListener(Listener);
  mafGUI *gui = new mafGUI(this);
  m_GuiDialog = gui;
  mafString tag_name;
  tag_name = "HISTOGRAM_VOLUME";

  //Setting up the histogram
  mafGUIHistogramWidget *histogram = new mafGUIHistogramWidget(gui,-1,wxPoint(0,0),wxSize(500,100),wxTAB_TRAVERSAL); //,true);
  histogram->SetLut(  material->m_ColorLut );
  histogram->SetListener(gui);
  histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
  histogram->SetData(dataResampled->GetPointData()->GetScalars());
  gui->Add(histogram,1);


  m_Windowing = new mafGUILutSlider(gui,-1,wxPoint(0,0),wxSize(500,24));
  m_Windowing->SetListener(gui);
  gui->Add(m_Windowing,0);

  gui->Divider();
  //gamma correction slider 
  m_Gamma = material->m_GammaCorrection;
  m_GammaSlider = gui->FloatSlider(ID_GAMMA_CORRETION,_("Gamma: "), &m_Gamma,0,5, wxSize(400,30), "", false);
  gui->Button(ID_RESET_LUT,"Reset Lut");
  this->Add(gui,1);
  this->SetMinSize(wxSize(500,124));

  m_Windowing->SetRange((long)sr[0],(long)sr[1]);
  m_Windowing->SetSubRange((long)material->m_TableRange[0],(long)material->m_TableRange[1]);
  histogram->Refresh();

  this->ShowModal();

  mafDEL(resampled);

}
//----------------------------------------------------------------------------
medGUILutHistogramEditor::~medGUILutHistogramEditor()
//----------------------------------------------------------------------------
{
  
  //Deleting Lut
  if(m_Lut) 
    mafDEL(m_Lut);
}


//----------------------------------------------------------------------------
void medGUILutHistogramEditor::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
	  {
      case ID_RANGE_MODIFIED:
        {
          m_Windowing->GetSubRange(&m_LowRange, &m_HiRange);
          UpdateVolumeLut();
          //Generating Event to update other views
          mafEventMacro(mafEvent(this,GetId()));
        }
        break;
      case ID_GAMMA_CORRETION:
        {
          UpdateVolumeLut();
          //Generating Event to update other views
          mafEventMacro(mafEvent(this,GetId()));
        }
        break;
      case ID_RESET_LUT:
        {
            //Setting lut/gamma to default values
            UpdateVolumeLut(true);
            ResetLutDialog(1.0, m_LowRange, m_HiRange);
            //Generating Event to update other views
            mafEventMacro(mafEvent(this,GetId()));
        }
        break;
    }
  }
}

//----------------------------------------------------------------------------
void medGUILutHistogramEditor::SetVolume(mafVME *vol)
//----------------------------------------------------------------------------
{
  m_Volume = vol;
  mmaVolumeMaterial *material = ((mafVMEVolume *)m_Volume)->GetMaterial();

  m_LowRange = m_Volume->GetOutput()->GetVTKData()->GetScalarRange()[0];
  m_HiRange = m_Volume->GetOutput()->GetVTKData()->GetScalarRange()[1];

  m_Gamma = material->m_GammaCorrection;
}

//----------------------------------------------------------------------------
void medGUILutHistogramEditor::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_ExternalLut = lut;

  // copy the given lut on the internal one
  if(m_ExternalLut)
    CopyLut(m_ExternalLut, m_Lut);
  
  //If we have a Lut swatch set the lut to show
  if (m_LutSwatch)
    m_LutSwatch->SetLut(m_Lut);
  
  TransferDataToWindow();
}

//----------------------------------------------------------------------------
void medGUILutHistogramEditor::CopyLut(vtkLookupTable *from, vtkLookupTable *to)
//----------------------------------------------------------------------------
{
  if(from==NULL || to==NULL ) return;
  
  int n = from->GetNumberOfTableValues();
  if(n>256) n=256;
  to->SetNumberOfTableValues(n);
  to->SetRange(from->GetRange());
  //copying lut values
  for(int i=0; i<n; i++)
  {
    double *rgba;
    rgba = from->GetTableValue(i);
    to->SetTableValue(i,rgba[0],rgba[1],rgba[2],rgba[3]);
  }
 
}
//----------------------------------------------------------------------------
void medGUILutHistogramEditor::ShowLutHistogramDialog(mafVME *vme, vtkLookupTable *lut,char *name, mafObserver *listener, int id)
//----------------------------------------------------------------------------
{
  //Call the defaut constructor to show the Dialog
  medGUILutHistogramEditor *led = new medGUILutHistogramEditor(vme,lut,name,listener,id);
}

//----------------------------------------------------------------------------
void medGUILutHistogramEditor::Resample(mafVME *vme, mafVMEVolumeGray* resampled)
//----------------------------------------------------------------------------
{

  //THIS FUNCTION WILL BE REPLACED WHIT A VOLUME INDIPENDENT 
  //SUB-SAMPLING SYSTEM
  double volumeOrientation[3] = {0.,0.,0.};
  double volumePosition[3] = {0.,0.,0.};
  mafSmartPointer<mafTransform> box_pose;
  box_pose->SetOrientation(volumeOrientation);
  box_pose->SetPosition(volumePosition);


  double bounds[6];
  vme->GetOutput()->GetVTKData()->GetBounds(bounds);

  double spacing[3];
  vtkImageData::SafeDownCast(vme->GetOutput()->GetVTKData())->GetSpacing(spacing);

  double factor = 4.;
  spacing[0] *= factor;
  spacing[1] *= factor;
  spacing[2] *= factor;

  mafSmartPointer<mafTransformFrame> local_pose;
  local_pose->SetInput(box_pose);

  mafSmartPointer<mafTransformFrame> output_to_input;

  int output_extent[6];
  output_extent[0] = 0;
  output_extent[1] = (bounds[1] - bounds[0]) / spacing[0];
  output_extent[2] = 0;
  output_extent[3] = (bounds[3] - bounds[2]) / spacing[1];
  output_extent[4] = 0;
  output_extent[5] = (bounds[5] - bounds[4]) / spacing[2];

  if (vtkDataSet *input_data = vme->GetOutput()->GetVTKData())
  {
    // the resample filter
    vtkMAFSmartPointer<vtkMAFVolumeResample> resampler;
    resampler->SetZeroValue(0.);

    // Set the target be vme's parent frame. And Input frame to the root. I've to 
    // set at each iteration since I'm using the SetMatrix, which doesn't support
    // transform pipelines.
    mafSmartPointer<mafMatrix> output_parent_abs_pose;
    vme->GetParent()->GetOutput()->GetAbsMatrix(*output_parent_abs_pose.GetPointer(),0.);
    local_pose->SetInputFrame(output_parent_abs_pose);

    mafSmartPointer<mafMatrix> input_parent_abs_pose;
    vme->GetParent()->GetOutput()->GetAbsMatrix(*input_parent_abs_pose.GetPointer(),0.);
    local_pose->SetTargetFrame(input_parent_abs_pose);
    local_pose->Update();

    mafSmartPointer<mafMatrix> output_abs_pose;
    vme->GetOutput()->GetAbsMatrix(*output_abs_pose.GetPointer(),0.);
    output_to_input->SetInputFrame(output_abs_pose);

    mafSmartPointer<mafMatrix> input_abs_pose;
    vme->GetOutput()->GetAbsMatrix(*input_abs_pose.GetPointer(),0.);
    output_to_input->SetTargetFrame(input_abs_pose);
    output_to_input->Update();

    double orient_input[3],orient_target[3];
    mafTransform::GetOrientation(*output_abs_pose.GetPointer(),orient_target);
    mafTransform::GetOrientation(*input_abs_pose.GetPointer(),orient_input);

    double origin[3];
    origin[0] = bounds[0];
    origin[1] = bounds[2];
    origin[2] = bounds[4];

    output_to_input->TransformPoint(origin,origin);

    resampler->SetVolumeOrigin(origin[0],origin[1],origin[2]);

    vtkMatrix4x4 *mat = output_to_input->GetMatrix().GetVTKMatrix();

    double local_orient[3],local_position[3];
    mafTransform::GetOrientation(output_to_input->GetMatrix(),local_orient);
    mafTransform::GetPosition(output_to_input->GetMatrix(),local_position);

    // extract versors
    double x_axis[3],y_axis[3];

    mafMatrix::GetVersor(0,mat,x_axis);
    mafMatrix::GetVersor(1,mat,y_axis);

    resampler->SetVolumeAxisX(x_axis);
    resampler->SetVolumeAxisY(y_axis);

    vtkMAFSmartPointer<vtkStructuredPoints> output_data;
    output_data->SetSpacing(spacing);
    // TODO: here I probably should allow a data type casting... i.e. a GUI widget
    output_data->SetScalarType(input_data->GetPointData()->GetScalars()->GetDataType());
    output_data->SetExtent(output_extent);
    output_data->SetUpdateExtent(output_extent);

    double w,l,sr[2];
    input_data->GetScalarRange(sr);

    w = sr[1] - sr[0];
    l = (sr[1] + sr[0]) * 0.5;

    resampler->SetWindow(w);
    resampler->SetLevel(l);
    resampler->SetInput(input_data);
    resampler->SetOutput(output_data);
    resampler->AutoSpacingOff();
    resampler->Update();

    output_data->SetSource(NULL);
    output_data->SetOrigin(bounds[0],bounds[2],bounds[4]);


    resampled->SetData(vtkImageData::SafeDownCast(output_data),0.);
    resampled->Update();
  }
}

//----------------------------------------------------------------------------
void medGUILutHistogramEditor::ResetLutDialog(double gamma, double low, double high)
//----------------------------------------------------------------------------
{
  
  m_Gamma = gamma;
  m_GammaSlider->SetRange(0.0, 5.0, m_Gamma); 
  m_GammaSlider->Update();
  m_GammaSlider->SetValue(m_Gamma); 
  m_GammaSlider->Update();
  m_GammaSlider->Refresh();

  m_GuiDialog->Update();

  m_Windowing->SetSubRange(low,high);
  m_Windowing->Update();

}

//----------------------------------------------------------------------------
void medGUILutHistogramEditor::UpdateVolumeLut(bool reset)
//----------------------------------------------------------------------------
{  
  if (m_Volume)
  {
    if(reset)
    {
      double sr[2];
      ((mafVMEVolume *)m_Volume)->GetVolumeOutput()->GetVTKData()->GetScalarRange(sr);
      m_LowRange = sr[0];
      m_HiRange = sr[1];
    }

    mmaVolumeMaterial *material = ((mafVMEVolume *)m_Volume)->GetMaterial();

    double oldGamma = material->m_GammaCorrection;

    material->m_ColorLut->SetTableRange(m_LowRange,m_HiRange );
    material->m_GammaCorrection = m_Gamma;

    material->UpdateFromTables();
    material->ApplyGammaCorrection(4);
    
  }
  //Forward event to update other views
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

