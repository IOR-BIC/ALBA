/*=========================================================================

 Program: MAF2
 Module: mafGUILutHistogramEditor
 Authors: Crimi Gianluigi
 
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
#include <wx/image.h>
#include <math.h>

#include "mafGUILutHistogramEditor.h"
#include "mafGUI.h"
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
#include "mafGUILutHistogramSwatch.h"
#include <wx/busyinfo.h>


#define SUB_SAMPLED_SIZE (64*64*64)

//----------------------------------------------------------------------------
// mafGUILutEditor
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//vtkDataSet *dataSet, mmaVolumeMaterial *material, char *name="Histogram & Windowing", mafObserver *Listener=NULL, int id=MINID);
mafGUILutHistogramEditor::mafGUILutHistogramEditor(vtkDataSet *dataSet,mmaVolumeMaterial *material, char *name, mafObserver *Listener, int id)
:mafGUIDialog(name)
//----------------------------------------------------------------------------
{
	 
  m_LutSwatch = NULL;
  m_ResampledData = NULL;
  m_Lut = vtkLookupTable::New();
  m_Lut->Build();

  SetDataSet(dataSet);
  SetMaterial(material);
  SetListener(Listener);
  SetId(id);
  
  m_FullSampling=0;
  m_LogScale=0;

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
  m_Histogram = new mafGUIHistogramWidget(gui,-1,wxPoint(0,0),wxSize(500,100),wxTAB_TRAVERSAL);
  m_Histogram->SetListener(this);
  m_Histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
  m_Histogram->SetData(m_ResampledData);
  m_Histogram->SetLut(  material->m_ColorLut );
  m_Histogram->ShowLines();
  m_Histogram->ShowText(false);
  gui->Add(m_Histogram,1);

  m_LutSwatch = new mafGUILutHistogramSwatch(gui ,-1,"", dataSet, material, wxSize(482,18),false);
  m_LutSwatch->ShowThreshold(true);
  m_LutSwatch->EnableOverHighlight(true);
  m_LutSwatch->SetListener(this);


  m_Windowing = new mafGUILutSlider(gui,-1,wxPoint(0,0),wxSize(500,24));
  m_Windowing->SetListener(gui);
  gui->Add(m_Windowing,0);

  gui->Divider();
  //gamma correction slider 
  m_Gamma = material->m_GammaCorrection;
  m_GammaSlider = gui->FloatSlider(ID_GAMMA_CORRETION,_("Gamma: "), &m_Gamma,0,5, wxSize(300,30), "", false);

  //Activate only if required
  if (m_DataSet->GetPointData()->GetScalars()->GetNumberOfTuples()>SUB_SAMPLED_SIZE)
    gui->Bool(ID_FULL_SAMPLING,"Full Sampling (accurate but slow)",&m_FullSampling,1);
  
  gui->Bool(ID_LOG_SCALE_VIEW,"View histogram in log scale",&m_LogScale,1);

  gui->Button(ID_RESET_LUT,"Reset Lut");

  wxStaticText *label = new wxStaticText(this, -1, " Right click + Up/Down in the histogram to zoom In/Out", wxDefaultPosition, wxSize(500,18), wxALIGN_LEFT | wxST_NO_AUTORESIZE );

  gui->Add(label,0,wxEXPAND | wxALL);

  gui->FitGui();
  gui->Update();
  this->Add(gui,1);

  m_Windowing->SetRange(m_LowRange,m_HiRange);
  m_Windowing->SetSubRange(m_Lut->GetTableRange()[0],m_Lut->GetTableRange()[1]);
  double ranges[2];
  material->m_ColorLut->GetTableRange(ranges);
  m_Windowing->SetSubRange(ranges[0],ranges[1]);
  m_Histogram->Refresh();
  this->Fit();
  this->SetMinSize(wxSize(620,410));
  this->Update();
}

//----------------------------------------------------------------------------
mafGUILutHistogramEditor::~mafGUILutHistogramEditor()
//----------------------------------------------------------------------------
{
  
  //Deleting Lut
  if(m_Lut) 
    mafDEL(m_Lut);

// Already deleted by the histogram
//   if (m_ResampledData)
//     vtkDEL(m_ResampledData);
}


//----------------------------------------------------------------------------
void mafGUILutHistogramEditor::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
	  {
      case ID_RANGE_MODIFIED:
        {
          m_Windowing->GetSubRange(&m_LowRange, &m_HiRange);
          m_Histogram->UpdateLines(m_LowRange, m_HiRange);
          UpdateVolumeLut();
          m_LutSwatch->Modified();
          //Generating Event to update other views
          mafEventMacro(mafEvent(this,GetId()));
        }
        break;
      case ID_GAMMA_CORRETION:
        {
          UpdateVolumeLut();
          m_LutSwatch->Modified();
          //Generating Event to update other views
          mafEventMacro(mafEvent(this,GetId()));
        }
        break;
      case ID_RESET_LUT:
        {
          //Setting lut/gamma to default values
          ResetLutDialog(1.0, m_LowRange, m_HiRange);
          m_Histogram->UpdateLines(m_LowRange, m_HiRange);
          UpdateVolumeLut();
          m_LutSwatch->Modified();
          //Generating Event to update other views
          mafEventMacro(mafEvent(this,GetId()));
        }
        break;
      case ID_FULL_SAMPLING:
      {

         wxBusyCursor wait;
         
         if (m_FullSampling)
         {
           //Set busy info only on slow (full histogram) operation
           wxBusyInfo wait(_("Updating Histogram ..."));
           m_Histogram->SetData(m_DataSet->GetPointData()->GetScalars());
         }
         else 
           m_Histogram->SetData(m_ResampledData);
      }
      break;
      case ID_LOG_SCALE_VIEW:
        {
          //Enable disable log scale
          m_Histogram->AutoscaleHistogramOn();
          m_Histogram->LogarithmicScale(m_LogScale);
          m_Histogram->UpdateLines(m_LowRange, m_HiRange);
          this->Refresh();
          this->Update();
        }
        break;

      default:
        {
         //forward up events, needed for showing highlight in other views  
         this->Update();
         this->Refresh();
         mafEventMacro(*e);
        }
      break;
    }
  }
}

//----------------------------------------------------------------------------
void mafGUILutHistogramEditor::SetDataSet(vtkDataSet *dataSet)
//----------------------------------------------------------------------------
{
  m_DataSet = dataSet;
  
  m_LowRange = dataSet->GetScalarRange()[0];
  m_HiRange = dataSet->GetScalarRange()[1];
}

//----------------------------------------------------------------------------
void mafGUILutHistogramEditor::SetMaterial(mmaVolumeMaterial *material)
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
void mafGUILutHistogramEditor::CopyLut(vtkLookupTable *from, vtkLookupTable *to)
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
void mafGUILutHistogramEditor::ShowLutHistogramDialog(vtkDataSet *dataSet,mmaVolumeMaterial *material,char *name, mafObserver *listener, int id)
//----------------------------------------------------------------------------
{
  //Call the default constructor to show the Dialog
  mafGUILutHistogramEditor *led = new mafGUILutHistogramEditor(dataSet,material,name,listener,id);
  led->CentreOnScreen();
  led->ShowModal();
}

//----------------------------------------------------------------------------
vtkDataArray* mafGUILutHistogramEditor::Resample(vtkDataArray *inDA, vtkDataArray* outDA)
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
void mafGUILutHistogramEditor::ResetLutDialog(double gamma, double low, double high)
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
void mafGUILutHistogramEditor::UpdateVolumeLut(bool reset)
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

  if (m_LutSwatch)
    m_LutSwatch->Modified();

  //Forward event to update other views
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}

//----------------------------------------------------------------------------
void mafGUILutHistogramEditor::OnSize(wxSizeEvent &event)
//----------------------------------------------------------------------------
{
  mafGUIDialog::OnSize(event);
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));
}
