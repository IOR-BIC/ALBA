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
	double min, max, mean, sdtdev, nPoints;
};

class vtkImageData;
class albaVMELandmarkCloud;
class albaHTMLTemplateParser;
struct Calibration;

enum TAIL_POSITION {
	TAIL_FRONT,
	TAIL_REAR,
	TAIL_LEFT,
	TAIL_RIGHT,
};

//----------------------------------------------------------------------------
// Class Name: appOpEmpty
//----------------------------------------------------------------------------
class albaOpESPCalibration : public albaOp
{
public:
	/** Constructor. */
	albaOpESPCalibration(wxString label = "Op Empty", bool enable = true);

	/** Destructor. */
	~albaOpESPCalibration();

	/** RTTI macro. */
	albaTypeMacro(albaOpESPCalibration, albaOp);

	/** Return a copy of the operation */
	/*virtual*/ albaOp* Copy();

	/** Return true for the acceptable vme type. */
	/*virtual*/ bool Accept(albaVME *node);

	/** Builds operation's interface. */
	/*virtual*/ void OpRun();

	/** Execute the operation. */
	/*virtual*/ void OpDo();

	void Calibrate();

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
		ID_GENARATE_REPORT = MINID,
		MINID,
	};

	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	/*virtual*/ void OpStop(int result);	

	/** Create the Operation GUI */
	virtual void CreateGui();


	/** Generate Report*/
	bool CreateReport();


	Range CaluculateCutOffRange(vtkImageData *imgData, double cutOff);

	template<typename DataType>
	Range CalculateCutOfRange(int scalarSizes, DataType * scalars, double cutOff);

	albaVME *CreateParallelepiped(double center[3], char * name, double xLen, double yLen, double height);
	albaVME *CreateCylinder(double centerX, double centerY, double centerZ, char * name, double height, double radius);

	bool GetCenter(vtkImageData *rg, Range xRange, Range yRange, int zEight, double *center/*, albaVMELandmarkCloud *lmc*/);

	double GetRadius(vtkImageData *rg, Range xRange, Range yRange, int zEight, double center[3], bool getInner = true);

	bool CalculateSpinalDensityInfo(vtkImageData *rg);

	bool CalculateBoneDensityInfo(vtkImageData *rg);

	bool CalculateTailDensityInfo(vtkImageData *rg);


	double GetTailCenter(vtkImageData *rg, int side, double x, double y, double z);

	bool FitPoints();



	Cylinder m_InnerCylinders[3];
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
private:
	void AddCalibrationToDB();

};
#endif
