/*==============================================================================

Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFMultiResolutionActor.h,v $
Language:  C++
Date:      $Date: 2007-11-26 12:55:48 $
Version:   $Revision: 1.1 $
Authors:   Alexander Savenko

================================================================================
Copyright (c) 2007 University of Bedfordshire, UK (www.beds.ac.uk)
All rights reserved.
===============================================================================*/

#ifndef __vtkMAFMultiResolutionActor_h
#define __vtkMAFMultiResolutionActor_h

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "vtkActor.h"
#include "vtkMAFConfigure.h"

//----------------------------------------------------------------------------
// forward refs :
//----------------------------------------------------------------------------
class vtkWindow;
class vtkViewport;
class vtkRenderer;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkMultiThreader;
class vtkDataObject;

//----------------------------------------------------------------------------
// mafPipeSurface :
//----------------------------------------------------------------------------
class VTK_vtkMAF_EXPORT vtkMAFMultiResolutionActor : public vtkActor
{
public:
  vtkTypeMacro(vtkMAFMultiResolutionActor, vtkActor);
  void PrintSelf(ostream& os, vtkIndent indent);

  /** Creates a vtkMAFMultiResolutionActor with the following defaults:*/
  static vtkMAFMultiResolutionActor *New();

  /** This causes the actor to be rendered. It, in turn, will render the actor's
  property and then mapper.  */
  virtual void Render(vtkRenderer *, vtkMapper *);

  /** This method is used internally by the rendering process. We override 
  the superclass method to properly set the estimated render time.*/
  int RenderOpaqueGeometry(vtkViewport *viewport);

  /** Release any graphics resources that are being consumed by this actor.
  The parameter window could be used to determine which graphic
  resources to release.*/
  void ReleaseGraphicsResources(vtkWindow *);


  /** switch ON/OFF multi-threading (default is ON)*/
  vtkGetMacro(MultiThreading, bool);
  vtkSetMacro(MultiThreading, bool);

  /** Enable/Disable box representation (default is ON)*/
  vtkGetMacro(BoxRepresentation, bool);
  vtkSetMacro(BoxRepresentation, bool);

  /** When this objects gets modified, this method also modifies the object.*/
  void Modified();
  
  /** Shallow copy of an LOD actor. Overloads the virtual vtkProp method.*/
  void ShallowCopy(vtkProp *prop);

  //---------------------------------------------------- statistics
  
  /** Get a total number of mappers associated with the actor*/
  int GetNumberOfResolutions() const;
  int GetTotalNumberOfResolutions() const;

  /** Get a pointer to the last drawn mapper (data) or NULL if the actor is out-of-date*/
  vtkMapper *GetLastDrawnMapper();
  vtkPolyData *GetLastDrawnPolyData();

  /** Get usage statistics for a particular resolution. Resolution should be between 0 and (GetTotalNumberOfResolutions() - 1)*/
  int   GetUsage(int resolution);
  double GetRenderTime(int resolution);

  /** Get pointer to a particular mapper. Mapper should be between 0 and (GetTotalNumberOfMappers() - 1)*/
  vtkMapper *GetMapper(int resolution) const;

  /** Get number of threads (0 or 1) spawned by the actor*/
  int GetNumberOfActiveThreads() const { return (this->MultiThreader != NULL && this->ThreadID >= 0) ? 1 : 0; }

protected:
  vtkMAFMultiResolutionActor();
  ~vtkMAFMultiResolutionActor();

  void CreateMultiResolutionModels();
  void DeleteMultiResolutionModels(); // delete out-of-date models and stop the threads
  void CreateDecimatedModels(bool parallel);

  // time when the multi-resolution models were created
  vtkTimeStamp BuildTime;

  // an internal vtkActor object used to render the chosen mapper
  vtkActor *RealActor;

  // mappers ordered by rendering time. Some mappers can be NULL
  enum { FirstDecimatedMapper, /* DecimatedMapper1, */LastDecimatedMapper, OutlineMapper, NumberOfMappers };
  vtkPolyDataMapper  *Mappers[NumberOfMappers];
  vtkMapper          *LastMapperDrawn;
  int                 RedrawMapperIndex; // index of the mapper to redraw
  vtkDataObject      *InputData;
  vtkPolyData        *InputDataCopy;
  int                 MappersUsage[NumberOfMappers + 1];
  double              MappersRenderTimeTotal[NumberOfMappers + 1];

  bool                BoxRepresentation;

  // thread control
  bool                MultiThreading;
  vtkMultiThreader   *MultiThreader;
  int                 ThreadID;
  bool                AbortFlag;

private:
  vtkMAFMultiResolutionActor(const vtkMAFMultiResolutionActor&);  // Not implemented.
  void operator=(const vtkMAFMultiResolutionActor&);           // Not implemented.

  void StopAndDeleteThread();

  // friends
  friend void *DecimateThreadFunction(void *arg);
  friend void DecimateProgressFunction(void *arg);
};

#endif
