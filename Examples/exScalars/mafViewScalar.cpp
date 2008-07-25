/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafViewScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-25 07:01:19 $
  Version:   $Revision: 1.3 $
  Authors:   Paolo Quadrani
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

#include "mafViewScalar.h"
#include "mafDecl.h"
#include "mafGUILutPreset.h"

#include "mafVME.h"
#include "mafPipe.h"
#include "mafPipeFactory.h"

#include "mafASCIIImporterUtility.h"

#include "vtkMAFSmartPointer.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkDoubleArray.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "vtkLookupTable.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafViewScalar);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafViewScalar::mafViewScalar(wxString label, int camera_position, bool show_axes, bool show_grid, bool show_ruler, int stereo)
:mafViewVTK(label,camera_position,show_axes,show_grid,show_ruler,stereo)
//----------------------------------------------------------------------------
{
  m_Files.clear();
  m_ScalarsArray.clear();
  m_DataOrder = DATA_ORDER_ROW;
  m_Line = NULL;
  m_Lut  = NULL;
  m_Mapper = NULL;
  m_ComponentActivated = 0;
}
//----------------------------------------------------------------------------
mafViewScalar::~mafViewScalar() 
//----------------------------------------------------------------------------
{
  for (int s=0; s<m_ScalarsArray.size();s++) 
  {
    vtkDEL(m_ScalarsArray[s]);
  }
  m_ScalarsArray.clear();
  vtkDEL(m_Line);
  vtkDEL(m_Mapper);
  vtkDEL(m_Lut);
}
//----------------------------------------------------------------------------
mafView *mafViewScalar::Copy(mafObserver *Listener)
//----------------------------------------------------------------------------
{
  mafViewScalar *v = new mafViewScalar(m_Label, m_CameraPositionId, m_ShowAxes, m_ShowGrid, m_ShowRuler,m_StereoType);
  v->m_Listener = Listener;
  v->m_Id = m_Id;
  v->m_PipeMap = m_PipeMap;
  v->Create();
  return v;
}
//-------------------------------------------------------------------------
mafGUI *mafViewScalar::CreateGui()
//-------------------------------------------------------------------------
{
  assert(m_Gui == NULL);

  wxString order_choices[2] = {"row","column"};

  vtkNEW(m_Line);
  vtkNEW(m_Mapper);
  vtkNEW(m_Lut);
  lutPreset(4,m_Lut);

  m_Gui = new mafGUI(this);
  m_Gui->Button(ID_ASCII_FILE,"ASCII data","","Choose single or multiple file ASCII.");
  m_Gui->Radio(ID_DATA_ORDER,"data order",&m_DataOrder,2,order_choices);
  m_Gui->Button(ID_READ_DATA,"read");
  m_Gui->Lut(ID_SCALAR_LUT,"lut",m_Lut);
  m_Gui->Integer(ID_COMPONENT_ACTIVE,"component",&m_ComponentActivated,0);

  m_Gui->Enable(ID_DATA_ORDER,m_Files.size()>0);
  m_Gui->Enable(ID_READ_DATA,m_Files.size()>0);
  m_Gui->Enable(ID_SCALAR_LUT,m_Files.size()>0);
  m_Gui->Enable(ID_COMPONENT_ACTIVE,m_Files.size()>0);

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafViewScalar::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case CAMERA_PRE_RESET:
        OnPreResetCamera();
        mafEventMacro(*maf_event);
      break;
      case CAMERA_POST_RESET:
        OnPostResetCamera();
        mafEventMacro(*maf_event);
      break;
      case ID_ASCII_FILE:
      {
        wxString wildc = "(*.*)|*.*";
        mafGetOpenMultiFiles("",wildc.c_str(),m_Files);
        m_Gui->Enable(ID_DATA_ORDER,m_Files.size()>0);
        m_Gui->Enable(ID_READ_DATA,m_Files.size()>0);
      }
      break;
      case ID_DATA_ORDER:
      break;
      case ID_COMPONENT_ACTIVE:
      {
        UpdateData();
      }
      break;
      case ID_READ_DATA:
        if (ImportASCII()== MAF_OK) 
        {
          ViewData();
          m_Gui->Enable(ID_SCALAR_LUT,m_Files.size()>0);
          m_Gui->Enable(ID_COMPONENT_ACTIVE,m_Files.size()>0);
        }
      break;
      case ID_SCALAR_LUT:
        CameraUpdate();
      break;
      default:
        mafViewVTK::OnEvent(maf_event);
      break;
    }
  }
  else
  {
    mafEventMacro(*maf_event);
  }
}
//----------------------------------------------------------------------------
int mafViewScalar::ImportASCII()
//----------------------------------------------------------------------------
{
  int import_result = MAF_ERROR;
  int s;
  unsigned nr,nc;
  double *row_array = NULL;
  double *col_array = NULL;
  mafString scalar_name;

  mafASCIIImporterUtility utility;
  for (int t=0; t<m_Files.size(); t++)
  {
    if(utility.ReadFile(m_Files[t].c_str()) == MAF_OK)
    {
      import_result = MAF_OK;
      nr = utility.GetNumberOfRows();
      nc = utility.GetNumberOfCols();
      if (m_DataOrder == DATA_ORDER_ROW)
      {
        for (s=0; s<nr; s++) 
        {
          row_array = new double[nc];
          m_ScalarsArray.push_back(vtkDoubleArray::New());
          scalar_name = "SCALAR_";
          scalar_name << s;
          m_ScalarsArray[m_ScalarsArray.size()-1]->SetName(scalar_name.GetCStr());
          m_ScalarsArray[m_ScalarsArray.size()-1]->SetNumberOfValues(nc);
          utility.ExtractRow(s,row_array);
          m_ScalarsArray[m_ScalarsArray.size()-1]->SetArray(row_array,nc,1);
        }
      }
      else if (m_DataOrder == DATA_ORDER_COLUMN)
      {
        for (s=0; s<nc; s++)
        {
          col_array = new double[nr];
          m_ScalarsArray.push_back(vtkDoubleArray::New());
          scalar_name = "SCALAR_";
          scalar_name << s;
          m_ScalarsArray[m_ScalarsArray.size()-1]->SetName(scalar_name.GetCStr());
          m_ScalarsArray[m_ScalarsArray.size()-1]->SetNumberOfValues(nr);
          utility.ExtractColumn(s,col_array);
          m_ScalarsArray[m_ScalarsArray.size()-1]->SetArray(col_array,nr,1);
        }
      }
    }
    else
    {
      import_result = MAF_ERROR;
    }
  }
  return import_result;
}
//----------------------------------------------------------------------------
void mafViewScalar::ViewData()
//----------------------------------------------------------------------------
{
  unsigned tuple = m_ScalarsArray[0]->GetNumberOfTuples();
  double sr[2];
  m_ScalarsArray[0]->GetRange(sr);
  m_Lut->SetTableRange(sr);

  vtkMAFSmartPointer<vtkPoints> points;
  vtkMAFSmartPointer<vtkCellArray> cells;
  points->SetNumberOfPoints(tuple);
  for (int p=0; p<tuple; p++) 
  {
    points->InsertPoint(p,p,0,*m_ScalarsArray[0]->GetTuple(p));
    cells->InsertNextCell(1,&p);
  }
  
  m_Line->SetPoints(points);
  m_Line->SetVerts(cells);
  m_Line->GetPointData()->SetScalars(m_ScalarsArray[0]);

  m_Mapper->SetInput(m_Line);
  m_Mapper->ScalarVisibilityOn();
  m_Mapper->UseLookupTableScalarRangeOn();
  m_Mapper->SetLookupTable(m_Lut);

  vtkMAFSmartPointer<vtkActor> actor;
  actor->SetMapper(m_Mapper);

  m_Rwi->m_RenFront->AddActor(actor);
  CameraReset();
}
//----------------------------------------------------------------------------
void mafViewScalar::UpdateData()
//----------------------------------------------------------------------------
{
  if (m_ComponentActivated >= m_ScalarsArray.size()) 
  {
    m_ComponentActivated = m_ScalarsArray.size() - 1;
  }
  unsigned tuple = m_ScalarsArray[m_ComponentActivated]->GetNumberOfTuples();
  vtkMAFSmartPointer<vtkCellArray> cells;
  vtkMAFSmartPointer<vtkPoints> points;
  points->SetNumberOfPoints(tuple);
  for (int p=0; p<tuple; p++) 
  {
    points->InsertPoint(p,p,0,*m_ScalarsArray[m_ComponentActivated]->GetTuple(p));
    cells->InsertNextCell(1,&p);
  }

  m_Line->SetPoints(points);
  m_Line->SetVerts(cells);
  m_Line->GetPointData()->SetScalars(m_ScalarsArray[m_ComponentActivated]);
  m_Line->Modified();

  double sr[2];
  m_ScalarsArray[m_ComponentActivated]->GetRange(sr);
  m_Lut->SetTableRange(sr);
  CameraUpdate();
}
