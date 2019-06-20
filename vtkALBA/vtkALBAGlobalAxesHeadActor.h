/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAGlobalAxesHeadActor.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkALBAGlobalAxesHeadActor_h
#define __vtkALBAGlobalAxesHeadActor_h

#include "vtkProp3D.h"
#include "albaConfigure.h"

class vtkActor;
class vtkAppendPolyData;
class vtkAssembly;
class vtkPolyDataReader;
class vtkFeatureEdges;
class vtkPropCollection;
class vtkProperty;
class vtkRenderer;
class vtkTransform;
class vtkTransformFilter;

class vtkVectorText;

/**
Classname: vtkALBAGlobalAxesHeadActor

vtkALBAGlobalAxesHeadActor - a 3D head used to give camera orientation feedback.
This class is used along with vtkALBAOrientationMarkerWidget to build a render window superimposed rotating head.
A file ..\Config\HelperData\3dHead.vtk containing the 3d head polydata must exists in the current working directory (to be improved)
This is based on ALBA vertical apps dir structure template which is as follows:
albaVerticalAppDir\bin\albaApp.exe (current working directory)
albaVerticalAppDir\Config\HelperData\3dHead.vtk
The 3d head should be contained in [-0.5 , 0.5, -0.5, 0.5, -0.5 , 0.5] (approx head dim should be 1)

Description:
vtkALBAGlobalAxesHeadActor is an hybrid 3D actor used to represent an anatomical
orientation marker in a scene.  The class consists of a 3D head centered
on the world origin.

Caveats:
vtkALBAGlobalAxesHeadActor is primarily intended for use with
vtkOrientationMarkerWidget. 

See Also:
vtkALBAOrientationMarkerWidget albaAxes 

*/

class ALBA_EXPORT vtkALBAGlobalAxesHeadActor : public vtkProp3D
{
public:
  static vtkALBAGlobalAxesHeadActor *New();
  vtkTypeRevisionMacro(vtkALBAGlobalAxesHeadActor,vtkProp3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // For some exporters and other other operations we must be
  // able to collect all the actors or volumes. These methods
  // are used in that process.
  virtual void GetActors(vtkPropCollection *);

  // Description:
  // This is the vtk file containing the 3D head. It must exists.
  std::string GetHeadABSFileName();

  // Description:
  // Support the standard render methods.
  virtual int RenderOpaqueGeometry(vtkViewport *viewport);
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport *viewport);

  // Description:
  // Does this prop have some translucent polygonal geometry?
  virtual int HasTranslucentPolygonalGeometry();

  // Description:
  // Shallow copy of an axes actor. Overloads the virtual vtkProp method.
  void ShallowCopy(vtkProp *prop);

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax). (The
  // method GetBounds(double bounds[6]) is available from the superclass.)
  void GetBounds(double bounds[6]);
  double *GetBounds();

  // Description:
  // Get the actors mtime plus consider its properties and texture if set.
  unsigned long int GetMTime();

  // Description:
  // Get the cube properties.
  vtkProperty *GetHeadProperty();

  // Description:
  // Get the text edges properties.
  vtkProperty *GetTextEdgesProperty();

  // Description:
  // Enable/disable drawing the cube.
  void SetHeadVisibility(int);
  int GetHeadVisibility();

  // Description:
  // Get the assembly so that user supplied transforms can be applied
  vtkAssembly *GetAssembly()
    { return this->Assembly; };
  
  // Description:
  // Set the initial position
  void SetInitialPose(vtkMatrix4x4* initMatrix);

protected:
  vtkALBAGlobalAxesHeadActor();
  ~vtkALBAGlobalAxesHeadActor();

  vtkPolyDataReader  *HeadReader;
  vtkActor           *HeadActor;

  void                UpdateProps();

  vtkAssembly        *Assembly;

  std::string HeadFileName;

private:
  vtkALBAGlobalAxesHeadActor(const vtkALBAGlobalAxesHeadActor&);  // Not implemented.
  void operator=(const vtkALBAGlobalAxesHeadActor&);  // Not implemented.
  bool FileExists(const char* filename);
};

#endif
