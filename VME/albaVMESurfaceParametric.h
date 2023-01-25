/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaVMESurfaceParametric
 Authors: Roberto Mucci , Stefano Perticoni, Nicola Vanella
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaVMESurfaceParametric_h
#define __albaVMESurfaceParametric_h

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaVME.h"
#include "albaEvent.h"

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class albaVME;
class mmaMaterial;
class albaVMEOutputSurface;
class vtkPolyData;

/** albaVMESurfaceParametric - this class represents a parametric surface that is a specific geometry.
currently generated geometries are: cylinder, sphere, cube, cone and plane.
*/
class ALBA_EXPORT albaVMESurfaceParametric : public albaVME
{
public:

  albaTypeMacro(albaVMESurfaceParametric, albaVME);
    
  enum PARAMETRIC_SURFACE_TYPE_ID
  {
    PARAMETRIC_SPHERE = 0,
    PARAMETRIC_CONE,
    PARAMETRIC_CYLINDER,
    PARAMETRIC_CUBE,
    PARAMETRIC_PLANE,
    PARAMETRIC_ELLIPSOID,
		PARAMETRIC_TRUNCATED_CONE,
		PARAMETRIC_ELLIPTIC_CYLINDER,
  };

  /** Copy the contents of another albaVMESurfaceParametric into this one. */
  virtual int DeepCopy(albaVME *a);

  /** Compare with another albaVMESurfaceParametric. */
  virtual bool Equals(albaVME *vme);

  /** return the right type of output */  
  albaVMEOutputSurface *GetSurfaceOutput();

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<albaTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();

  /** Return the suggested pipe-typename for the visualization of this vme */
  virtual albaString GetVisualPipe() {return albaString("albaPipeSurface");};

  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();
	
  /** Precess events coming from other objects */ 
  virtual void OnEvent(albaEventBase *alba_event);

  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const albaMatrix &mat);

	/** Set the geometry type to be generated: use PARAMETRIC_SURFACE_TYPE_ID as arg*/
	void SetGeometryType(int parametricSurfaceTypeID);

	/** Return the type of the parametric object.*/
	int GetGeometryType() { return m_GeometryType; };

	/** Set the radius for the parametric sphere.*/
	void SetSphereRadius(double radius);

	/** Sets CylinderRadius */
	void SetCylinderRadius(double cylinderRadius);

	/** Sets CylinderHeight */
	void SetCylinderHeight(double cylinderHeight);

	/** Return the radius of the parametric sphere.*/
	double GetSphereRadius() { return m_SphereRadius; };

	/** Return the radius of the parametric cylinder.*/
	double GetCylinderRadius() { return m_CylinderRadius; };

	/** Return the height of the parametric cylinder.*/
	double GetCylinderHeight() { return m_CylinderHeight; };

	/** Return the cylinder orientation axis.*/
	int GetCylinderAxis() { return m_CylinderOrientationAxis; };

	/** Returns EllipsoidXLenght */
	double GetEllipsoidXLenght() const { return m_EllipsoidXLenght; }

	/** Sets EllipsoidXLenght */
	void SetEllipsoidXLenght(double ellipsoidXLenght) { m_EllipsoidXLenght = ellipsoidXLenght; }
	
	/** Returns EllipsoidYLenght */
	double GetEllipsoidYLenght() const { return m_EllipsoidYLenght; }

	/** Sets EllipsoidYLenght */
	void SetEllipsoidYLenght(double ellipsoidYLenght) { m_EllipsoidYLenght = ellipsoidYLenght; }
	
	/** Returns EllipsoidZLenght */
	double GetEllipsoidZLenght() const { return m_EllipsoidZLenght; }

	/** Sets EllipsoidZLenght */
	void SetEllipsoidZLenght(double ellipsoidZLenght) { m_EllipsoidZLenght = ellipsoidZLenght; }

	/** Gets CubeXLength */
	double GetCubeXLength() const { return m_CubeXLength; }
	
	/** Sets CubeXLength */
	void SetCubeXLength(double val) { m_CubeXLength = val; }
	
	/** Gets CubeYLength */
	double GetCubeYLength() const { return m_CubeYLength; }

	/** Sets CubeYLength */
	void SetCubeYLength(double val) { m_CubeYLength = val; }
	
	/** Gets CubeZLength */
	double GetCubeZLength() const { return m_CubeZLength; }

	/** Sets CubeYLength */
	void SetCubeZLength(double val) { m_CubeZLength = val; }

	double GetEllipticCylinderR1() const { return m_EllipticCylinderR1; }
	void SetEllipticCylinderR1(double val);
	double GetEllipticCylinderHeight() const { return m_EllipticCylinderHeight; }
	void SetEllipticCylinderHeight(double val);
	double GetEllipticCylinderR2() const { return m_EllipticCylinderR2; }
	void SetEllipticCylinderR2(double val);
	int GetEllipticCylinderCapping() const { return m_EllipticCylinderCapping; }
	void SetEllipticCylinderCapping(int val);
	double GetCylinderRes() const { return m_CylinderRes;}
	void SetCylinderRes(double val);
	double GetEllipticCylinderRes() const { return m_EllipticCylinderRes; }
	void SetEllipticCylinderRes(double val);
	double GetSpherePhiRes() const { return m_SpherePhiRes; }
	void SetSpherePhiRes(double val) { m_SpherePhiRes = val; }
	double GetSphereTheRes() const { return m_SphereTheRes; }
	void SetSphereTheRes(double val) { m_SphereTheRes = val; }


	/** Returns PlaneOrigin */
	double *GetPlaneOrigin() { return m_PlaneOrigin; }

	/** Sets PlaneOrigin */
	void SetPlaneOrigin(double *origin) { m_PlaneOrigin[0] = origin[0]; m_PlaneOrigin[0] = origin[0]; m_PlaneOrigin[0] = origin[0];}


	/** Returns PlanePoint1 */
	double *GetPlanePoint1() { return m_PlanePoint1; }

	/** Sets PlanePoint1 */
	void SetPlanePoint1(double *point1) { m_PlanePoint1[0] = point1[0]; m_PlanePoint1[0] = point1[0]; m_PlanePoint1[0] = point1[0]; }

	/** Returns PlanePoint2 */
	double *GetPlanePoint2() { return m_PlanePoint2; }

	/** Sets PlanePoint2 */
	void SetPlanePoint2(double *Point2) { m_PlanePoint2[0] = Point2[0]; m_PlanePoint2[0] = Point2[0]; m_PlanePoint2[0] = Point2[0]; }

protected:

  albaVMESurfaceParametric();
  virtual ~albaVMESurfaceParametric();

  enum PARAMETRIC_SURFACE_WIDGET_ID
  {
    CHANGE_PARAMETER = Superclass::ID_LAST,
    CHANGE_VALUE_SPHERE,
    CHANGE_VALUE_CONE,
    CHANGE_VALUE_CYLINDER,
    CHANGE_VALUE_CUBE,
    CHANGE_VALUE_PLANE,
    CHANGE_VALUE_ELLIPSOID,
		CHANGE_VALUE_TRUNCATED_CONE,
		CHANGE_VALUE_ELLIPTIC_CYLINDER,
    ID_GEOMETRY_TYPE,
    ID_LAST
  };

  enum ID_ORIENTATION_AXIS
  {
    ID_X_AXIS = 0,
    ID_Y_AXIS,
    ID_Z_AXIS,
  };
  

  /** this function uses PARAMETRIC_SURFACE_TYPE_ID as argument*/
  void EnableParametricSurfaceGui(int surfaceTypeID);

	virtual int InternalStore(albaStorageElement *parent);
	virtual int InternalRestore(albaStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();

	/** update the output data structure */
	virtual void InternalUpdate();

	void CreateTruncatedCone();

	void CreateEllipsoid();

	void CreatePlane();

	void CreateCube();

	void CreateCylinder();

	void CreateCone();

	void CreateSphere();

	void CreateEllipticCylinder();

  /** Internally used to create a new instance of the GUI.*/
  virtual albaGUI *CreateGui();

	void AddLineToGUI(albaGUI *gui, int nLines);
	void CreateGuiPlane();
  void CreateGuiCube();
  void CreateGuiCylinder();
  void CreateGuiCone();
  void CreateGuiSphere();
  void CreateGuiEllipsoid();
	void CreateGuiTruncatedCone();
	void CreateGuiEllipticCylinder();
  
  albaGUI *m_GuiSphere;
  albaGUI *m_GuiCone;
  albaGUI *m_GuiCylinder;
  albaGUI *m_GuiCube;
  albaGUI *m_GuiPlane;
  albaGUI *m_GuiEllipsoid;
	albaGUI *m_GuiTruncatedCone;
	albaGUI  *m_GuiEllipticCylinder;

	albaTransform *m_Transform; 
	vtkPolyData  *m_PolyData;
  
  int m_GeometryType;

	double m_SphereRadius;
  double m_SpherePhiRes;
  double m_SphereTheRes;

  double m_ConeHeight;
  double m_ConeRadius;
  int m_ConeCapping;
  double m_ConeRes;
  int m_ConeOrientationAxis;

  double m_CylinderHeight;
  double m_CylinderRadius;
  double m_CylinderRes;
  int m_CylinderOrientationAxis;

  double m_CubeXLength;
  double m_CubeYLength;
  double m_CubeZLength;

  double m_PlaneXRes;
  double m_PlaneYRes;
  double m_PlaneOrigin[3];
  double m_PlanePoint1[3];
  double m_PlanePoint2[3];

  double m_EllipsoidXLenght;
  double m_EllipsoidYLenght;
  double m_EllipsoidZLenght;
  double m_EllipsoidPhiRes;
  double m_EllipsoidTheRes;
  int m_EllipsoidOrientationAxis;

	double m_TruncatedConeHeight;
	double m_TruncatedConeUpperDiameter;
	double m_TruncatedConeLowerDiameter;
	double m_TruncatedConeRes;
	int m_TruncatedConeCapping;
	int m_TruncatedConeOrientationAxis;

	double m_EllipticCylinderHeight;
	double m_EllipticCylinderR1;
	double m_EllipticCylinderR2;
	double m_EllipticCylinderRes;
	int m_EllipticCylinderCapping;
	int m_EllipticCylinderOrientationAxis;

private:
  albaVMESurfaceParametric(const albaVMESurfaceParametric&); // Not implemented
  void operator=(const albaVMESurfaceParametric&); // Not implemented
};
#endif
