/*=========================================================================
  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAGlobalAxesPolydataActor.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

	This software is distributed WITHOUT ANY WARRANTY; without even
	the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
	PURPOSE.  See the above copyright notice for more information.
=========================================================================*/

#ifndef __vtkALBAGlobalAxesPolydataActor_h
#define __vtkALBAGlobalAxesPolydataActor_h

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
ClassName: vtkALBAGlobalAxesPolydataActor

vtkALBAGlobalAxesPolydataActor - a 3D head used to give camera orientation feedback.
This class is used along with vtkALBAOrientationMarkerWidget to build a render window superimposed rotating head.
A file ..\Config\HelperData\3dHead.vtk containing the 3d head polydata must exists in the current working directory (to be improved)
This is based on ALBA vertical apps dir structure template which is as follows:
albaVerticalAppDir\bin\albaApp.exe (current working directory)
albaVerticalAppDir\Config\HelperData\3dHead.vtk
The 3d head should be contained in [-0.5 , 0.5, -0.5, 0.5, -0.5 , 0.5] (approx head dim should be 1)

Description:
vtkALBAGlobalAxesPolydataActor is an hybrid 3D actor used to represent an anatomical
orientation marker in a scene.  The class consists of a 3D head centered
on the world origin.

Caveats:
vtkALBAGlobalAxesPolydataActor is primarily intended for use with
vtkOrientationMarkerWidget. 

See Also:
vtkALBAOrientationMarkerWidget albaAxes 
*/

class ALBA_EXPORT vtkALBAGlobalAxesPolydataActor : public vtkProp3D
{
public:

	enum ACTOR_TYPE_ENUM
	{
		HEAD,
		BODY,
	};

	vtkALBAGlobalAxesPolydataActor(int type = HEAD);
  static vtkALBAGlobalAxesPolydataActor *New();
	vtkTypeMacro(vtkALBAGlobalAxesPolydataActor,vtkProp3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // For some exporters and other other operations we must be
  // able to collect all the actors or volumes. These methods
  // are used in that process.
  virtual void GetActors(vtkPropCollection *);

  // Description:
  // This is the vtk file containing the 3D head. It must exists.
  std::string GetABSFileName();

	// Description:
	// Set the vtk file containing the 3D head or Body
	void SetFileName(std::string filename);

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
	vtkMTimeType  GetMTime();

  // Description:
  // Get the cube properties.
  vtkProperty *GetProperty();

  // Description:
  // Get the text edges properties.
  vtkProperty *GetTextEdgesProperty();

  // Description:
  // Enable/disable drawing the cube.
  void SetVisibility(int);
  int GetVisibility();

  // Description:
  // Get the assembly so that user supplied transforms can be applied
  vtkAssembly *GetAssembly()
    { return this->Assembly; };
  
  // Description:
  // Set the initial position
  void SetInitialPose(vtkMatrix4x4* initMatrix);

	void SetType(int type);

protected:

	~vtkALBAGlobalAxesPolydataActor();

  vtkPolyDataReader  *Reader;
  vtkActor           *Actor;

  void                UpdateProps();

  vtkAssembly        *Assembly;

	int ActorType;
  std::string FileName;

private:

  vtkALBAGlobalAxesPolydataActor(const vtkALBAGlobalAxesPolydataActor&);  // Not implemented.

	void operator=(const vtkALBAGlobalAxesPolydataActor&);  // Not implemented.
  bool FileExists(const char* filename);
};

#endif
