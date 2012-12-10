/*=========================================================================
  Program:   MAF2Medical
  Module:    $RCSfile: medVMEStent.h,v $
  Language:  C++
  Date:      $Date: 2012-10-23 10:15:31 $
  Version:   $Revision: 1.1.2.7 $
  Authors:   Hui Wei
==========================================================================
Copyright (c) 2001/2005 
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#ifndef __medVMEStent_h 
#define __medVMEStent_h

/**----------------------------------------------------------------------------*/
// Include:
/**----------------------------------------------------------------------------*/
#include "medVMEDefines.h"  //very important for MED_VME_EXPORT
#include "mafVME.h"
#include "mafEvent.h"

#include "vtkMEDStentModelSource.h"

#include <vector>
using std::vector;
/**----------------------------------------------------------------------------*/
// forward declarations :
/**----------------------------------------------------------------------------*/
class mafNode;
class mmaMaterial;
class mafVMEOutputSurface;
class mafVMEOutputPolyline;
class vtkPolyData;
class vtkTransform;
class vtkAppendPolyData;
class vtkTubeFilter;
/**----------------------------------------------------------------------------*/
//--------typedef----------
/**----------------------------------------------------------------------------*/
typedef itk::DefaultDynamicMeshTraits<double, 3, 3, double, double> MeshTraits;
typedef itk::SimplexMesh<double,3,MeshTraits>           SimplexMeshType;
typedef SimplexMeshType::NeighborListType               NeighborsListType;
typedef itk::SimplexMeshGeometry::IndexArray            IndexArray;
typedef SimplexMeshType::PointType                      PointType;
typedef vtkMEDStentModelSource::Strut                         Strut;
typedef vtkMEDStentModelSource::StentConfigurationType        enumStCfgType;
typedef vtkMEDStentModelSource::LinkConnectionType            enumLinkConType;
typedef vtkMEDStentModelSource::LinkOrientationType           enumLinkOrtType;

typedef vector<Strut>::const_iterator    StrutIterator;

/**
this class create a stent based on some basic parameters. 
It also can be depolyed to a curve line as its centerline.
After it was given an constrain surface, it can expand inside this
constrain surface.
*/
class MED_VME_EXPORT medVMEStent : public mafVME
{

protected:
	 /** for gui control */
	  enum STENT_WIDGET_ID
	  {
		
		CHANGE_VALUE,
		STENT_DIAMETER,
		CROWN_LENGTH,
		STRUT_ANGLE,
		STRUT_THICKNESS,
		ID_STENT_CONFIGURATION,
		ID_LINK_CONNECTION,
		LINK_LENGTH,
		NUMBER_OF_LINKS,//(along circumference)
		LINK_ALIGNMENT,
		LINK_ORIENTATION,
		ID_CENTERLINE,
		ID_CONSTRAIN_SURFACE,
		ID_DEFORMATION,
		ID_LAST
	  };
    /** store attributes and matrix*/
	virtual int InternalStore(mafStorageElement *parent);
    /** restore attributes and matrix*/
	virtual int InternalRestore(mafStorageElement *node);

	/** called to prepare the update of the output */
	virtual void InternalPreUpdate();
	/** update the output data structure */
	virtual void InternalUpdate();
	/** do deformation under the constrain of constrain surface */
	void DoDeformation(int type);
	/** do deformation under the constrain of constrain surface */
	void DoDeformation2(int type);
	
	/** Internally used to create a new instance of the GUI.*/
	virtual mafGUI *CreateGui();

	mafTransform *m_Transform; 
	double m_StentRadius;
	vtkPolyData  *m_Centerline; 
	//vtkPolyData *m_CenterlineModify;//sheathVTK

	/**----------- stent parameters-----------*/
	/** basic stent  */
	double m_Stent_Diameter;
	double m_Crown_Length;
	double m_Strut_Angle;
	double m_Strut_Thickness;
	int m_Id_Stent_Configuration;
	/**  stent link  */
	int m_Id_Link_Connection;
	double m_Link_Length;
	int m_Link_Alignment;
	int m_Link_orientation;  
	/**----------- center line and constrain surface-----------*/
	mafString m_CenterLineName;
	mafString m_ConstrainSurfaceName;

private:
	/**  deformation iterator  */
	int m_NumberOfCycle;
	vtkPolyData *m_StentPolyLine;  
	vector<vector<double>> m_StentCenterLine;
	vector<vector<double>>::const_iterator m_CenterLineStart;
	vector<vector<double>>::const_iterator m_CenterLineEnd;
	vtkPolyData* CreateAConstrainSurface();
	void moveCatheter(int numberOfCycle);
	void expandStent(int numberOfCycle);

	/** three output in append polydata*/
	vtkPolyData  *m_PolyData;
	vtkPolyData  *m_CatheterPolyData;
	vtkPolyData  *m_ConstrainSurfaceTmp;
	vtkAppendPolyData *m_AppendPolyData;
	/** the output of this vme */
	vtkPolyData *m_AppendPolys;

	vtkPolyData *m_SheathVTK;
	/** used to create stent */
	vtkMEDStentModelSource m_StentSource;
	SimplexMeshType::Pointer m_SimplexMesh;
	/** to check if deformation in progress */
	int m_DeformFlag ;

public:

  mafTypeMacro(medVMEStent, mafVME);
  /* constructor */
  medVMEStent();
  /* destructor */
  virtual ~medVMEStent();
  
  //---------------------------Setter-/-Getter------------------------------------  
  /** Copy the contents of another medVMEStent into this one. */
  virtual int DeepCopy(mafNode *a);
  /** Compare with another mafVMESurfaceParametric. */
  virtual bool Equals(mafVME *vme);
  
  /** Return the suggested pipe-typename for the visualization of this vme */
  //virtual mafString GetVisualPipe() {return mafString("mafPipeSurface");};
  //mafVMEOutputSurface *GetSurfaceOutput();//return the right type of output 

  /** used to visualize this vme */
  virtual mafString GetVisualPipe() {return mafString("mafPipePolyline");};
  mafVMEOutputPolyline *GetPolylineOutput(); //return the right type of output 

 // vtkPolyData *GetPolyData(){InternalUpdate();return m_PolyData;};
  /** the append polydata */
  vtkPolyData *GetPolyData();//{ InternalUpdate();return  m_PolyData;};// m_AppendPolyData->GetOutput(); };
  
  /** Get the vtkPolyData generated from the model */
  //vtkPolyData  *GetVtkPolyData(){InternalUpdate();return m_PolyData;};

  /**
  Return the list of timestamps for this VME. Timestamps list is 
  obtained merging timestamps for matrixes and VME items*/
  virtual void GetLocalTimeStamps(std::vector<mafTimeStamp> &kframes);

  /** return always false since (currently) the slicer is not an animated VME (position 
  is the same for all timestamps). */
  virtual bool IsAnimated();



  /** Return pointer to material attribute. */
  mmaMaterial *GetMaterial();

  /** return an xpm-icon that can be used to represent this node */
  static char ** GetIcon();
	
  /** Precess events coming from other objects */ 
  virtual void OnEvent(mafEventBase *maf_event);


  /**
  Set the Pose matrix of the VME. This function modifies the MatrixVector. You can
  set or get the Pose for a specified time. When setting, if the time does not exist
  the MatrixVector creates a new KeyMatrix on the fly. When getting, the matrix vector
  interpolates on the fly according to the matrix interpolator.*/
  virtual void SetMatrix(const mafMatrix &mat);
  
  /**
  select center line or constrain surface 
  */
    void SetStentLink(const char *link_name, mafNode *ns);
  //------set properties-----------
  void SetCenterLine(vtkPolyData *line);
  void SetConstrainSurface(vtkPolyData *surface);
  void SetStentDiameter(double diameter){m_Stent_Diameter = diameter;}
  void SetStentCrownLength(double crownL){m_Crown_Length = crownL;}
  void SetStentConfiguration(int stentCfig){m_Id_Stent_Configuration = stentCfig;}
  void SetLinkLength(double linkLength){m_Link_Length = linkLength;}
  void SetLinkAlignment(int linkAlgn){m_Link_Alignment = linkAlgn;}
  void SetLinkOrientation(int linkOrit){m_Link_orientation = linkOrit;}
  //--------convert tube strips into triangle polydata 
  vtkPolyData *TubeToPolydata( vtkTubeFilter * sheath );
};
#endif
