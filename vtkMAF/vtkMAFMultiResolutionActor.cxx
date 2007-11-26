/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFMultiResolutionActor.cxx,v $
Language:  C++
Date:      $Date: 2007-11-26 12:55:48 $
Version:   $Revision: 1.1 $
Authors:   Alexander Savenko

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/

#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "vtkObjectFactory.h"
#include "vtkMultiThreader.h"
#include "vtkOutlineSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkDecimatePro.h"
#include "vtkMatrix4x4.h"
#include "vtkProperty.h"
#include "vtkTexture.h"
#include "vtkDataSet.h"

#include "vtkMAFMultiResolutionActor.h"

//---------------------------- static functions -------------------------------
static void *DecimateThreadFunction(void *ptr);
static void DecimateProgressFunction(void *arg);

//-----------------------------------------------------------------------------
vtkMAFMultiResolutionActor* vtkMAFMultiResolutionActor::New()
//-----------------------------------------------------------------------------
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMAFMultiResolutionActor");
  if(ret)
    return (vtkMAFMultiResolutionActor*)ret;
  
  // If the factory was unable to create the object, then create it here.
  return new vtkMAFMultiResolutionActor;
}

//----------------------------------------------------------------------------
vtkMAFMultiResolutionActor::vtkMAFMultiResolutionActor()
//-----------------------------------------------------------------------------
{
  // get a hardware dependent actor and mappers
  this->RealActor = vtkActor::New();
  vtkMatrix4x4 *m = vtkMatrix4x4::New();
  this->RealActor->SetUserMatrix(m);
  m->Delete();
  
  for (int i = 0; i < NumberOfMappers; i++)
    this->Mappers[i] = NULL;
  
  this->MultiThreading = true;
  this->MultiThreader = NULL;
  this->ThreadID = -1;
  this->LastMapperDrawn = NULL;
  this->InputData = NULL;
  this->InputDataCopy = NULL;
  this->BoxRepresentation = true;
}
//----------------------------------------------------------------------------
vtkMAFMultiResolutionActor::~vtkMAFMultiResolutionActor()
//-----------------------------------------------------------------------------
{
  this->RealActor->Delete();
  
  for (int i = 0; i < NumberOfMappers; i++)
  {
    if (this->Mappers[i])
      this->Mappers[i]->Delete();
  }
  
  // delete the thread (note that it can still be running)
  if (MultiThreader) 
  {
    // each thread adds a reference to vtkMAFMultiResolutionActor object, so it cannot be deleted
    // until the thread is finished
    assert(ThreadID < 0); 
    MultiThreader->Delete();
  }
  if (this->InputDataCopy != NULL)
    this->InputDataCopy->Delete();
}
//----------------------------------------------------------------------------
void vtkMAFMultiResolutionActor::PrintSelf(ostream& os, vtkIndent indent) 
//-----------------------------------------------------------------------------
{
  vtkActor::PrintSelf(os, indent);
  
  for (int i = 0; i < NumberOfMappers; i++) 
  {
    if (this->Mappers[i] == NULL || this->Mappers[i]->GetInput() == NULL)
      continue;
    if (this->Mappers[i]->GetInput()->GetNumberOfStrips() > 0)
      os << indent << "Mapper " << i << ": " << this->Mappers[i]->GetInput()->GetNumberOfStrips() << " strips" << endl;
    else
      os << indent << "Mapper " << i << ": " << this->Mappers[i]->GetInput()->GetNumberOfPolys() << " polygons" << endl;
  }

  os << indent << "Multithreading: " << this->MultiThreading << endl;
  os << indent << "Active threads: " << this->GetNumberOfActiveThreads() << endl;
}

//----------------------------------------------------------------------------
void vtkMAFMultiResolutionActor::Render(vtkRenderer *ren, vtkMapper *vtkNotUsed(m)) 
//-----------------------------------------------------------------------------
{
  static int renderCounter = 0; // used to recalculate rendering time each X renderings

  // error checking  
  if (this->Mapper == NULL || this->Mapper->GetInput() == NULL) 
  {
    if (this->Mapper == NULL)
      vtkErrorMacro("No mapper for actor.")
    else
      vtkErrorMacro("No data for actor.");
    return;
  }
  
  // If there are no models or the primary mapper has changed update the models
  bool firstRendering = false;
  if (this->Mappers[OutlineMapper] == NULL && this->ThreadID < 0 ||
      this->Mapper->GetInput()->GetMTime() > this->BuildTime ||
      this->InputData != this->Mapper->GetInput()) 
  {
    this->DeleteMultiResolutionModels();
    this->RedrawMapperIndex = -1;
    firstRendering = true;
  }
  
  // Figure out which resolution to use 
  vtkMapper *bestMapper = this->Mapper;

  double allocTime  = this->AllocatedRenderTime;
  double renderTime = this->GetRenderTime(0);
  int mapperIndex = 0;
  // heuristic: if the initial rendering time was wrong it will allow to recalculate it
  if ((renderCounter++ % 10) == 0)
    allocTime *= 1.2f;

  for (int i = 0; i <= ((this->BoxRepresentation || this->Mappers[1] == NULL) ? OutlineMapper : LastDecimatedMapper) && renderTime > allocTime && this->RedrawMapperIndex >= i; i++) 
  {
    if (this->Mappers[i] != NULL) 
    {
      renderTime = this->GetRenderTime(i + 1);
      bestMapper = this->Mappers[i]; // not (i + 1)
      mapperIndex = i + 1;
      if (renderTime == 0)
        this->RedrawMapperIndex = -1; // update render time for other mappers
      else if (this->ThreadID < 0 && (i == LastDecimatedMapper || this->Mappers[i + 1] == NULL))
        renderTime *= 0.25f;
    }
  }
  
  this->RedrawMapperIndex++;
  ///////////////////////////////////// rendering
  
  // render the properties
  if (!this->Property)
    this->GetProperty();
  this->Property->Render(this, ren);
 
  if (this->BackfaceProperty) 
  {
    this->BackfaceProperty->BackfaceRender(this, ren);
    this->RealActor->SetBackfaceProperty(this->BackfaceProperty);
  }
  this->RealActor->SetProperty(this->Property);
  
  // render the texture
  if (this->Texture)
    this->Texture->Render(ren);
  
  // make sure the device has the same matrix
  vtkMatrix4x4 *matrix = this->RealActor->GetUserMatrix();
  this->GetMatrix(matrix);
  
  // Store information on time it takes to render.
  // We might want to estimate time from the number of polygons in mapper.
  if (firstRendering)
    this->Mapper->Update(); // to ensure that updating the mapper does not affect render time too much

  this->RealActor->Render(ren, bestMapper);
  this->EstimatedRenderTime = bestMapper->GetTimeToDraw();
  this->MappersRenderTimeTotal[mapperIndex] += this->EstimatedRenderTime;
  this->MappersUsage[mapperIndex]++;
  this->LastMapperDrawn = bestMapper;

  if (firstRendering)
    CreateMultiResolutionModels();

} // Render()

//----------------------------------------------------------------------------
vtkMapper *vtkMAFMultiResolutionActor::GetLastDrawnMapper() 
//----------------------------------------------------------------------------
{
  if (this->Mapper == NULL || this->Mapper->GetInput() == NULL || this->InputData != this->Mapper->GetInput() ||
      this->InputData->GetMTime() > (unsigned long)this->BuildTime)
    return NULL;
  return this->LastMapperDrawn;
} // GetLastDrawnMapper()

//----------------------------------------------------------------------------
vtkPolyData *vtkMAFMultiResolutionActor::GetLastDrawnPolyData() 
//----------------------------------------------------------------------------
{
  vtkPolyDataMapper *mapper = vtkPolyDataMapper::SafeDownCast(this->GetLastDrawnMapper());
  return mapper ? mapper->GetInput() : NULL;
} // GetLastDrawnPolyData()

//--------------------------------------------------------------------------
int vtkMAFMultiResolutionActor::GetNumberOfResolutions() const 
//----------------------------------------------------------------------------
{
  int count = (this->Mapper == NULL || this->Mapper->GetInput() == NULL) ? 0 : 1;
  for (int i = 0; i < NumberOfMappers; i++) 
  {
    if (this->Mappers[i] != 0)
      count++;
  }
  return count;
} // GetNumberOfResolutions()

//--------------------------------------------------------------------------
int vtkMAFMultiResolutionActor::GetTotalNumberOfResolutions() const 
//----------------------------------------------------------------------------
{
  return NumberOfMappers + 1;
}

//--------------------------------------------------------------------------
vtkMapper *vtkMAFMultiResolutionActor::GetMapper(int index) const 
//----------------------------------------------------------------------------
{
  if (index == 0)
    return this->Mapper;
  if (index > 0 && index <= NumberOfMappers)
    return this->Mappers[index - 1];
  return NULL;
}

//--------------------------------------------------------------------------
int vtkMAFMultiResolutionActor::GetUsage(int mapper) 
//----------------------------------------------------------------------------
{
  if (this->Mapper == NULL || this->Mapper->GetInput() == NULL || this->InputData != this->Mapper->GetInput() ||
      this->InputData->GetMTime() > (unsigned long)this->BuildTime || mapper < 0 || mapper > NumberOfMappers)
    return 0;
  if (mapper == 0 || this->Mappers[mapper - 1] != NULL)
    return this->MappersUsage[mapper];
  return 0;
}

//--------------------------------------------------------------------------
double vtkMAFMultiResolutionActor::GetRenderTime(int mapper) 
//----------------------------------------------------------------------------
{
  if (this->Mapper == NULL || this->Mapper->GetInput() == NULL || this->InputData != this->Mapper->GetInput() ||
      this->InputData->GetMTime() > (unsigned long)this->BuildTime || mapper < 0 || mapper > NumberOfMappers)
    return -1.f;
  if ((mapper == 0 || this->Mappers[mapper - 1] != NULL) && this->MappersUsage[mapper] > 0)
    return (this->MappersRenderTimeTotal[mapper] / this->MappersUsage[mapper]);
  return -1.f;
}

//--------------------------------------------------------------------------
int vtkMAFMultiResolutionActor::RenderOpaqueGeometry(vtkViewport *vp) 
//----------------------------------------------------------------------------
{
  int          renderedSomething = 0; 
  vtkRenderer  *ren = (vtkRenderer *)vp;
  
  if (this->Mapper == NULL || this->Mapper->GetInput() == NULL)
    return 0;
  
  // make sure we have a property
  if (this->Property == NULL)
    // force creation of a property
    this->GetProperty();
  
  // is this actor opaque ?
  if (this->GetIsOpaque()) 
  {
    this->Property->Render(this, ren);
    
    // render the back face property
    if (this->BackfaceProperty)
      this->BackfaceProperty->BackfaceRender(this, ren);
    
    // render the texture 
    if (this->Texture)
      this->Texture->Render(ren);

    this->Render(ren,this->Mapper);
    
    renderedSomething = 1;
  }
  
  return renderedSomething;
}
//----------------------------------------------------------------------------
void vtkMAFMultiResolutionActor::ReleaseGraphicsResources(vtkWindow *renWin) 
//----------------------------------------------------------------------------
{
  vtkActor::ReleaseGraphicsResources(renWin);
  
  // broadcast the message down to the individual LOD mappers
  for (int i = 0; i < NumberOfMappers; i++) 
  {
    if (this->Mappers[i] != NULL)
      this->Mappers[i]->ReleaseGraphicsResources(renWin);
  }
}

//----------------------------------------------------------------------------
void vtkMAFMultiResolutionActor::CreateMultiResolutionModels() 
//----------------------------------------------------------------------------
{
  assert(this->Mapper && this->Mapper->GetInput());
  this->InputData = this->Mapper->GetInput();
  
  assert(this->MultiThreader == NULL);

  // create outline mapper
  // it should be very quick so do not spawn threads
  if (this->Mappers[OutlineMapper] == NULL)
    this->Mappers[OutlineMapper] = vtkPolyDataMapper::New();
  vtkOutlineSource *outline = vtkOutlineSource::New();
  outline->SetBounds(this->Mapper->GetInput()->GetBounds()); // should we update the mapper?
  outline->Update();
  this->Mappers[OutlineMapper]->SetInput(outline->GetOutput());
  outline->Delete();

  // create other mappers
  if (vtkPolyData::SafeDownCast(this->Mapper->GetInput()) == NULL) // not polydata
    return;

  // copy the data
  assert(this->InputDataCopy == NULL);
  if (this->MultiThreading) 
  {
    this->InputDataCopy = vtkPolyData::New();
    this->InputDataCopy->DeepCopy(this->InputData);
  }

  if (this->MultiThreading) 
  {
    this->MultiThreader = vtkMultiThreader::New();
    this->AbortFlag = false;
    this->ThreadID = MultiThreader->SpawnThread((vtkThreadFunctionType)DecimateThreadFunction, this);
    if (ThreadID < 0)
      this->CreateDecimatedModels(false);  
  }
  else 
  { // single-threaded version
    this->CreateDecimatedModels(false);
  }
  this->BuildTime.Modified();
}

//----------------------------------------------------------------------------
void vtkMAFMultiResolutionActor::DeleteMultiResolutionModels() 
//----------------------------------------------------------------------------
{
  // stop any threads
  if (this->MultiThreader)
    StopAndDeleteThread(); // ask the thread to stop and wait until it finishes
  this->AbortFlag = false;
  
  // delete old mappers
  int i;
  for (i = 0; i < NumberOfMappers; i++) 
  {
    if (this->Mappers[i]) 
    {
      this->Mappers[i]->Delete();
      this->Mappers[i] = NULL;
    }
  }
  
  // clear statistics
  for (i = 0; i <= NumberOfMappers; i++) 
  {
    this->MappersUsage[i] = 0;
    this->MappersRenderTimeTotal[i] = 0;
  }

  // unreferenced input data
  if (this->InputDataCopy)
    this->InputDataCopy->Delete();
  this->InputDataCopy = NULL;
  this->InputData = NULL;
}

//----------------------------------------------------------------------------
void *DecimateThreadFunction(void *arg) 
//----------------------------------------------------------------------------
{
  ThreadInfoStruct *info = (ThreadInfoStruct *)arg;
  vtkMAFMultiResolutionActor* actor = (vtkMAFMultiResolutionActor*)info->UserData;
  actor->CreateDecimatedModels(true);
  return VTK_THREAD_RETURN_VALUE;
}

struct DecimateProgressStruct 
{
  vtkProcessObject        *process;
  vtkMAFMultiResolutionActor *actor;
};

//----------------------------------------------------------------------------
void DecimateProgressFunction(void *arg) 
//----------------------------------------------------------------------------
{
  DecimateProgressStruct *info = (DecimateProgressStruct*)(arg);
  if (info->actor->AbortFlag)
    info->process->AbortExecuteOn();
}

//-------------------------------------------------------------------------
// this method can be called from a thread. however it is guaranteed that this
// method cannot be executed in two threads simultaneously (for the same object, of course)
void vtkMAFMultiResolutionActor::CreateDecimatedModels(bool parallel) 
//----------------------------------------------------------------------------
{
  // do not to delete the object until the thread finishes
  vtkObject *tmpObject = NULL;
  if (parallel) 
  {
    tmpObject = vtkObject::New();
    this->Register(tmpObject);
  }

  // create the models
  assert(!parallel || this->InputDataCopy != NULL);
  vtkPolyData *inputData  = (this->InputDataCopy != NULL) ? this->InputDataCopy : vtkPolyData::SafeDownCast(this->InputData);
  vtkPolyData *outputData = NULL;

  // initialize the filters
  vtkDecimatePro *decimator = vtkDecimatePro::New();
  decimator->SetTargetReduction(0.5f);              // half of the polygons
  // flags
  decimator->BoundaryVertexDeletionOn();
  decimator->PreserveTopologyOff();
  decimator->SplittingOn();
  decimator->AccumulateErrorOn();

  DecimateProgressStruct progressStruct;
  progressStruct.process = decimator;
  progressStruct.actor   = this;
  //decimator->SetProgressMethod(DecimateProgressFunction, &progressStruct);

  // if rendering time is known, use it to choose the best parameters
  int   numberOfModels = LastDecimatedMapper - FirstDecimatedMapper + 1;
  double firstModelReduction = 0.5f, secondModelReduction = 0.5f;
  if (this->MappersUsage[0] > 0) 
  {
    double renderTime = (this->MappersUsage[0] == 1) ? (0.75f * this->MappersRenderTimeTotal[0])
                                                    : (this->MappersRenderTimeTotal[0] / this->MappersUsage[0]);
    if (inputData && inputData->GetNumberOfCells() > 100000)
      renderTime += 0.1f;
    if (renderTime < 0.01f)
      numberOfModels = 0;
    else if (renderTime < 0.6f)
      numberOfModels = 1;
    else
      numberOfModels = 2;

    firstModelReduction = 1.f - (numberOfModels == 2 ? 0.8f : 0.03f) / (renderTime + 0.01f);
    if (firstModelReduction < 0.2f)
      firstModelReduction = 0.2f;
    else if (firstModelReduction > 0.9f)
      firstModelReduction = 0.9f;
    decimator->SetTargetReduction(firstModelReduction);

    renderTime *= (1.f - firstModelReduction);
    secondModelReduction = 1.f - 0.03f / (renderTime + 0.01f);
  }

  // decimation
  int i;
  for (i = FirstDecimatedMapper; i < (FirstDecimatedMapper + numberOfModels) && inputData != NULL; i++) 
  {
    decimator->SetInput(inputData);
    // free unused memory
    if (inputData != this->InputDataCopy && this->InputDataCopy != NULL) 
    {
      this->InputDataCopy->Delete();
      this->InputDataCopy = NULL;
    }
    try 
    {
      decimator->Update();
    }
    catch(...) 
    {
      assert(false);
    }
    if (this->AbortFlag)
      break;

    if (decimator->GetOutput()->GetNumberOfPolys() < 24)
      break;

    outputData = vtkPolyData::New();
    outputData->ShallowCopy(decimator->GetOutput());
    if (this->Mappers[i] == NULL) 
    {
      vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
      mapper->SetInput(outputData);
      mapper->SetImmediateModeRendering(this->Mapper->GetImmediateModeRendering());
      outputData->Delete();   // delete extra reference
      assert(this->Mappers[i] == NULL);
      this->Mappers[i] = mapper;
    }
    else 
    {
      this->Mappers[i]->SetInput(outputData);
      outputData->Delete();   // delete extra reference
    }
    inputData = outputData;
    decimator->SetTargetReduction(secondModelReduction);
  }
  decimator->Delete();

  // free memory
  if (this->InputDataCopy)
    this->InputDataCopy->Delete();
  this->InputDataCopy = NULL;
  for (i = FirstDecimatedMapper; i < (FirstDecimatedMapper + numberOfModels) && inputData != NULL; i++)
    this->Mappers[i]->GetInput()->DeleteCells();
  // remove the lock from the object
  if (parallel) 
  {
    this->UnRegister(tmpObject);
    tmpObject->Delete();
    this->ThreadID = -1;
  }
}

//----------------------------------------------------------------------------
void vtkMAFMultiResolutionActor::Modified() 
//----------------------------------------------------------------------------
{
  this->RealActor->Modified();
  this->vtkActor::Modified();
}

//----------------------------------------------------------------------------
void vtkMAFMultiResolutionActor::ShallowCopy(vtkProp *prop) 
//----------------------------------------------------------------------------
{
  vtkMAFMultiResolutionActor *a = vtkMAFMultiResolutionActor::SafeDownCast(prop);
  if ( a != NULL ) 
  {
    a->SetMultiThreading(MultiThreading);
    for (int i = 0; i < NumberOfMappers; i++)
    {
      if (a->Mappers[i] != NULL)
        a->Mappers[i]->Delete();
      a->Mappers[i] = this->Mappers[i];
      a->Mappers[i]->Register(a); // reference counting
    }
  }
  
  // Now do superclass
  this->vtkActor::ShallowCopy(prop);
}

//------------------------------------------------------------------------------
#define MAX_WAIT_IN_SEC 20

//----------------------------------------------------------------------------
void vtkMAFMultiResolutionActor::StopAndDeleteThread() 
//----------------------------------------------------------------------------
{
  if (this->MultiThreader == NULL)
    return;
  if (this->ThreadID < 0) 
  {
    this->MultiThreader->Delete();
    this->MultiThreader = NULL;
    return;
  }
  this->AbortFlag = true;

  for (int wi = 0; wi < (MAX_WAIT_IN_SEC * 2) && (this->ThreadID >= 0); wi++) 
  {
#ifdef WIN32
    Sleep(500);
#elif defined(__FreeBSD__) || defined(__linux__) || defined(sgi)
    struct timespec sleep_time, dummy;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 500000000;
    nanosleep(&sleep_time, &dummy);
#endif
  }
  if (this->ThreadID >= 0) 
  { // terminate the thread (can result in MEMORY LEAKS!!!)
    this->MultiThreader->TerminateThread(this->ThreadID);
    this->ThreadID = -1;
#if defined(DEBUG) | defined(_DEBUG)
    assert(false); // this is not an error. the assert means that the decimating process check abort flag too rarely
#endif
  }
  this->MultiThreader->Delete();
  this->MultiThreader = NULL;
  // clean the data
  for (int i = 0; i < NumberOfMappers; i++) 
  {
    if (this->Mappers[i]) 
    {
      this->Mappers[i]->Delete();
      this->Mappers[i] = NULL;
    }
  }
  
  this->AbortFlag = false;
  return;
}
