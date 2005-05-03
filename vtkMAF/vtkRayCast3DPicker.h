/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: vtkRayCast3DPicker.h,v $
  Language:  C++
  Date:      $Date: 2005-05-03 05:57:32 $
  Version:   $Revision: 1.2 $
  Authors:   Michele Diegoli
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __vtkRayCast3DPicker_h
#define __vtkRayCast3DPicker_h

#include "vtkMAFConfigure.h"
#include "vtkAbstractPropPicker.h"

class vtkAbstractMapper3D;
class vtkDataSet;
class vtkTransform;
class vtkActorCollection;
class vtkProp3DCollection;
class vtkPoints;
class vtkGenericCell;

/** class for 3D geometric pickers (uses ray cast)
  vtkPicker is used to select instances of vtkProp3D by shooting a ray 
  into a graphics window and intersecting with the actor's bounding box. 
  The ray is defined from couple of points in world coordinates, 
  and a point located from the camera's position.
 
  vtkPicker may return more than one vtkProp3D, since more than one bounding 
  box may be intersected. vtkPicker returns the list of props that were hit, 
  the pick coordinates in world and untransformed mapper space, and the 
  prop (vtkProp3D) and mapper that are "closest" to the camera. The closest 
  prop is the one whose center point (i.e., center of bounding box) 
  projected on the ray is closest to the camera.

  Inspired to vtkCellPicker.

  @todo
  - rewrite all documentation!!!
*/
class VTK_vtkMAF_EXPORT vtkRayCast3DPicker : public vtkAbstractPropPicker
{
public:
  static vtkRayCast3DPicker *New();
  vtkTypeRevisionMacro(vtkRayCast3DPicker,vtkAbstractPropPicker);
  void PrintSelf(ostream& os, vtkIndent indent);

  /**
    Specify tolerance for performing pick operation. Tolerance is specified
    as fraction of rendering window size. (Rendering window size is measured
    across diagonal.) */
  vtkSetMacro(Tolerance,double);
  vtkGetMacro(Tolerance,double);

  /** return the picking tolerance in world coordinates, at last pick! */
  vtkGetMacro(PickTolerance,double);
  

  /**
    Return position in mapper (i.e., non-transformed) coordinates of 
    pick point.*/
  vtkGetVectorMacro(MapperPosition,double,3);

  /** Return mapper that was picked (if any).*/
  vtkGetObjectMacro(Mapper,vtkAbstractMapper3D);

  /**
    Get a pointer to the dataset that was picked (if any). If nothing 
    was picked then NULL is returned. */
  vtkGetObjectMacro(DataSet,vtkDataSet);

  /**
    Return a collection of all the prop 3D's that were intersected
    by the pick ray. This collection is not sorted. */
  vtkProp3DCollection *GetProp3Ds() {return this->Prop3Ds;};

  /**
    Return a collection of all the actors that were intersected.
    This collection is not sorted. (This is a convenience method
    to maintain backward compatibility.) */
  vtkActorCollection *GetActors();

  /**
    Return a list of the points the the actors returned by GetActors
    were intersected at. The order of this list will match the order of
    GetActors. */
  vtkPoints *GetPickedPositions() {return this->PickedPositions;};
  
  /**
    Perform pick operation with selection point provided. Currenltly does
    nothin. Return non-zero if something was successfully picked. */
  virtual int Pick(double selectionX, double selectionY, double selectionZ, 
                   vtkRenderer *renderer);
  /** 
    Perform pick operation with selection point provided. Currenltly does
    nothin. Return non-zero if something was successfully picked. */
  int Pick(double selectionPt[3], vtkRenderer *ren)
    {return this->Pick(selectionPt[0], selectionPt[1], selectionPt[2], ren);};

  /** Pick the scene with a 3D ray */
  int Pick(double *p1, double *p2, vtkRenderer *renderer);

//**********************************************************************************************************
// FROM vtkCellPicker
//**********************************************************************************************************
  vtkGenericCell *Cell;
  int SubId;
  vtkIdType CellId; 
  double PCoords[3]; // picked cell parametric coordinates
  
  vtkGetVectorMacro(PCoords, double,3);
  
  double x[3];
//**********************************************************************************************************
 
  vtkTransform *GetTransform(){ return this->Transform; };
protected:
  vtkRayCast3DPicker();
  ~vtkRayCast3DPicker();

  void MarkPicked(vtkAssemblyPath *path, vtkProp3D *p, vtkAbstractMapper3D *m, 
                  double tMin, double mapperPos[3]);
  virtual double IntersectWithLine(double p1[3], double p2[3], double tol, 
                                  vtkAssemblyPath *path, vtkProp3D *p, 
                                  vtkAbstractMapper3D *m);
  virtual void Initialize();

  double Tolerance; ///< tolerance for computation (% of window)
  double MapperPosition[3]; ///< selection point in untransformed coordinates
  double PickTolerance; ///< Tolerance at the pick point in world coordinates

  vtkAbstractMapper3D *Mapper; ///< selected mapper (if the prop has a mapper)
  vtkDataSet *DataSet; ///< selected dataset (if there is one)

  double GlobalTMin; ///< parametric coordinate along pick ray where hit occured
  vtkTransform *Transform; ///< use to perform ray transformation
  vtkActorCollection *Actors; ///< candidate actors (based on bounding box)
  vtkProp3DCollection *Prop3Ds; ///< candidate actors (based on bounding box)
  vtkPoints *PickedPositions; ///< candidate positions
  
private:
  vtkRayCast3DPicker(const vtkRayCast3DPicker&);  // Not implemented.
  void operator=(const vtkRayCast3DPicker&);  // Not implemented.
};


#endif


