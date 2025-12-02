/*=========================================================================
Program:   DentApp
Module:    appOpEmpty.h
Language:  C++
Date:      $Date: 2018-01-01 12:00:00 $
Version:   $Revision: 1.0.0.0 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) LTM-IOR 2018 (https://github.com/IOR-LTM)

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notice for more information.
=========================================================================*/

#ifndef __albaOpESPCalibration_H__
#define __albaOpESPCalibration_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDecl.h"
#include "albaOp.h"
#include "albaVMEVolumeGray.h"
#include "vtkDataSet.h"

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/dom/DOM.hpp>
//----------------------------------------------------------------------------
// Forward references :
//----------------------------------------------------------------------------
struct Range
{
	int l;
	int r;
};

struct Cylinder
{
	double centerX, centerY;
	Range xRange,yRange,zRange;
	double radius;
};

struct BoneCylinder
{
	double centerX, centerY;
	Range xRange, yRange, zRange;
	double InnerRadius,OuterRadius;
};

struct Tail
{
	Range xRange, yRange, zRange;
};

struct DensityInfo
{
	double min, max, mean, errorOnEstimation, sdtdev, nPoints;
};

class vtkImageData;
class albaVMELandmarkCloud;
class albaHTMLTemplateParser;
struct Calibration;

enum TailPosition {
	TAIL_ON_FRONT,
	TAIL_ON_REAR,
	TAIL_ON_LEFT,
	TAIL_ON_RIGHT,
};

//----------------------------------------------------------------------------
// Class Name: appOpEmpty
//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpESPCalibration : public albaOp
{
public:
	/** Constructor. */
	albaOpESPCalibration(wxString label = "ESP Calibration", bool enable = true);

	/** Destructor. */
	~albaOpESPCalibration();

	/** RTTI macro. */
	albaTypeMacro(albaOpESPCalibration, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	int Calibrate();

	/** Receive events coming from the user interface.*/
	void OnEvent(albaEventBase *alba_event);
  
	void EnableOp(bool enable) { m_Enabled = enable; };

	/** Return an xpm-icon that can be used to represent this operation */
	virtual char ** GetIcon();

protected:

	//----------------------------------------------------------------------------
	// Widgets ID's
	//----------------------------------------------------------------------------
	enum CALIBRATION_ID
	{
		ID_SAVE_CALIBRATION = MINID,
		MINID,
	};

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();
	
	/** Generate Report*/
	bool SaveCalibration();
	
	bool CheckNodeElement(XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const char *elementName);
	Range CaluculateCutOffRange(vtkImageData *imgData, double cutOff);

	template<typename DataType>
	Range CalculateCutOfRange(int scalarSizes, DataType * scalars, double cutOff);

	albaVME *CreateVMEParallelepiped(double center[3], char * name, double xLen, double yLen, double height);
	albaVME *CreateVMECylinder(double centerX, double centerY, double centerZ, char * name, double height, double radius);

	Cylinder CreateCylinder(double * origin, double * spacing, double centerX, double centerY, double centerZ, double radius, double cylHeight);

	BoneCylinder CreateBoneCylinder(TailPosition tailPosition, double * origin, double * spacing, double center[3], double wallCenter[3], double innerRadius, double OuterRadius, double cylHeight);
	
	Tail CreateTail(double * origin, double * spacing, double centerX, double centerY, double centerZ, double xSize, double ySize, double zSize);

	bool CalculateSpinalDensityInfo(vtkImageData *rg);

	bool CalculateBoneDensityInfo(vtkImageData *rg);

	bool CalculateTailDensityInfo(vtkImageData *rg);
	
	double GetTailCenter(vtkImageData *rg, TailPosition tailPos, double x, double y, double z);

	void GetBoneCenter(vtkImageData *rg, TailPosition tailPos, double x, double y, double z, double newCenter[3], double wallPos[3]);

	void IdToXYZpos(vtkIdType startCenter, int * dims, int &xPos, int &yPos, int &zPos);
	
	bool FitPoints();
	
	std::vector <albaVME *> toHidelist;
	Cylinder m_SpinalCylinders[3];
	BoneCylinder m_BoneCylinders[2];
	Tail m_Tail;

	DensityInfo m_AreaInfo[5];
	albaVMEVolumeGray *m_Volume;

	double m_BoneMean[2];

	double m_Slope;
	double m_Intercept;
	double m_RSquare;

	bool m_Enabled;
	
	albaHTMLTemplateParser *m_Parser;

	albaView *m_View;
};
#endif