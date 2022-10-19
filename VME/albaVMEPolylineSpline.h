/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMEPolylineSpline
 Authors: Daniele Giunchi & Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __albaVMEPolylineSpline_h
#define __albaVMEPolylineSpline_h
//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVME.h"
#include "albaVMEPolyline.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;
class albaVME;
class vtkPoints;
class mmaMaterial;
class albaVMELandmarkCloud;

/** albaVMEPolylineSpline - a procedural VME computing the spline by a given polyline as link
  albaVMEPolylineSpline is a node implementing a spline starting from a polyline using vtkCardinalSpline
  algorithm.
  @sa albaVMEPolyline
  @todo
  - 
  */
class ALBA_EXPORT albaVMEPolylineSpline : public albaVME
{
public:

  enum ID_VME_POLYLINE_SPLINE
  {
    AXIS_NONE = 0,
    AXIS_X,
    AXIS_Y,
    AXIS_Z,
  };

  albaTypeMacro(albaVMEPolylineSpline,albaVME);

  /** print a dump of this object */
  virtual void Print(std::ostream& os, const int tabs=0);

  /** Copy the contents of another VME-Spline into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another VME-Spline. */
  virtual bool Equals(albaVME *vme);

  /** return the right type of output */  
  albaVMEOutputPolyline *GetPolylineOutput();

  /**
    Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
    set or get the Pose for a specified time. When setting, if the time does not exist
    the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
    interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const albaMatrix &mat);

  /**
    Return the list of timestamps for this VME. Timestamps list is 
    obtained merging timestamps for matrices and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** return always false since (currently) the spline is not an animated VME (position 
      is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return true if the data associated with the VME is present and updated at the current time.*/
  virtual bool IsDataAvailable();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipePolyline");};

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();

	/** Set links for the Polyline*/
  void SetPolylineLink(albaVME *n);

  /** Get links for the Polyline*/
	albaVME *GetPolylineLink();

  /** Order the points and cell in polydata*/
  void SplinePolyline(vtkPolyData *polyline);

  /** Order the points and cell in polydata*/
  void OrderPolyline(vtkPolyData *polyline);

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

	/** Set color of Spline. */
	void SetColor(double r, double g, double b);

  /** reorganize the points in order to follow an axis */
  void OrderPolylineByAxis(vtkPolyData* polyline, int axis);

	enum SPLINE_WIDGET_ID
  {
    ID_LINK_POLYLINE = Superclass::ID_LAST,
		ID_NUMBER_NODES,
    ID_LAST
  };

	static bool PolylineAccept(albaVME*node) {return(node != NULL && (node->IsA("albaVMEPolyline") || node->IsA("albaVMELandmarkCloud")));};

	/** Precess events coming from other objects */ 
  virtual void OnEvent(albaEventBase *alba_event);

  /** Get Coefficient used for calculated number of points*/ 
  int GetSplineCoefficient(){return m_SplineCoefficient;}
  void SetSplineCoefficient(int coeff){m_SplineCoefficient = coeff;}

  void SetOrderByAxisFlag(int axis){m_OrderByAxisMode = axis;};
 
  // obsolete function
  //void SetMinimumSpacingOptimization(double spacing){m_MinimumSpacing = spacing;}

protected:
  albaVMEPolylineSpline();
  virtual ~albaVMEPolylineSpline();

  //void OptimizeMinimumSpacingSpline();

  virtual int InternalStore(albaStorageElement *parent);
  virtual int InternalRestore(albaStorageElement *node);

  /** called to prepare the update of the output */
  virtual void InternalPreUpdate();

  /** update the output data structure */
  virtual void InternalUpdate();

	/** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

  albaTransform*     m_Transform; ///< pose matrix for the spline 

  vtkPolyData				*m_Polyline;
	vtkPoints					*m_PointsSplined;

	int								m_SplineCoefficient;
  //double            m_MinimumSpacing;

	albaString					m_PolylineLinkName;
  int               m_OrderByAxisMode;


private:
  albaVMEPolylineSpline(const albaVMEPolylineSpline&); // Not implemented
  void operator=(const albaVMEPolylineSpline&); // Not implemented
};

#endif
