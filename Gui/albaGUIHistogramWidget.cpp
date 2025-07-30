/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGUIHistogramWidget
 Authors: Paolo Quadrani, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGUIHistogramWidget.h"
#include "albaDecl.h"
#include "albaRWI.h"
#include "albaGUI.h"
#include "albaVME.h"
#include "albaGUIRangeSlider.h"
#include "albaGUILutSlider.h"

#include "albaDeviceButtonsPad.h"
#include "albaDeviceButtonsPadMouse.h"
#include "albaEventInteraction.h"
#include "mmuIdFactory.h"

#include "vtkDataSet.h"
#include "vtkDataArray.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPointData.h"
#include "vtkLookupTable.h"
#include "vtkLineSource.h"
#include "vtkProperty.h"
#include "vtkPolyDataMapper.h"

ALBA_ID_IMP(albaGUIHistogramWidget::RANGE_MODIFIED);

//----------------------------------------------------------------------------
albaGUIHistogramWidget::albaGUIHistogramWidget(wxWindow* parent, wxWindowID id /* = -1 */, const wxPoint& pos /* = wxDefaultPosition */, const wxSize& size /* = wxSize */,long style /* = wxTAB_TRAVERSAL */, bool showThresholds /* = false */)
:albaGUIPanel(parent,id,pos,size,style)
//----------------------------------------------------------------------------
{
	m_Listener    = NULL;
  m_Histogram   = NULL;
	m_HistogramData = NULL;
  m_Gui         = NULL;
  m_Data        = NULL;
	m_VME = NULL;
  m_Lut         = NULL;
  m_Slider      = NULL;
  m_SliderThresholds = NULL;

  m_SelectedRange[0] = 0.0;
  m_SelectedRange[1] = 1.0;
  m_HisctogramValue = 0;

  m_Threshold[0] = 0;
  m_Threshold[1] = 1;
  
  m_LogHistogramFlag   = false;
  m_ShowText           = true;

	m_NumberOfBins = 100;
  //////////////////////////////////////////////////////////////////////////

  vtkNEW(m_Histogram);
  m_Histogram->SetColor(0.3, 0.3, 0.9);
  m_Histogram->SetHisctogramRepresentation(vtkALBAHistogram::BAR_REPRESENTATION);

  wxBoxSizer *sizerV = new wxBoxSizer(wxVERTICAL);

  m_HistogramRWI = new albaRWI(albaGetFrame());
  m_HistogramRWI->SetListener(this);
  m_HistogramRWI->m_RenFront->AddActor2D(m_Histogram);
  //m_HistogramRWI->m_RenFront->AddActor2D(actor);
  m_HistogramRWI->m_RenFront->SetBackground(0.28,0.28,0.28);
  m_HistogramRWI->SetSize(pos.x,pos.y,size.GetWidth(),size.GetHeight());
  ((wxWindow *)m_HistogramRWI->m_RwiBase)->SetSize(size.GetWidth(),size.GetHeight());
  ((wxWindow *)m_HistogramRWI->m_RwiBase)->SetMinSize(wxSize(size.GetWidth(),size.GetHeight()));
  //m_HistogramRWI->m_RwiBase->SetMinSize(wxSize(size.GetWidth(),size.GetHeight()));
  m_HistogramRWI->m_RwiBase->Reparent(this);
  m_HistogramRWI->m_RwiBase->SetListener(this);
  m_HistogramRWI->m_RwiBase->Show(true);

  if (showThresholds)
  {
	  wxBoxSizer *sizerH1 = new wxBoxSizer(wxHORIZONTAL);
	  m_SliderThresholds = new albaGUILutSlider(this,ID_SLIDER_THRESHOLD ,wxPoint(0,0),wxSize(10,24), 0, "Thresholds");
		m_SliderThresholds->SetFloatingPointTextOn();
	  m_SliderThresholds->SetListener(this);
	  m_SliderThresholds->SetSize(5,24);
	  m_SliderThresholds->SetMinSize(wxSize(5,24));
	  m_SliderThresholds->SetRange(m_Threshold);
	  m_SliderThresholds->SetSubRange(m_Threshold);
	  sizerH1->Add(m_SliderThresholds,wxEXPAND);
	  sizerV->Add(sizerH1, 0,wxEXPAND);

    m_Histogram->ShowLinesOn();
  }

  wxStaticText *foo_l = new wxStaticText(this,-1, "");
  wxStaticText *foo_r = new wxStaticText(this,-1, "");

  wxBoxSizer *sizerH2 = new wxBoxSizer(wxHORIZONTAL);
  sizerH2->Add(foo_l, 0, wxLEFT, 0);
  sizerH2->Add(m_HistogramRWI->m_RwiBase, 1, wxEXPAND, 0);
  sizerH2->Add(foo_r, 0, wxLEFT, 0);
  sizerV->Add(sizerH2, 1,wxEXPAND);

  SetSizer(sizerV);

  sizerV->Layout();           // resize & fit the contents
  sizerV->SetSizeHints(this); // resize the dialog accordingly 

  SetAutoLayout(true);
  sizerV->Fit(this);

//  CreateGui();

  SetMinSize(size);
}
//----------------------------------------------------------------------------
albaGUIHistogramWidget::~albaGUIHistogramWidget() 
//----------------------------------------------------------------------------
{
  m_HistogramRWI->m_RenFront->RemoveActor(m_Histogram);
  cppDEL(m_Slider);
  vtkDEL(m_Histogram);
  cppDEL(m_HistogramRWI);
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::UpdateLines(int min,int max)
//----------------------------------------------------------------------------
{
  m_Threshold[0]=min;
  m_Threshold[1]=max;
  m_Histogram->UpdateLines(m_Threshold);
  m_HistogramRWI->CameraUpdate();
}
//----------------------------------------------------------------------------
albaGUI *albaGUIHistogramWidget::GetGui()
//----------------------------------------------------------------------------
{

	if (m_Gui == NULL)
  {
    CreateGui();
  }
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::GetThresholds(double *lower, double *upper)
//----------------------------------------------------------------------------
{
  *lower=m_Threshold[0];
  *upper=m_Threshold[1];
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::CreateGui()
//----------------------------------------------------------------------------
{
  wxString represent[3] = {"points", "lines", "bar"};
  m_Gui = new albaGUI(this);
  m_Gui->Show(true);

  if (m_Lut != NULL)
  {
    m_Slider = new albaGUIRangeSlider(this, ID_RANGE_SLICER, wxDefaultPosition, wxSize(m_Gui->GetMetrics(GUI_FULL_WIDTH), 30), wxNO_BORDER);
    m_Slider->SetListener(this);
    m_Slider->EnableCenterWidget(false);
    m_Slider->EnableBoundaryHandles(false);
    m_Data->GetRange(m_SelectedRange);
    m_Slider->SetRange(m_SelectedRange);
    m_Lut->GetTableRange(m_SelectedRange);
    m_Slider->SetValue(0, m_SelectedRange[0]);
    m_Slider->SetValue(2, m_SelectedRange[1]);
    m_Slider->SetSize(wxSize(m_Gui->GetMetrics(GUI_FULL_WIDTH), 30));
    m_Slider->SetMinSize(wxSize(m_Gui->GetMetrics(GUI_FULL_WIDTH), 30));
    m_Slider->Update();
    m_Gui->Add(m_Slider);
  }

	m_Gui->Slider(ID_NUMBER_OF_BIN, "Bin #:", &m_NumberOfBins, 10, 500, "Sets the number of bins of the Histogream");
  m_Gui->Bool(ID_LOGSCALE,"Log Scale",&m_LogHistogramFlag,0,"Enable/Disable log scale for histogram");
	m_Gui->Button(ID_EXPORT_DATA, "Export Data");
	m_Gui->Button(ID_EXPORT_STATS, "Export Stats");
	m_Gui->Divider();

  EnableWidgets(m_Data != NULL);
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::OnEvent( albaEventBase *event )
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(event))
  {
		switch (e->GetId())
		{
			case ID_RANGE_MODIFIED:
			{
				if (m_SliderThresholds)
					m_SliderThresholds->GetSubRange(m_Threshold);
				m_Histogram->UpdateLines(m_Threshold);
			}
			break;
			case ID_LOGSCALE:
				LogarithmicScale(m_LogHistogramFlag);
				break;
			case ID_RANGE_SLICER:
				m_SelectedRange[0] = m_Slider->GetValue(0);
				m_SelectedRange[1] = m_Slider->GetValue(2);
				m_Lut->SetTableRange(m_SelectedRange);
				albaEventMacro(albaEvent(this, albaGUIHistogramWidget::RANGE_MODIFIED));
				break;
			case ID_NUMBER_OF_BIN:
				m_Histogram->SetNumberOfBins(m_NumberOfBins);
				break;
			case ID_EXPORT_DATA:
				ExportData();
				break;
			case ID_EXPORT_STATS:
				ExportStats();
				break;
			default:
				e->Log();
				break;
		}
    m_HistogramRWI->CameraUpdate();
  }
  else if (albaEventInteraction *ei = albaEventInteraction::SafeDownCast(event))
  {
    if (ei->GetId() == albaDeviceButtonsPadMouse::GetMouse2DMoveId())
    {
      if(m_Histogram->GetInputData() == NULL) return;
			if (m_ShowText)
			{
				double pos[2];
				ei->Get2DPosition(pos);

				m_HisctogramValue = m_Histogram->GetHistogramValue(pos[0], pos[1]);
				m_Histogram->SetLabel(albaString(m_HisctogramValue).GetCStr());
			}
      m_HistogramRWI->CameraUpdate();
    }
  }
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::UpdateGui()
//----------------------------------------------------------------------------
{
  m_HistogramRWI->CameraUpdate();
	m_NumberOfBins = m_Histogram->GetNumberOfBins();
  m_LogHistogramFlag    = m_Histogram->GetLogHistogram();
  if (m_Gui != NULL)
  {
    m_Gui->Update();
    EnableWidgets(m_Data != NULL);
  }
}
//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetData(vtkDataArray *data, albaVME *vme)
//----------------------------------------------------------------------------
{
  m_Data = data;
	m_VME = vme;
  m_Histogram->SetInputData(m_Data);
	double sr[2];
  m_Data->GetRange(sr);

	if (m_Lut)
		m_Lut->GetTableRange(m_Threshold);
	else
		m_Data->GetRange(m_Threshold);

  if (m_SliderThresholds != NULL)
  {
	  m_SliderThresholds->SetRange(sr[0],sr[1]);
	  m_SliderThresholds->SetSubRange(m_Threshold);
  }
	m_Histogram->SetNumberOfBins(m_NumberOfBins);
  m_Histogram->UpdateLines(m_Threshold);
	m_Histogram->Modified();
  UpdateGui();
	m_HistogramRWI->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetLut(vtkLookupTable *lut)
//----------------------------------------------------------------------------
{
  m_Lut=lut;
  double sr[2];
  if(m_Data)
    m_Data->GetRange(sr);
  else 
    lut->GetTableRange(sr);
  lut->GetTableRange(m_Threshold);
  if (m_SliderThresholds != NULL)
  {
	  m_SliderThresholds->SetRange(sr[0],sr[1]);
	  m_SliderThresholds->SetSubRange(m_Threshold);
  }
  m_Histogram->UpdateLines(m_Threshold);
  UpdateGui();
}


//----------------------------------------------------------------------------
void albaGUIHistogramWidget::LogarithmicScale(int enable)
//----------------------------------------------------------------------------
{
  m_Histogram->SetLogHistogram(enable);
  UpdateGui();
}



//----------------------------------------------------------------------------
void albaGUIHistogramWidget::EnableWidgets(bool enable)
//----------------------------------------------------------------------------
{
	m_Gui->Enable(albaGUIHistogramWidget::ID_NUMBER_OF_BIN, enable);
	m_Gui->Enable(albaGUIHistogramWidget::ID_LOGSCALE, enable);
}

//----------------------------------------------------------------------------
void albaGUIHistogramWidget::ExportData()
{
	wxString proposed = albaGetLastUserFolder().ToAscii();
	proposed += m_Data->GetName();
	proposed += ".csv";

	wxString wildc = "ASCII CSV file (*.csv)|*.csv";
	wxString f = albaGetSaveFile(proposed, wildc).ToAscii();
	FILE *outFile = NULL;

	int result = OP_RUN_CANCEL;

	if (!f.IsEmpty())
		outFile = albaTryOpenFile(f.ToAscii(), "w");

	if (outFile != NULL)
	{//Header
		fprintf(outFile, "%s;\n", m_Data->GetName());

		//Content
		for (int i = 0; i < m_Data->GetNumberOfTuples(); i++)
		{
			double value = m_Data->GetTuple1(i);
			fprintf(outFile, "%f;\n", value);
		}
		fclose(outFile);


		albaOpenWithDefaultApp(f.ToAscii());
	}
}

//----------------------------------------------------------------------------
void albaGUIHistogramWidget::ExportStats()
{

	wxString proposed = albaGetLastUserFolder().ToAscii();
	proposed += m_Data->GetName();
	proposed += ".csv";

	wxString wildc = "ASCII CSV file (*.csv)|*.csv";
	wxString f = albaGetSaveFile(proposed, wildc).ToAscii();

	bool firstAcces = !wxFileExists(f.ToAscii());

	FILE * pFile=NULL;

	if (!f.IsEmpty())
		pFile = albaTryOpenFile(f.ToAscii(), "a+");

	if (pFile != NULL)
	{


		if (firstAcces) // Header
			fprintf(pFile,"VME Name;Scalar Name;Mean;Min;Max;STD Error;\n");

		albaString vmeName = m_VME ? m_VME->GetName() : "";

		double sumValues = 0;
		double min = VTK_DOUBLE_MAX;
		double max = VTK_DOUBLE_MIN;
		int nValues = m_Data->GetNumberOfTuples();

		//Values are on Squared Dist
		for (int i = 0; i < nValues; i++)
		{
			double value = m_Data->GetTuple1(i);
			sumValues += value;
			min = MIN(min, value);
			max = MAX(max, value);
		}

		double mean = sumValues / (double)nValues;
		double errSq = 0;

		for (int i = 0; i < nValues; i++)
		{
			double err = mean - m_Data->GetTuple1(i);
			errSq += err*err;
		}

		double stdDev = sqrt(errSq / (double)nValues);

		fprintf(pFile, "%s;%s;%.2f;%.2f;%.2f;%.2f;\n", vmeName.GetCStr(), m_Data->GetName(), mean, min, max, stdDev);
		
		fclose(pFile);

		albaOpenWithDefaultApp(f.ToAscii());
	}
}

//----------------------------------------------------------------------------
void albaGUIHistogramWidget::ShowLines(int value)
//----------------------------------------------------------------------------
{
  if (value)
    m_Histogram->ShowLinesOn();
  else
    m_Histogram->ShowLinesOff();
}


//----------------------------------------------------------------------------
void albaGUIHistogramWidget::SetHistogramData(vtkImageData *histogram)
//----------------------------------------------------------------------------
{
	m_HistogramData = histogram;
}
//----------------------------------------------------------------------------
double albaGUIHistogramWidget::GetHistogramScalarValue(int x, int y)
//----------------------------------------------------------------------------
{
  return m_Histogram->GetScalarValue(x,y);
}
//----------------------------------------------------------------------------
long int albaGUIHistogramWidget::GetHistogramValue(int x, int y)
//----------------------------------------------------------------------------
{
  return m_Histogram->GetHistogramValue(x,y);
}
