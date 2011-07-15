/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGUILutHistogramEditor.cpp,v $
  Language:  C++
  Date:      $Date: 2011-07-15 16:17:40 $
  Version:   $Revision: 1.1.2.6 $
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
#include "mafGUIFloatSlider.h"
#include "mafGUILutPreset.h"
#include "mafGUILutSlider.h"
#include "mafGUIDialog.h"
#include "vtkDataSet.h"
#include "vtkPointData.h"
#include "mmaMaterial.h"
#include "mmaVolumeMaterial.h"
#include "mafGUIHistogramWidget.h"
#include "vtkFloatArray.h"

#define SUB_SAMPLED_SIZE (64*64*64)

//----------------------------------------------------------------------------
// mafGUILutEditor
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//vtkDataSet *dataSet, mmaVolumeMaterial *material, char *name="Histogram & Windowing", mafObserver *Listener=NULL, int id=MINID);
medGUILutHistogramEditor::medGUILutHistogramEditor(vtkDataSet *dataSet,mmaVolumeMaterial *material, char *name, mafObserver *Listener, int id)
:mafGUIDialog(name)         
//----------------------------------------------------------------------------
{
	  
  m_LutSwatch = NULL;// new medGUILutHistogramSwatch(this, -1, wxDefaultPosition,wxSize(286,16));
  m_ResampledData = NULL;
  m_Lut = vtkLookupTable::New();
  m_Lut->Build();

  SetDataSet(dataSet);
  SetMaterial(material);
  SetListener(Listener);
  SetId(id);
  
  m_FullSampling=0;

  /*
  mafVMEVolumeGray *resampled = NULL;
  resampled = mafVMEVolumeGray::New();
*/
  wxBusyCursor wait;
  

  //resampling data because full histograms
  //take long time
  //resample
  if (m_DataSet->GetPointData()->GetScalars()->GetNumberOfTuples()>SUB_SAMPLED_SIZE)
    m_ResampledData = Resample(m_DataSet->GetPointData()->GetScalars(), m_ResampledData );
  else 
    m_ResampledData = m_DataSet->GetPointData()->GetScalars();
  
  this->SetListener(Listener);
  mafGUI *gui = new mafGUI(this);
  m_GuiDialog = gui;
  mafString tag_name;
  tag_name = "HISTOGRAM_VOLUME";

  //Setting up the histogram
  m_Histogram = new mafGUIHistogramWidget(gui,-1,wxPoint(0,0),wxSize(500,100),wxTAB_TRAVERSAL); //,true);
  m_Histogram->SetLut(  material->m_ColorLut );
  m_Histogram->SetListener(gui);
  m_Histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
  m_Histogram->SetData(m_ResampledData);
  gui->Add(m_Histogram,1);


  m_Windowing = new mafGUILutSlider(gui,-1,wxPoint(0,0),wxSize(500,24));
  m_Windowing->SetListener(gui);
  gui->Add(m_Windowing,0);

  gui->Divider();
  //gamma correction slider 
  m_Gamma = material->m_GammaCorrection;
  m_GammaSlider = gui->FloatSlider(ID_GAMMA_CORRETION,_("Gamma: "), &m_Gamma,0,5, wxSize(400,30), "", false);

  //Activate only if required
  if (m_DataSet->GetPointData()->GetScalars()->GetNumberOfTuples()>SUB_SAMPLED_SIZE)
    gui->Bool(ID_FULL_SAMPLING,"Full Sampling (accurate but slow)",&m_FullSampling,1);
  
  gui->Button(ID_RESET_LUT,"Reset Lut");
  this->Add(gui,1);
  this->SetMinSize(wxSize(500,124));

  m_Windowing->SetRange(m_LowRange,m_HiRange);
  m_Windowing->SetSubRange(m_Lut->GetTableRange()[0],m_Lut->GetTableRange()[1]);
  double ranges[2];
  material->m_ColorLut->GetTableRange(ranges);
  m_Windowing->SetSubRange(ranges[0],ranges[1]);
  m_Histogram->Refresh();

  this->ShowModal();


}
//----------------------------------------------------------------------------
medGUILutHistogramEditor::~medGUILutHistogramEditor()
//----------------------------------------------------------------------------
{
  
  //Deleting Lut
  if(m_Lut) 
    mafDEL(m_Lut);

  //Deleting Sub Sampled Data
  if (m_ResampledData)
    vtkDEL(m_ResampledData);
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
      case ID_FULL_SAMPLING:
      {
         wxBusyCursor wait;
         
         if (m_FullSampling)
           m_Histogram->SetData(m_DataSet->GetPointData()->GetScalars());
         else 
           m_Histogram->SetData(m_ResampledData);

      }
      break;
    }
  }
}

//----------------------------------------------------------------------------
void medGUILutHistogramEditor::SetDataSet(vtkDataSet *dataSet)
//----------------------------------------------------------------------------
{
  m_DataSet = dataSet;
  
  m_LowRange = dataSet->GetScalarRange()[0];
  m_HiRange = dataSet->GetScalarRange()[1];
}

//----------------------------------------------------------------------------
void medGUILutHistogramEditor::SetMaterial(mmaVolumeMaterial *material)
//----------------------------------------------------------------------------
{
  m_Material=material;
  m_Gamma = material->m_GammaCorrection;
  vtkLookupTable *lut = material->m_ColorLut;

  m_ExternalLut = lut;

  // copy the given lut on the internal one
  if(m_ExternalLut)
    CopyLut(m_ExternalLut, m_Lut);

  //If we have a Lut swatch set the lut to show
  if (m_LutSwatch)
    m_LutSwatch->SetMaterial(m_Material);

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
void medGUILutHistogramEditor::ShowLutHistogramDialog(vtkDataSet *dataSet,mmaVolumeMaterial *material,char *name, mafObserver *listener, int id)
//----------------------------------------------------------------------------
{
  //Call the default constructor to show the Dialog
  medGUILutHistogramEditor *led = new medGUILutHistogramEditor(dataSet,material,name,listener,id);
}

//----------------------------------------------------------------------------
vtkDataArray* medGUILutHistogramEditor::Resample(vtkDataArray *inDA, vtkDataArray* outDA)
//----------------------------------------------------------------------------
{

  if (outDA)
    vtkDEL(outDA);

  double fullSize;

  vtkFloatArray *tmp;
  vtkNEW(tmp);

  outDA=tmp;
  
  //Generating new resampled dataset
  outDA->SetNumberOfTuples(SUB_SAMPLED_SIZE + 2);
  outDA->SetName("SCALARS");


  fullSize=inDA->GetNumberOfTuples();

  for (int i=0;i<SUB_SAMPLED_SIZE;i++)
  {
    //copying only a sub-set of the data
    double value = inDA->GetTuple1( ceil( i * fullSize / (double)SUB_SAMPLED_SIZE ) );
    outDA->SetTuple1(i,value);
  }
  //Add the min and max values in order to obtain the same range after sub-sampling
  outDA->SetTuple1(SUB_SAMPLED_SIZE,inDA->GetRange()[0]);
  outDA->SetTuple1(SUB_SAMPLED_SIZE+1,inDA->GetRange()[1]);

  outDA->ComputeRange(0);

  return outDA;
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
  if (m_Material)
  {
    if(reset)
    {
      m_LowRange = m_DataSet->GetScalarRange()[0];
      m_HiRange = m_DataSet->GetScalarRange()[1];
    }

    double oldGamma = m_Material->m_GammaCorrection;

    m_Material->m_ColorLut->SetTableRange(m_LowRange,m_HiRange );
    m_Material->m_GammaCorrection = m_Gamma;

    m_Material->UpdateFromTables();
    m_Material->ApplyGammaCorrection(4);
    m_Material->UpdateProp();

  }
  //Forward event to update other views
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}


