/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafVMEPolylineSpline.h,v $
  Language:  C++
  Date:      $Date: 2009-04-27 10:38:37 $
  Version:   $Revision: 1.10.2.3 $
  Authors:   Daniele Giunchi & Matteo Giacomoni
==========================================================================
  Copyright (c) 2001/2005 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#ifndef __mafVMEPolylineSpline_h
#define __mafVMEPolylineSpline_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafVME.h"
#include "mafVMEPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class mafNode;
class vtkPoints;
class mmaMaterial;

/** mafVMEPolylineSpline - a procedural VME computing the spline by a given polyline as link
  mafVMEPolylineSpline is a node implementing a spline starting from a polyline using vtkCardinalSpline
  algorithm.
  @sa mafVMEPolyline
  @todo
  - 
  */
class MAF_EXPORT mafVMEPolylineSpline : public mafVME
{
public:

  enum ID_VME_POLYLINE_SPLINE
  {
    AXIS_NONE = 0,
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
  };

  mafTypeMacro(mafVMEPolylineSpline,mafVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Spline into this one. */
  virtual int DeepCopy(mafNode *a);

  /** Compare with another VME-Spline. */
  virtual bool Equals(mafVME *vme);

  /** return the right type of output */  
  mafVMEOutputPolyline *GetPolylineOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the spline is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipePolyline");};

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

	/** Set links for the Polyline*/
  void SetPolylineLink(mafNode *n);

  /** Get links for the Polyline*/
	mafVME *GetPolylineLink();

  /** Order the points and cell in polydata*/
  void SplinePolyline(vtkPolyData *polyline);

  /** Order the points and cell in polydata*/
  void OrderPolyline(vtkPolyData *polyline);

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** reorganize the points in order to follow an axis */
  void OrderPolylineByAxis(vtkPolyData* polyline, int axis);

	enum SPLINE_WIDGET_ID
  {
    ID_LINK_POLYLINE = Superclass::ID_LAST,
		ID_NUMBER_NODES,
    ID_LAST
  };

	static bool PolylineAccept(mafNode *node) {return(node != NULL && (node->IsA("mafVMEPolyline")));};

	/** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);

  /** Get Coefficient used for calculated number of points*/ 
  int GetSplineCoefficient(){return m_SplineCoefficient;}
  void SetSplineCoefficient(int coeff){m_SplineCoefficient = coeff;}

  void SetOrderByAxisFlag(int axis){m_OrderByAxisMode = axis;};
 
  // obsolete function
  //void SetMinimumSpacingOptimization(double spacing){m_MinimumSpacing = spacing;}

protected:
  mafVMEPolylineSpline();
  virtual ~mafVMEPolylineSpline();

  //void OptimizeMinimumSpacingSpline();

  virtual int InternalStore(mafStorageElement *parent);
  virtual int InternalRestore(mafStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

	/** Internally used to create a new instance of the GUI.*/
  virtual mafGUI *CreateGui();

  mafTransform*     m_Transform; ///< pose matrix for the spline 

  vtkPolyData				*m_Polyline;
	vtkPoints					*m_PointsSplined;

	int								m_SplineCoefficient;
  //double            m_MinimumSpacing;

	mafString					m_PolylineLinkName;
  int               m_OrderByAxisMode;


private:
  mafVMEPolylineSpline(const mafVMEPolylineSpline&); // Not implemented
  void operator=(const mafVMEPolylineSpline&); // Not implemented
};

#endif
