/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAAnnotatedCubeActor.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
Classname: vtkAnnotatedCubeActor - a 3D cube with face labels

Ported from VTK 5 to ALBA by Stefano Perticoni
This class is used along with vtkALBAOrientationMarkerWidget to build a render window superimposed rotating cube with
anatomical markers. 

Description:
vtkAnnotatedCubeActor is a hybrid 3D actor used to represent an anatomical
orientation marker in a scene.  The class consists of a 3D unit cube centered
on the origin with each face labelled in correspondance to a particular
coordinate direction.  For example, with Cartesian directions, the user
defined text labels could be: +X, -X, +Y, -Y, +Z, -Z, while for anatomical
directions: A, P, L, R, S, I.  Text is automatically centered on each cube
face and is not restriceted to single characters. In addition to or in
replace of a solid text label representation, the outline edges of the labels
can be displayed.  The individual properties of the cube, face labels
and text outlines can be manipulated as can their visibility.

Caveats:
vtkAnnotatedCubeActor is primarily intended for use with
vtkOrientationMarkerWidget. The cube face text is generated by vtkVectorText
and therefore the font attributes are restricted.

See Also:
vtkAxesActor vtkALBAOrientationMarkerWidget vtkVectorText albaAxes 
*/

#ifndef __vtkALBAAnnotatedCubeActor_h
#define __vtkALBAAnnotatedCubeActor_h

#include "vtkProp3D.h"
#include "albaConfigure.h"

class vtkActor;
class vtkAppendPolyData;
class vtkAssembly;
class vtkCubeSource;
class vtkFeatureEdges;
class vtkPropCollection;
class vtkProperty;
class vtkRenderer;
class vtkTransform;
class vtkTransformFilter;
class vtkVectorText;

class ALBA_EXPORT vtkALBAAnnotatedCubeActor : public vtkProp3D
{
public:
  static vtkALBAAnnotatedCubeActor *New();
  vtkTypeRevisionMacro(vtkALBAAnnotatedCubeActor,vtkProp3D);
  void PrintSelf(ostream& os, vtkIndent indent);

  /** For some exporters and other other operations we must be
  able to collect all the actors or volumes. These methods
  are used in that process. */
  virtual void GetActors(vtkPropCollection *); 

  /** Support the standard render methods. */
  virtual int RenderOpaqueGeometry(vtkViewport *viewport);

  /** Support the standard render methods. */
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport *viewport);

  /** Does this prop have some translucent polygonal geometry? */
  virtual int HasTranslucentPolygonalGeometry();

  /** Shallow copy of an axes actor. Overloads the virtual vtkProp method. */
  void ShallowCopy(vtkProp *prop);

  /** Release any graphics resources that are being consumed by this actor.
  The parameter window could be used to determine which graphic resources to release. */
  void ReleaseGraphicsResources(vtkWindow *);

  /** Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax). (The
  method GetBounds(double bounds[6]) is available from the superclass.) */
  void GetBounds(double bounds[6]);
  
  /** Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax). (The
  method GetBounds(double bounds[6]) is available from the superclass.) */
  double *GetBounds();

  /**  Get the actors mtime plus consider its properties and texture if set. */
  unsigned long int GetMTime();

  /** Set the scale factor for the face text */
  void SetFaceTextScale(double);

  /** Get the scale factor for the face text */
  vtkGetMacro(FaceTextScale, double);

  /** Get the individual face text properties. */
  vtkProperty *GetXPlusFaceProperty();

  /** Get the individual face text properties. */
  vtkProperty *GetXMinusFaceProperty();

  /** Get the individual face text properties. */
  vtkProperty *GetYPlusFaceProperty();

  /** Get the individual face text properties. */
  vtkProperty *GetYMinusFaceProperty();

  /** Get the individual face text properties. */
  vtkProperty *GetZPlusFaceProperty();

  /** Get the individual face text properties. */
  vtkProperty *GetZMinusFaceProperty();

  /** Get the cube properties. */
  vtkProperty *GetCubeProperty();

  /** Get the text edges properties. */
  vtkProperty *GetTextEdgesProperty();

  /** Set the face text. */
  vtkSetStringMacro( XPlusFaceText );

  /** Get the face text. */
  vtkGetStringMacro( XPlusFaceText );

  /** Set the face text. */
  vtkSetStringMacro( XMinusFaceText );

  /** Get the face text. */
  vtkGetStringMacro( XMinusFaceText );

  /** Set the face text. */
  vtkSetStringMacro( YPlusFaceText );

  /** Get the face text. */
  vtkGetStringMacro( YPlusFaceText );
  
  /** Set the face text. */
  vtkSetStringMacro( YMinusFaceText );

  /** Get the face text. */
  vtkGetStringMacro( YMinusFaceText );

  /** Set the face text. */
  vtkSetStringMacro( ZPlusFaceText );

  /** Get the face text. */
  vtkGetStringMacro( ZPlusFaceText );

  /** Set the face text. */
  vtkSetStringMacro( ZMinusFaceText );
  
  /** Get the face text. */
  vtkGetStringMacro( ZMinusFaceText );

  
  /** Enable/disable drawing the vector text edges. */
  void SetTextEdgesVisibility(int);
  int GetTextEdgesVisibility();

  /** Enable/disable drawing the cube. */
  void SetCubeVisibility(int);

  /** Enable/disable drawing the cube. */
  int GetCubeVisibility();

  /** Enable/disable drawing the vector text. */
  void SetFaceTextVisibility(int);

  /** Enable/disable drawing the vector text. */
  int GetFaceTextVisibility();

  /** Augment individual face text orientations. */
  vtkSetMacro(XFaceTextRotation,double);

  /** Augment individual face text orientations. */
  vtkGetMacro(XFaceTextRotation,double);

  /** Augment individual face text orientations. */
  vtkSetMacro(YFaceTextRotation,double);

  /** Augment individual face text orientations. */
  vtkGetMacro(YFaceTextRotation,double);

  /** Augment individual face text orientations. */
  vtkSetMacro(ZFaceTextRotation,double);

  /** Augment individual face text orientations. */
  vtkGetMacro(ZFaceTextRotation,double);

  /** Get the assembly so that user supplied transforms can be applied */
  vtkAssembly *GetAssembly()
    { return this->Assembly; }

protected:
  vtkALBAAnnotatedCubeActor();
  ~vtkALBAAnnotatedCubeActor();

  vtkCubeSource      *CubeSource;
  vtkActor           *CubeActor;

  vtkAppendPolyData  *AppendTextEdges;
  vtkFeatureEdges    *ExtractTextEdges;
  vtkActor           *TextEdgesActor;

  void                UpdateProps();

  char               *XPlusFaceText;
  char               *XMinusFaceText;
  char               *YPlusFaceText;
  char               *YMinusFaceText;
  char               *ZPlusFaceText;
  char               *ZMinusFaceText;

  double              FaceTextScale;

  double              XFaceTextRotation;
  double              YFaceTextRotation;
  double              ZFaceTextRotation;

  vtkVectorText      *XPlusFaceVectorText;
  vtkVectorText      *XMinusFaceVectorText;
  vtkVectorText      *YPlusFaceVectorText;
  vtkVectorText      *YMinusFaceVectorText;
  vtkVectorText      *ZPlusFaceVectorText;
  vtkVectorText      *ZMinusFaceVectorText;

  vtkActor           *XPlusFaceActor;
  vtkActor           *XMinusFaceActor;
  vtkActor           *YPlusFaceActor;
  vtkActor           *YMinusFaceActor;
  vtkActor           *ZPlusFaceActor;
  vtkActor           *ZMinusFaceActor;

  vtkTransformFilter *TransformFilter;
  vtkTransform       *Transform;

  vtkAssembly        *Assembly;

private:
  vtkALBAAnnotatedCubeActor(const vtkALBAAnnotatedCubeActor&);  // Not implemented.
  void operator=(const vtkALBAAnnotatedCubeActor&);  // Not implemented.
};

#endif
