/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVisualPipeCollisionDetection
 Authors: Matteo Giacomoni
 
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

#include "albaVisualPipeCollisionDetection.h"
#include "albaSceneNode.h"
#include "albaGui.h"
#include "albaVME.h"
#include "albaVMEOutputSurface.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkALBACollisionDetectionFilter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkDoubleArray.h"
#include "vtkCellData.h"
#include "vtkColorTransferFunction.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkRenderer.h"
#include "vtkIdList.h"
#include "vtkProperty.h"
#include "vtkPolyDataNormals.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaVisualPipeCollisionDetection);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaVisualPipeCollisionDetection::albaVisualPipeCollisionDetection()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_CollisionFilter = NULL;
  m_Mapper = NULL;
  m_Actor = NULL;
  m_Matrix0 = NULL;
  m_CellToExlude = NULL;
  m_ShowSurfaceToCollide = true;
  m_EnablePipeUpdate = true;
  m_ColorNotCollisionCells[0] = 0.0;
  m_ColorNotCollisionCells[1] = 1.0;
  m_ColorNotCollisionCells[2] = 0.0;
 }
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::Create(albaSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputSurface));
  //To capture matrix changes events
  m_Vme->AddObserver(this);
  albaVMEOutputSurface *surface_output = albaVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
  assert(surface_output);
  surface_output->Update();

  albaNEW(m_Matrix0);
  //Store old matrix to speedup dispacth of events
  m_Matrix0->DeepCopy(m_Vme->GetOutput()->GetAbsMatrix());

  vtkPolyData *pd = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
  pd->Update();

  vtkNEW(m_CollisionFilter);
  m_CollisionFilter->SetInput(0,pd);
  m_CollisionFilter->SetMatrix(0,m_Matrix0->GetVTKMatrix());

  vtkALBASmartPointer<vtkPolyData> output0;
  //If the second input is set
  if (m_SurfacesToCollide.size() != 0)
  {
    for (int i=0;i<m_SurfacesToCollide.size();i++)
    {
      assert(m_SurfacesToCollide[i]->GetOutput()->IsALBAType(albaVMEOutputSurface));
      albaVMEOutputSurface *surfaceOutputToCollide = albaVMEOutputSurface::SafeDownCast(m_SurfacesToCollide[i]->GetOutput());
      assert(surfaceOutputToCollide);
      surfaceOutputToCollide->Update();
      vtkPolyData *pdToCollide = vtkPolyData::SafeDownCast(surfaceOutputToCollide->GetVTKData());
      pdToCollide->Update();

      albaMatrix *m1;
      albaNEW(m1);
      //Store old matrix to speedup dispacth of events
      m1->DeepCopy(m_SurfacesToCollide[i]->GetOutput()->GetAbsMatrix());
      m_CollisionFilter->SetInput(1,pdToCollide);
      m_CollisionFilter->SetMatrix(1,m1->GetVTKMatrix());
      m_Matrix1.push_back(m1);

      output0->DeepCopy(m_CollisionFilter->GetOutput(0));
      output0->Update();
      //Create scalars array to visualize contacts cells
      vtkALBASmartPointer<vtkDoubleArray> contactScalars;
      contactScalars->SetName("CONTACT");
      contactScalars->SetNumberOfTuples(m_CollisionFilter->GetOutput(0)->GetNumberOfCells());
      for (int i=0;i<m_CollisionFilter->GetOutput(0)->GetNumberOfCells();i++)
      {
        contactScalars->SetTuple1(i,0.0);
      }
      vtkDataArray *array = output0->GetFieldData()->GetArray("ContactCells");
      if (array != NULL)
      {
        for (int i=0;i<array->GetNumberOfTuples();i++)
        {
          int value = (int)array->GetTuple1(i);
          contactScalars->SetTuple1(value,1.0);
        }

        output0->GetCellData()->AddArray(contactScalars);
      }
    }
  }
  else//No second input exist
  {
    //Set input of the mapper as first input surface
    output0->DeepCopy(pd);
    output0->Update();
  }

  vtkALBASmartPointer<vtkColorTransferFunction> table;
  table->AddRGBPoint(0,m_ColorNotCollisionCells[0],m_ColorNotCollisionCells[1],m_ColorNotCollisionCells[2]);
  table->AddRGBPoint(1,1,0,0);
  table->Build();

  //Scalars should be visualized
  vtkNEW(m_Mapper);
  m_Mapper->ScalarVisibilityOn();
  //Collision scalars are presents inside cells
  m_Mapper->SetScalarModeToUseCellData();
  m_Mapper->SetLookupTable(table);
  m_Mapper->UseLookupTableScalarRangeOn();
  m_Mapper->SetInput(output0);

  vtkNEW(m_Actor);
  m_Actor->SetMapper(m_Mapper);
  m_AssemblyFront->AddPart(m_Actor);
}
//----------------------------------------------------------------------------
albaVisualPipeCollisionDetection::~albaVisualPipeCollisionDetection()
//----------------------------------------------------------------------------
{
  //Remove observers
  for (int i=0;i<m_SurfacesToCollide.size();i++)
  {
    m_SurfacesToCollide[i]->RemoveObserver(this);
  }
  m_Vme->RemoveObserver(this);
  m_AssemblyFront->RemovePart(m_Actor);
  vtkDEL(m_Actor);
  vtkDEL(m_Mapper);
  vtkDEL(m_CollisionFilter);
  //Delete matrix
  albaDEL(m_Matrix0);
  for (int i=0;i<m_Matrix1.size();i++)
  {
    albaDEL(m_Matrix1[i]);
  }
  //remove actors
  for (int i=0;i<m_SurfacebToCollideActor.size();i++)
  {
    m_RenFront->RemoveActor(m_SurfacebToCollideActor[i]);
    vtkDEL(m_SurfacebToCollideActor[i]);
    vtkDEL(m_SurfaceToCollideMapper[i]);
  }
  //Delete array of cell to exlude
  if (m_CellToExlude != NULL)
  {
    delete []m_CellToExlude;
    m_CellToExlude = NULL;
  }
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::Select(bool sel)
//----------------------------------------------------------------------------
{
  //Store selected status
	m_Selected = sel;
}
//----------------------------------------------------------------------------
albaGUI *albaVisualPipeCollisionDetection::CreateGui()
//----------------------------------------------------------------------------
{
  //create gui
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  m_Gui->Button(ID_SELECT_SURFACE,_("Select Surface:"),"",_("Select a surface to compute collisions"));
  m_Gui->Button(ID_ADD_SURFACE,_("Add Surface:"),"",_("Add a surface to compute collisions"));
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
    case ID_ADD_SURFACE:
      {
        //if user add a surface to compute collision
        albaString title = _("Choose Surface");
        e->SetPointer(&albaVisualPipeCollisionDetection::SurfaceAccept);
        e->SetString(&title);
        e->SetId(VME_CHOOSE);
        albaEventMacro(*e);
        albaVME *surfaceTMP = e->GetVme();
        //if not surface has been selected
        if (surfaceTMP == NULL)
        {
          return;
        }
        AddSurfaceToCollide(surfaceTMP);
        return;
      }
      break;
    case ID_SELECT_SURFACE:
      {
        //if the user select a single surface for collision detection
        albaString title = _("Choose Surface");
        e->SetPointer(&albaVisualPipeCollisionDetection::SurfaceAccept);
        e->SetString(&title);
        e->SetId(VME_CHOOSE);
        albaEventMacro(*e);
        albaVME *surfaceTMP = e->GetVme();
        //if not surface has been selected
        if (surfaceTMP == NULL)
        {
          return;
        }
        SetSurfaceToCollide(surfaceTMP);
        return;
      }
      break;
      default:
        albaEventMacro(*e);
      break;
    }
  }
  else if (alba_event->GetId() == VME_ABSMATRIX_UPDATE)
  {
    //Update the pipeline of visualization
    if (m_EnablePipeUpdate)
    {
    	UpdatePipeline();
    }
  }
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::UpdatePipeline(bool force /* = false */)
//----------------------------------------------------------------------------
{
  //If second input exist
  if (m_SurfacesToCollide.size() != 0 && m_SurfacesToCollide.size() == m_Matrix1.size())
  {
    vtkALBASmartPointer<vtkDoubleArray> contactScalars0;
    contactScalars0->SetName("CONTACT");
    contactScalars0->SetNumberOfTuples(m_Vme->GetOutput()->GetVTKData()->GetNumberOfCells());
    //Create scalars array to visualize contacts cells
    for (int i=0;i<contactScalars0->GetNumberOfTuples();i++)
    {
      //initialize the scalars
      contactScalars0->SetTuple1(i,0.0);
    }
    //for all surfaces to compute collisions
    for (int i=0;i<m_SurfacesToCollide.size();i++)
    {
	    albaMatrix *m0 = m_Vme->GetOutput()->GetAbsMatrix();
	    albaMatrix *m1 = m_SurfacesToCollide[i]->GetOutput()->GetAbsMatrix();
      //if matrix are equals and so it's not necessary compute collision
	    if (!force && (m0->Equals(m_Matrix0) && m1->Equals(m_Matrix1[i])))
	    {
	      return;
	    }
      vtkPolyData *data = vtkPolyData::SafeDownCast(m_SurfacesToCollide[i]->GetOutput()->GetVTKData());
      data->Update();
      if (m_CollisionFilter->GetInput(1) != data)
      {
        m_CollisionFilter->SetInput(1,data);
      }
      //store new matrix
      m_Matrix0->DeepCopy(m0);
      m_Matrix1[i]->DeepCopy(m1);
      m_CollisionFilter->SetCollisionModeToHalfContacts();
      m_CollisionFilter->SetMatrix(0,m_Matrix0->GetVTKMatrix());
      m_CollisionFilter->SetMatrix(1,m_Matrix1[i]->GetVTKMatrix());
      m_CollisionFilter->Update();
      //Create the array with correct scalars to view contact cells
      vtkALBASmartPointer<vtkPolyData> poly;
      poly->DeepCopy(vtkPolyData::SafeDownCast(m_SurfacesToCollide[i]->GetOutput()->GetVTKData()));
      poly->Update();
      vtkDataArray *array1 = m_CollisionFilter->GetOutput(1)->GetFieldData()->GetArray("ContactCells");
      vtkDataArray *arrayToExclude1 = poly->GetCellData()->GetArray(m_ScalarNameToExclude.ToAscii());
      vtkDataArray *array0 = m_CollisionFilter->GetOutput(0)->GetFieldData()->GetArray("ContactCells");
      vtkDataArray *arrayToExclude0 = m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetArray(m_ScalarNameToExclude.ToAscii());
      if (array0 != NULL)
      {
        for (int i=0;i<array0->GetNumberOfTuples();i++)
        {
          bool exclude = false;
          if (arrayToExclude0)
          {
            int value = (int)array0->GetTuple1(i);
            double test = arrayToExclude0->GetTuple1(value);
            if (arrayToExclude0->GetTuple1(value) == TRUE)
            {
              //contactScalars0->SetTuple1(value,0.0);
              exclude = true;
            }
          }
          if (arrayToExclude1)
          {
            int value = (int)array1->GetTuple1(i);
            if (arrayToExclude1->GetTuple1(value) == TRUE)
            {
              exclude = true;
            }
          }
          if (!exclude)
          {
            int value = (int)array0->GetTuple1(i);
            contactScalars0->SetTuple1(value,1.0);
          }
        }
      }
      //Show the surface to collide with correct scalars
      if (m_ShowSurfaceToCollide)
      {
//         vtkALBASmartPointer<vtkPolyData> poly;
//         poly->DeepCopy(vtkPolyData::SafeDownCast(m_SurfacesToCollide[i]->GetOutput()->GetVTKData()));
//         poly->Update();
        vtkALBASmartPointer<vtkTransform> t;
        t->SetMatrix(m1->GetVTKMatrix());
        vtkALBASmartPointer<vtkTransformPolyDataFilter> tpd;
        tpd->SetTransform(t);
        tpd->SetInput(poly);
        tpd->Update();
        vtkALBASmartPointer<vtkDoubleArray> contactScalars1;
        contactScalars1->SetName("CONTACT");
        contactScalars1->SetNumberOfTuples(poly->GetNumberOfCells());
        //Create scalars array to visualize contacts cells
        for (int j=0;j<contactScalars1->GetNumberOfTuples();j++)
        {
          contactScalars1->SetTuple1(j,0.0);
        }        

//         vtkDataArray *array1 = m_CollisionFilter->GetOutput(1)->GetFieldData()->GetArray("ContactCells");
//         vtkDataArray *arrayToExclude1 = poly->GetCellData()->GetArray(m_ScalarNameToExclude.ToAscii());
        if (array1 != NULL)
        {
          for (int j=0;j<array1->GetNumberOfTuples();j++)
          {
            bool exclude = false;
            if (arrayToExclude0)
            {
              int value = (int)array0->GetTuple1(i);
              if (arrayToExclude0->GetTuple1(value) == TRUE)
              {
                //contactScalars0->SetTuple1(value,0.0);
                exclude = true;
              }
            }
            if (arrayToExclude1)
            {
              int value = (int)array1->GetTuple1(i);
              if (arrayToExclude1->GetTuple1(value) == TRUE)
              {
                exclude = true;
              }
            }
            if (!exclude)
            {
              int value = (int)array0->GetTuple1(i);
              contactScalars1->SetTuple1(value,1.0);
            }
          }
        }
        vtkALBASmartPointer<vtkPolyData> polyResult;
        polyResult->DeepCopy(tpd->GetOutput());
        polyResult->Update();
        polyResult->GetCellData()->AddArray(contactScalars1);
        polyResult->GetCellData()->SetActiveScalars("CONTACT");
        polyResult->Update();

        //Add new mapper if a new surface has been added
        if (m_SurfaceToCollideMapper.size() > i)
        {
          m_SurfaceToCollideMapper[i]->SetInput(polyResult);
          m_SurfaceToCollideMapper[i]->Update();
        }
        else
        {
          //use old mapper if no new surface has been added
          vtkALBASmartPointer<vtkColorTransferFunction> table;
          table->AddRGBPoint(0,m_ColorNotCollisionCells[0],m_ColorNotCollisionCells[1],m_ColorNotCollisionCells[2]);
          table->AddRGBPoint(1,1,0,0);
          table->Build();
          vtkPolyDataMapper *mapper;
          vtkNEW(mapper);
          mapper->SetInput(polyResult);
          mapper->ScalarVisibilityOn();
          //Collision scalars are presents inside cells
          mapper->SetScalarModeToUseCellData();
          mapper->SetLookupTable(table);
          mapper->UseLookupTableScalarRangeOn();
          vtkActor *actor;
          vtkNEW(actor);
          actor->SetMapper(mapper);
          //Add new surface to the view
          m_RenFront->AddActor(actor);
          m_SurfaceToCollideMapper.push_back(mapper);
          m_SurfacebToCollideActor.push_back(actor);
        }
      }
    }
    // Store scalars to the output data
    vtkALBASmartPointer<vtkPolyData> output0;
    output0->DeepCopy(m_Vme->GetOutput()->GetVTKData());
    output0->Update();
    output0->GetCellData()->AddArray(contactScalars0);
    output0->GetCellData()->SetActiveScalars("CONTACT");
    output0->Update();
    m_Mapper->SetInput(output0);
    m_Mapper->Update();
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
bool albaVisualPipeCollisionDetection::SurfaceAccept(albaVME*node)
//----------------------------------------------------------------------------
{
  return(node != NULL && node->GetOutput()->IsA("albaVMEOutputSurface"));
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::SetSurfaceToCollide( albaVME *surface )
//----------------------------------------------------------------------------
{
  //use a singole surface to compute collisions
  if (surface == NULL)
  {
    return;
  }
  if (m_SurfacesToCollide.size() != 0)
  {
    for (int i=0;i<m_SurfacesToCollide.size()-1;i++)
    {
	    //Remove observer before change surface to collide
	    m_SurfacesToCollide[i]->RemoveObserver(this);
	    albaDEL(m_Matrix1[i]);
    }
    m_SurfacesToCollide.clear();
    m_Matrix1.clear();
  }
  m_SurfacesToCollide.push_back(surface);
  //To capture matrix changes events
  m_SurfacesToCollide[m_SurfacesToCollide.size()-1]->AddObserver(this);
  albaMatrix *m1;
  albaNEW(m1);
  m1->DeepCopy(m_SurfacesToCollide[m_SurfacesToCollide.size()-1]->GetOutput()->GetAbsMatrix());
  m_Matrix1.push_back(m1);
  UpdatePipeline(true);
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::AddSurfaceToCollide( albaVME *surface )
//----------------------------------------------------------------------------
{
  //use multiple surface to compute collsions
  if (surface == NULL)
  {
    return;
  }
  for (int i=0;i<m_SurfacesToCollide.size();i++)
  {
    if (m_SurfacesToCollide[i] == surface)
    {
      return;
    }
  }
  m_SurfacesToCollide.push_back(surface);
  //To capture matrix changes events
  m_SurfacesToCollide[m_SurfacesToCollide.size()-1]->AddObserver(this);
  albaMatrix *m1;
  albaNEW(m1);
  m1->DeepCopy(m_SurfacesToCollide[m_SurfacesToCollide.size()-1]->GetOutput()->GetAbsMatrix());
  m_Matrix1.push_back(m1);
  UpdatePipeline(true);
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::ShowSurfaceToCollideOn()
//----------------------------------------------------------------------------
{
  m_ShowSurfaceToCollide = true;
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::ShowSurfaceToCollideOff()
//----------------------------------------------------------------------------
{
  m_ShowSurfaceToCollide = false;
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::SetListOfCellToExclude(bool *list)
//----------------------------------------------------------------------------
{
  //set a list of cells to exclude from the collisions computation
  int n = m_Vme->GetOutput()->GetVTKData()->GetNumberOfCells();
  if (m_CellToExlude != NULL)
  {
    delete []m_CellToExlude;
    m_CellToExlude = NULL;
  }
  if (list == NULL)
  {
    m_CellToExlude = NULL;
    return;
  }
  //Store the cells to exclude as boolean array
  m_CellToExlude = new bool[n];
  for (int i=0;i<n;i++)
  {
    m_CellToExlude[i] = list[i];;
  }
}
//----------------------------------------------------------------------------
void albaVisualPipeCollisionDetection::SetOpacity( double opacity )
//----------------------------------------------------------------------------
{
  if (m_Actor)
  {
    m_Actor->GetProperty()->SetOpacity(opacity);
  }
  for (int i=0;i<m_SurfacebToCollideActor.size();i++)
  {
    m_SurfacebToCollideActor[i]->GetProperty()->SetOpacity(opacity);
  }
}

void albaVisualPipeCollisionDetection::SetColorForNotCollisionSurface( double rgb[3] )
{
  m_ColorNotCollisionCells[0] = rgb[0];
  m_ColorNotCollisionCells[1] = rgb[1];
  m_ColorNotCollisionCells[2] = rgb[2];

  vtkALBASmartPointer<vtkColorTransferFunction> table;
  table->AddRGBPoint(0,m_ColorNotCollisionCells[0],m_ColorNotCollisionCells[1],m_ColorNotCollisionCells[2]);
  table->AddRGBPoint(1,1,0,0);
  table->Build();

  m_Mapper->SetLookupTable(table);

  for (int i=0;i<m_SurfaceToCollideMapper.size();i++)
  {
    vtkALBASmartPointer<vtkColorTransferFunction> table;
    table->AddRGBPoint(0,m_ColorNotCollisionCells[0],m_ColorNotCollisionCells[1],m_ColorNotCollisionCells[2]);
    table->AddRGBPoint(1,1,0,0);
    table->Build();

    m_SurfaceToCollideMapper[i]->SetLookupTable(table);
  }
}
