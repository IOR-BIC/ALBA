/*=========================================================================
Program:   DentApp
Module:    appOpEmpty.cpp
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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the APP must include "appDefines.h" as first.
// This force to include Window, wxWidgets and VTK exactly in this order.
//----------------------------------------------------------------------------

#include "albaOpESPCalibration.h"

#include "albaDecl.h"

#include "albaGUI.h"
#include "albaHTMLTemplateParser.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEImage.h"
#include "albaVMEIterator.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEOutput.h"
#include "albaVMESurfaceParametric.h"
#include "albaVect3d.h"
#include "mmaMaterial.h"

#include "vtkAlbaProjectVolume.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "albaTagArray.h"
#include "wx/busyinfo.h"
#include "vtkStructuredPoints.h"

#define WATER_VALUE 0
#define SPONGIOUS_L1 50
#define SPONGIOUS_L2 100
#define SPONGIOUS_L1 200


#define AIR_VALUE -1000.0
#define TARGET_MAX 1380.0	 //Scale factor 	
#define CUT_OFF_BODY 975.0 //Middle point from wings (700) and Cortical (700)
#define CUT_OFF_WINGS 425.0 //Middle point from outside (150) and wings (700)
#define TAIL_RATIO 14.0/20.0 //Tail on L3 to rearwall ratio
#define INTERVERTEBRAL_SPACE_RATIO 1.0/10.0
#define MARGIN_REDUCTION 0.75

#define FRONT_BACK_CYL_RADIUS_RATIO 7.5/9.0
#define BACK_CYL_R1_R2_RATIO 9.5/13.0
#define BACK_CYL_MARGIN_REDUCTION 0.94

#define TAILSIZE_MIN 6.0
#define TAILSIZE_MAX 13.0
#define BODY_RADIUS_TAIL_RATIO 1.5


#define CIRCLES_THRESHOLD 0
#define EXPECTED_VALUES {0,0,0,0}

#define SIDE_X 0
#define SIDE_Y 1



//----------------------------------------------------------------------------
albaCxxTypeMacro(albaOpESPCalibration);

//----------------------------------------------------------------------------
albaOpESPCalibration::albaOpESPCalibration(wxString label, bool enable) :albaOp(label)
{
	m_OpType = OPTYPE_OP;
	m_Canundo = true;

	m_Enabled = enable;
}

//----------------------------------------------------------------------------
albaOpESPCalibration::~albaOpESPCalibration()
{
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::Accept(albaVME *node)
{
	return node->IsA("albaVMEVolumeGray");
}

//----------------------------------------------------------------------------
albaOp* albaOpESPCalibration::Copy()
{
	albaOpESPCalibration *cp = new albaOpESPCalibration(m_Label, m_Enabled);
	return cp;
}
//----------------------------------------------------------------------------
void albaOpESPCalibration::OpRun()
{
	wxBusyInfo wait(_("Extracting calibration info.\nPlease wait..."));
	wxBusyCursor waitCursor;

	m_Volume = albaVMEVolumeGray::SafeDownCast(m_Input);
	
	Calibrate();

	if (!m_TestMode)
	{
		CreateGui();
	}
}
//----------------------------------------------------------------------------
void albaOpESPCalibration::OpStop(int result)
{
	if (!m_TestMode)
	{
		HideGui();
	}

	if (result == OP_RUN_OK)
	{
		AddCalibrationToDB();
	}

	albaEventMacro(albaEvent(this, result));
}
//----------------------------------------------------------------------------
void albaOpESPCalibration::OpDo()
{
	Calibrate();
	return;
}

//----------------------------------------------------------------------------
void albaOpESPCalibration::Calibrate()
{
	m_Volume->GetOutput()->Update();
	vtkImageData *volumeData = vtkImageData::SafeDownCast(m_Volume->GetOutput()->GetVTKData());
	
	double *scalarRange = volumeData->GetScalarRange();
	double *spacing = volumeData->GetSpacing();
	double *origin = volumeData->GetOrigin();
	int *dims = volumeData->GetDimensions();

	vtkALBAProjectVolume *projectFilterX;
	vtkNEW(projectFilterX);
	projectFilterX->SetInput(volumeData);
	projectFilterX->SetProjectionModalityToMax();
	projectFilterX->SetProjectionSideToX();
	projectFilterX->Update();

	albaVMEImage *imageOutputX;
	albaNEW(imageOutputX);
	imageOutputX->SetData((vtkImageData *)projectFilterX->GetOutput(), m_Volume->GetMTime());
	imageOutputX->SetName("outputX");
	imageOutputX->ReparentTo(m_Volume);
	
	vtkALBAProjectVolume *projectFilterY;
	vtkNEW(projectFilterY);
	projectFilterY->SetInput(volumeData);
	projectFilterY->SetProjectionModalityToMax();
	projectFilterY->SetProjectionSideToY();
	projectFilterY->Update();

	albaVMEImage *imageOutputY;
	albaNEW(imageOutputY);
	imageOutputY->SetData((vtkImageData *)projectFilterY->GetOutput(), m_Volume->GetMTime());
	imageOutputY->SetName("outputY");
	imageOutputY->ReparentTo(m_Volume);

	vtkALBAProjectVolume *projectFilterYX;
	vtkNEW(projectFilterYX);
	projectFilterYX->SetInput(projectFilterY->GetOutput());
	projectFilterYX->SetProjectionModalityToMax();
	projectFilterYX->SetProjectionSideToY();
	projectFilterYX->Update();

	albaVMEImage *imageOutputYX;
	albaNEW(imageOutputYX);
	imageOutputYX->SetData((vtkImageData *)projectFilterYX->GetOutput(), m_Volume->GetMTime());
	imageOutputYX->SetName("outputYX");
	imageOutputYX->ReparentTo(m_Volume);

	vtkALBAProjectVolume *projectFilterXY;
	vtkNEW(projectFilterXY);
	projectFilterXY->SetInput(projectFilterX->GetOutput());
	projectFilterXY->SetProjectionModalityToMax();
	projectFilterXY->SetProjectionSideToY();
	projectFilterXY->Update();

	albaVMEImage *imageOutputXY;
	albaNEW(imageOutputXY);
	imageOutputXY->SetData((vtkImageData *)projectFilterXY->GetOutput(), m_Volume->GetMTime());
	imageOutputXY->SetName("outputXY");
	imageOutputXY->ReparentTo(m_Volume);

		vtkALBAProjectVolume *projectFilterYZ;
	vtkNEW(projectFilterYZ);


	//check distance of the min scalar from Air value and Water value to guess if there is some air in the data
	bool hasAir = (abs(scalarRange[0] - AIR_VALUE) < abs(scalarRange[0] - WATER_VALUE));

	//Rescaling cut off zone basing on current CT max value
	double wingsCutOff = CUT_OFF_WINGS*scalarRange[1] / TARGET_MAX;
	double bodyCutOff = CUT_OFF_BODY*scalarRange[1] / TARGET_MAX;

	int xBodyCenter, yBodyCenter, rearWall, bodyRadius;
	double cilCoord[2];
		
	albaLogMessage("Calculate X wings cut off:");
	Range xWingsRange = CaluculateCutOffRange((vtkImageData *)projectFilterYX->GetOutput(), wingsCutOff);
	albaLogMessage("Calculate X body cut off:");
	Range xBodyRange = CaluculateCutOffRange((vtkImageData *)projectFilterYX->GetOutput(), bodyCutOff);

	albaLogMessage("Calculate Y wings cut off:");
	Range yWingsRange = CaluculateCutOffRange((vtkImageData *)projectFilterXY->GetOutput(), wingsCutOff);
	albaLogMessage("Calculate Y body cut off:");
	Range yBodyRange = CaluculateCutOffRange((vtkImageData *)projectFilterXY->GetOutput(), bodyCutOff);

	
	int xRangeDiff = (xWingsRange.r - xWingsRange.l) - (xBodyRange.r - xBodyRange.l);
	int yRangeDiff = (yWingsRange.r - yWingsRange.l) - (yBodyRange.r - yBodyRange.l);

	bool wingsOnX = xRangeDiff > yRangeDiff;

	if (wingsOnX)
	{
		albaLogMessage("wings on X, xDiff=%d yDiff=%d", xRangeDiff, yRangeDiff);

		xBodyCenter = (xBodyRange.l + xBodyRange.r) / 2;
		int yDiffLeft = (yBodyRange.l - yWingsRange.l);
		int yDiffRight = (yWingsRange.r - yBodyRange.r);

		if (yDiffLeft > yDiffRight)
		{
			albaLogMessage("Tail on left");
			//TODO
		}
		else
		{
			albaLogMessage("Tail on left");

			rearWall = yBodyRange.l + ((yBodyRange.r- yBodyRange.l)*TAIL_RATIO);
			yBodyCenter = (yBodyRange.l + rearWall) / 2;
			bodyRadius = (rearWall - yBodyRange.l) / 2;

			projectFilterY->SetProjectSubRange(true);
			projectFilterY->SetProjectionRange(0, rearWall);
			projectFilterY->Update();
			projectFilterYZ->SetInput(projectFilterY->GetOutput());

		}
	}
	else
	{
		albaLogMessage("wings on Y, xDiff=%d yDiff=%d", xRangeDiff, yRangeDiff);
		
		yBodyCenter = (yBodyRange.l + yBodyRange.r) / 2;
		int xDiffLeft = (xBodyRange.l - xWingsRange.l);
		int xDiffRight = (xWingsRange.r - xBodyRange.r);

		if (xDiffLeft > xDiffRight)
		{
			albaLogMessage("Tail on left");
			//TODO
		}
		else
		{
			albaLogMessage("Tail on left");
			
			rearWall = xBodyRange.l + ((xBodyRange.r - xBodyRange.l)*TAIL_RATIO);
			xBodyCenter = (xBodyRange.l + rearWall) / 2;
			bodyRadius = (rearWall - xBodyRange.l) / 2;
		}
	}

	projectFilterYZ->SetProjectionModalityToMax();
	projectFilterYZ->SetProjectionSideToX();
	projectFilterYZ->Update();

	albaVMEImage *imageOutputYZ;
	albaNEW(imageOutputYZ);
	imageOutputYZ->SetData((vtkImageData *)projectFilterYZ->GetOutput(), m_Volume->GetMTime());
	imageOutputYZ->SetName("outputYZ");
	imageOutputYZ->ReparentTo(m_Volume);

	albaLogMessage("Calculate Z body cut off:");
	Range zBodyRange = CaluculateCutOffRange((vtkImageData *)projectFilterYZ->GetOutput(), bodyCutOff);
	Range zWingsRange = CaluculateCutOffRange((vtkImageData *)projectFilterYZ->GetOutput(), wingsCutOff);


	double radius = bodyRadius * spacing[0] * MARGIN_REDUCTION;
	double centerX,centerY,centerZ[3];
	double BackCylCenterX, BackCylCenterY, rearRadius1, rearRadius2;

	centerX = origin[0] + (xBodyCenter*spacing[0]);
	centerY = origin[1] + (yBodyCenter*spacing[1]);
	
	double interVertSpace = (zWingsRange.r - zBodyRange.l) * INTERVERTEBRAL_SPACE_RATIO;
	int fullVertHeight = zWingsRange.r - zBodyRange.l;
	double cylSpace = (fullVertHeight - interVertSpace * 2.5) / 3;
	double cylHeight = cylSpace * spacing[2] * MARGIN_REDUCTION;
	double backCylFullRadius = bodyRadius * spacing[0] * FRONT_BACK_CYL_RADIUS_RATIO;
	double TailCubeCenter[3], TailCubeXlen, TailCubeYLen;



	centerZ[0] = origin[2] + (zBodyRange.l + cylSpace*0.5 + 0.25*interVertSpace) * spacing[2];
	centerZ[1] = origin[2] + (zBodyRange.l + cylSpace*1.5 + 1.25*interVertSpace) * spacing[2];
	centerZ[2] = origin[2] + (zBodyRange.l + cylSpace*2.5 + 2.25*interVertSpace) *spacing[2];

		
	CreateCylinder(centerX, centerY, centerZ[0], "CYL L1", cylHeight, radius);
	CreateCylinder(centerX, centerY, centerZ[1], "CYL L2", cylHeight, radius);
	CreateCylinder(centerX, centerY, centerZ[2], "CYL L3", cylHeight, radius);

	if (wingsOnX)
	{
		//back cyl
		BackCylCenterX = centerX;
		BackCylCenterY = origin[1] + rearWall*spacing[1];

		//tail
		TailCubeCenter[1] = BackCylCenterY + (bodyRadius * spacing[0] * BODY_RADIUS_TAIL_RATIO);
		TailCubeCenter[2] = centerZ[0];
		TailCubeCenter[0] = GetTailCenter(volumeData,SIDE_X,centerX, TailCubeCenter[1], TailCubeCenter[2]);

		TailCubeXlen = TAILSIZE_MIN;
		TailCubeYLen = TAILSIZE_MAX;

	}
	else
	{
		//back cyl
		BackCylCenterX = origin[0] + rearWall*spacing[0]; 
		BackCylCenterY = centerY;

		//tail
		TailCubeCenter[0] = BackCylCenterX + (bodyRadius * spacing[0] * BODY_RADIUS_TAIL_RATIO);
		TailCubeCenter[1] = centerY;
		TailCubeCenter[2] = centerZ[0];
		TailCubeXlen = TAILSIZE_MAX;
		TailCubeYLen = TAILSIZE_MIN;
	}
	
	double backRadius1 = backCylFullRadius*BACK_CYL_MARGIN_REDUCTION;
	double backRadius2 = (backRadius1 * BACK_CYL_R1_R2_RATIO) + (backCylFullRadius-backRadius1);  //adding margin reduction


	CreateCylinder(BackCylCenterX, BackCylCenterY, centerZ[0], "Rear CYL L1 out", cylHeight, backRadius1);
	CreateCylinder(BackCylCenterX, BackCylCenterY, centerZ[0], "Rear CYL L1 in", cylHeight, backRadius2);

	CreateCylinder(BackCylCenterX, BackCylCenterY, centerZ[1], "Rear CYL L2", cylHeight, backRadius1);
	CreateCylinder(BackCylCenterX, BackCylCenterY, centerZ[1], "Rear CYL L2", cylHeight, backRadius2);

	CreateParallelepiped(TailCubeCenter, "Tail", TailCubeXlen, TailCubeYLen, cylHeight);


	albaVMELandmarkCloud *lCloud;
	albaNEW(lCloud);
	lCloud->SetName("PointRef");
	
	
	lCloud->AppendLandmark(centerX, centerY, centerZ[1], "center");
	lCloud->AppendLandmark(centerX, centerY, origin[2] + zBodyRange.l*spacing[2], "zBodyL");
	lCloud->AppendLandmark(centerX, centerY, origin[2] + zBodyRange.r*spacing[2], "zBodyR");

	
	lCloud->AppendLandmark(origin[0] + (rearWall*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "wall");
	
	lCloud->AppendLandmark(origin[0] + (xWingsRange.l*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "xWingL");
	lCloud->AppendLandmark(origin[0] + (xWingsRange.r*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "xWingR");
	lCloud->AppendLandmark(origin[0] + (xBodyRange.l*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "xBodyL");
	lCloud->AppendLandmark(origin[0] + (xBodyRange.r*spacing[0]), origin[1] + ((dims[1] / 2)*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "xBodyR");
	
	lCloud->AppendLandmark(origin[0] + ((dims[0] / 2)*spacing[0]), origin[1] + (yWingsRange.l*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "yWingL");
	lCloud->AppendLandmark(origin[0] + ((dims[0] / 2)*spacing[0]), origin[1] + (yWingsRange.r*spacing[1]),  origin[2] + ((dims[2] / 2)*spacing[2]), "yWingR");
	lCloud->AppendLandmark(origin[0] + ((dims[0] / 2)*spacing[0]), origin[1] + (yBodyRange.l*spacing[1]),  origin[2] + ((dims[2] / 2)*spacing[2]), "yBodyL");
	lCloud->AppendLandmark(origin[0] + ((dims[0] / 2)*spacing[0]), origin[1] + (yBodyRange.r*spacing[1]), origin[2] + ((dims[2] / 2)*spacing[2]), "yBodyR");


	lCloud->ReparentTo(m_Volume);

	
	
	CalculateSpinalDensityInfo((vtkImageData *)volumeData);
	CalculateBoneDensityInfo((vtkImageData *)volumeData);
	CalculateTailDensityInfo((vtkImageData *)volumeData);
	FitPoints();

}

//----------------------------------------------------------------------------
void albaOpESPCalibration::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
		if (e->GetSender() == m_Gui)
		{
			switch (e->GetId())
			{
			case ID_GENARATE_REPORT:
				CreateReport();
				break;

			case wxOK:
				OpStop(OP_RUN_OK);
				break;

			case wxCANCEL:
				OpStop(OP_RUN_CANCEL);
				break;
			}
		}
		else
		{
			Superclass::OnEvent(alba_event);
		}
	}
}

//----------------------------------------------------------------------------
Range albaOpESPCalibration::CaluculateCutOffRange(vtkImageData *imgData, double cutOff)
{
	vtkPointData 			*inputPd = imgData->GetPointData();
	vtkDataArray 			*inputScalars = inputPd->GetScalars();

	void *inputPointer = inputScalars->GetVoidPointer(0);
	int scalarDim = inputScalars->GetNumberOfTuples();

	double scalarRange[2];
	inputScalars->GetRange(scalarRange);

	switch (inputScalars->GetDataType())
	{
		case VTK_CHAR:
			return CalculateCutOfRange(scalarDim, (char*)inputPointer, cutOff);
			break;
		case VTK_UNSIGNED_CHAR:
			return CalculateCutOfRange(scalarDim, (unsigned char*)inputPointer, cutOff);
			break;
		case VTK_SHORT:
			return CalculateCutOfRange(scalarDim, (short*)inputPointer, cutOff);
			break;
		case VTK_UNSIGNED_SHORT:
			return CalculateCutOfRange(scalarDim, (unsigned short*)inputPointer, cutOff);
			break;
		case VTK_INT:
			return CalculateCutOfRange(scalarDim, (int*)inputPointer, cutOff);
			break;
		case VTK_UNSIGNED_INT:
			return CalculateCutOfRange(scalarDim, (unsigned int*)inputPointer, cutOff);
			break;
		case VTK_FLOAT:
			return CalculateCutOfRange(scalarDim, (float*)inputPointer, cutOff);
			break;
		case VTK_DOUBLE:
			return CalculateCutOfRange(scalarDim, (double*)inputPointer, cutOff);
			break;
		default:
			albaLogMessage("Calibration: Scalar type is not supported");
			Range empty;
			empty.l = empty.r = -1;
			return empty;
	}
}

//----------------------------------------------------------------------------
template<typename DataType>
Range albaOpESPCalibration::CalculateCutOfRange(int scalarSizes, DataType * scalars,double cutOff)
{
	int inCutOff = false;
	int lastInCutOff = false;
	Range range;
	range.l = range.r = -1;
	int left = -1;

	
	lastInCutOff = scalars[0] > cutOff;
	
	for (int i = 1; i < scalarSizes; i++)
	{
		inCutOff = scalars[i] >= cutOff;

		if (inCutOff && lastInCutOff && range.l == -1)
			range.l = i-1;

		if (inCutOff && lastInCutOff)
			range.r = i;

		lastInCutOff = inCutOff;
	}


	albaLogMessage("Cut-off at:%f [%d,%d]",cutOff, range.l,range.r);

	return range;
}


//----------------------------------------------------------------------------
albaVME *albaOpESPCalibration::CreateParallelepiped(double center[3], char * name, double xLen, double yLen, double height)
{
	albaTransform cylinderBase;
	cylinderBase.RotateX(90, false);
	cylinderBase.Translate(center, false);
	cylinderBase.Update();


	albaVMESurfaceParametric *cylinder;
	albaNEW(cylinder);
	cylinder->SetName(name);
	cylinder->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CUBE);
	cylinder->SetCubeXLength(xLen);
	cylinder->SetCubeYLength(yLen);
	cylinder->SetCubeZLength(height);
	cylinder->SetMatrix(cylinderBase.GetMatrix());

	cylinder->ReparentTo(m_Volume);

	return cylinder;
}


//----------------------------------------------------------------------------
albaVME *albaOpESPCalibration::CreateCylinder(double centerX, double centerY, double centerZ, char * name, double height, double radius)
{
	albaTransform cylinderBase;
	cylinderBase.RotateX(90, false);
	cylinderBase.Translate(centerX, centerY, centerZ, false);
	cylinderBase.Update();


	albaVMESurfaceParametric *cylinder;
	albaNEW(cylinder);
	cylinder->SetName(name);
	cylinder->SetGeometryType(albaVMESurfaceParametric::PARAMETRIC_CYLINDER);
	cylinder->SetCylinderHeight(height);
	cylinder->SetCylinderRadius(radius);
	cylinder->SetMatrix(cylinderBase.GetMatrix());

	cylinder->ReparentTo(m_Volume);

	return cylinder;
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::GetCenter(vtkImageData *rg, Range xRange, Range yRange, int zEight,  double *center/*, albaVMELandmarkCloud *lmc*/)
{
	long xAccum=0, yAccum=0;
	long xMin = VTK_LONG_MAX, xMax = VTK_LONG_MIN, yMin = VTK_LONG_MAX, yMax = VTK_LONG_MIN;

	int acc=0;

	int currentPoint[3];
	currentPoint[2] = zEight;


	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	double *spacing = rg->GetSpacing();

	double scalarRange[2];
	scalars->GetRange(scalarRange);
	double maxScalar = scalarRange[1];

	double circleThreshold =  maxScalar;

	for (int y = yRange.l; y <= yRange.r; y++)
	{
		for (int x=xRange.l; x <= xRange.r; x++)
		{
			currentPoint[0] = x;
			currentPoint[1] = y;
			
			vtkIdType pointId = rg->ComputePointId(currentPoint);
			if (scalars->GetTuple1(pointId) > circleThreshold)
			{
				acc++;
				xAccum += x;
				yAccum += y;

				if (x < xMin)	xMin = x;
				if (y < yMin)	yMin = y;
				if (x > xMax)	xMax = x;
				if (y > yMax)	yMax = y;
								
				/* 
				double pCoord[3];
				pCoord[0] = xCoordinates->GetTuple1(x);
				pCoord[1] = yCoordinates->GetTuple1(y);
				pCoord[2] = zCoordinates->GetTuple1(zEight);

				albaString lName;
				lName.Printf("Point %d [%d %d %d]", acc, x, y, zEight);

				lmc->AppendLandmark(pCoord[0], pCoord[1], pCoord[2], lName.GetCStr());
				/**/
			}
		}
	}
	if (acc == 0)
		return false;

	if (xMin == VTK_LONG_MAX || xMax == VTK_LONG_MIN || yMin == VTK_LONG_MAX || yMax == VTK_LONG_MIN)
		return false;


	center[0] = (spacing[0] * (xMin + xMax)) / 2.0;
	center[1] = (spacing[1] * (yMin + yMax)) / 2.0; 
	center[2] = spacing[2] * zEight;

	return true;
}

//----------------------------------------------------------------------------
double albaOpESPCalibration::GetRadius(vtkImageData *rg, Range xRange, Range yRange, int zEight, double center[3], bool getInner)
{
	albaVect3d vCenter, vPoint;
	double radius;
	radius = getInner ? VTK_DOUBLE_MAX : VTK_DOUBLE_MIN;
		

	vCenter = center;

	int currentPoint[3];
	currentPoint[2] = zEight;

	double *spacing = rg->GetSpacing();

		
	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	double scalarRange[2];
	scalars->GetRange(scalarRange);
	double maxScalar = scalarRange[1];

	double circleThreshold = maxScalar;
	
	for (int y = yRange.l; y <= yRange.r; y++)
	{
		for (int x = xRange.l; x <= xRange.r; x++)
		{
			currentPoint[0] = x;
			currentPoint[1] = y;

			vtkIdType pointId = rg->ComputePointId(currentPoint);
			if (scalars->GetTuple1(pointId) > circleThreshold)
			{
				vPoint[0] = x*spacing[0];
				vPoint[1] = y*spacing[1];
				vPoint[2] = zEight*spacing[2];

				double dist = vCenter.Distance(vPoint);
				radius = getInner ? MIN(radius, dist) : MAX(radius, dist);
			}
		}
	}
	return radius;
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::CalculateSpinalDensityInfo(vtkImageData *rg)
{
	double *spacing = rg->GetSpacing();

	albaVect3d currentPointVect, center;
	currentPointVect[2] = center[2] = 0;
	int currentPoint[3];

	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	for (int i = 0; i < 3; i++)
	{
		std::vector<double> values;
		Cylinder cyl = m_InnerCylinders[i];
		center[0] = cyl.centerX;
		center[1] = cyl.centerY;
		double sumDensity = 0;
		double acc = 0;
		double minValue = VTK_DOUBLE_MAX, maxValue = VTK_DOUBLE_MIN;

		/* 
		albaVMELandmarkCloud *lCloud;
		albaNEW(lCloud);
		lCloud->SetName("PointMean");
		/**/

		for (int z = cyl.zRange.l; z <= cyl.zRange.r; z++)
			for (int y = cyl.yRange.l; y <= cyl.yRange.r; y++)
				for (int x = cyl.xRange.l; x <= cyl.xRange.r; x++)
				{
					currentPointVect[0] = x*spacing[0];
					currentPointVect[1] = y*spacing[1];
					if (center.Distance(currentPointVect) < cyl.radius)
					{
						currentPoint[0] = x;
						currentPoint[1] = y;
						currentPoint[2] = z;
						vtkIdType pointId = rg->ComputePointId(currentPoint);
						double scalar = scalars->GetTuple1(pointId);
						sumDensity += scalar;
						acc += 1;
						maxValue = MAX(scalar, maxValue);
						minValue = MIN(scalar, minValue);
						values.push_back(scalar);

						/*
						if (!(((int)acc) % 15) &&  (z == cyl.zRange.l || z == cyl.zRange.r)) {
							double pCoord[3];
							pCoord[0] = xCoordinates->GetTuple1(x);
							pCoord[1] = yCoordinates->GetTuple1(y);
							pCoord[2] = zCoordinates->GetTuple1(z);

							albaString lName;
							lName.Printf("Point %d ", (int)acc);

							lCloud->AppendLandmark(pCoord[0], pCoord[1], pCoord[2], lName.GetCStr());
						}
						/**/
					}
				}
		
		double mean = sumDensity / acc;
		double devSum = 0;
		for (int i = 0; i < acc; i++)
		{
			devSum += (values[i] - mean) * (values[i] - mean);
		}
		double stdDev = sqrt(devSum / acc);
		
		m_AreaInfo[i].mean = mean;
		m_AreaInfo[i].min = minValue;
		m_AreaInfo[i].max = maxValue;
		m_AreaInfo[i].sdtdev = stdDev;
		m_AreaInfo[i].nPoints = acc;
		
		albaLogMessage("\Spinal area L%d:\n Mean:%f\n Min:%f\n Max:%f\n stdDev:%f", i+1, mean, minValue, maxValue, stdDev);

		/*
		lCloud->ReparentTo(m_Volume);
		/**/
	}



	return true;
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::CalculateBoneDensityInfo(vtkImageData *rg)
{
	double *spacing = rg->GetSpacing();

	albaVect3d currentPointVect, center;
	currentPointVect[2] = center[2] = 0;
	int currentPoint[3];

	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	double sumDensity = 0, acc = 0;
	double localSumD[2] = { 0,0 }, localAcc[2] = { 0,0 };
	double minValue = VTK_DOUBLE_MAX, maxValue = VTK_DOUBLE_MIN;

	std::vector<double> values;

	for (int i = 0; i < 2; i++)
	{
		BoneCylinder cyl = m_BoneCylinders[i];
		center[0] = cyl.centerX;
		center[1] = cyl.centerY;
		
		/*
		albaVMELandmarkCloud *lCloud;
		albaNEW(lCloud);
		lCloud->SetName("PointMean");
		/**/

		for (int z = cyl.zRange.l; z <= cyl.zRange.r; z++)
			for (int y = cyl.yRange.l; y <= cyl.yRange.r; y++)
				for (int x = cyl.xRange.l; x <= cyl.xRange.r; x++)
				{
					currentPointVect[0] = x*spacing[0];
					currentPointVect[1] = y*spacing[1];
					
					double distance = center.Distance(currentPointVect);
					if (distance > cyl.InnerRadius && distance < cyl.OuterRadius)
					{
						currentPoint[0] = x;
						currentPoint[1] = y;
						currentPoint[2] = z;
						vtkIdType pointId = rg->ComputePointId(currentPoint);
						double scalar = scalars->GetTuple1(pointId);
						localSumD[i] += scalar;
						sumDensity += scalar;
						localAcc[i] += 1;
						acc += 1;
						maxValue = MAX(scalar, maxValue);
						minValue = MIN(scalar, minValue);
						values.push_back(scalar);

						/*
						if (!(((int)acc) % 3) && (z == cyl.zRange.l || z == cyl.zRange.r)) {
							double pCoord[3];
							pCoord[0] = xCoordinates->GetTuple1(x);
							pCoord[1] = yCoordinates->GetTuple1(y);
							pCoord[2] = zCoordinates->GetTuple1(z);

							albaString lName;
							lName.Printf("Point %d ", (int)acc);

							lCloud->AppendLandmark(pCoord[0], pCoord[1], pCoord[2], lName.GetCStr());
						}
						/**/
					}
				}

		m_BoneMean[i] = localSumD[i] / localAcc[i];

		albaLogMessage("\nBone area #%d:\n Mean:%f\n ", i, m_BoneMean[i]);

		/*
		lCloud->ReparentTo(m_Volume);
		/**/
	}

	//TODO ADD check to localmean difference

	double mean = sumDensity / acc;
	double devSum = 0;
	for (int i = 0; i < acc; i++)
	{
		devSum += (values[i] - mean) * (values[i] - mean);
	}
	double stdDev = sqrt(devSum / acc);

	m_AreaInfo[3].mean = mean;
	m_AreaInfo[3].min = minValue;
	m_AreaInfo[3].max = maxValue;
	m_AreaInfo[3].sdtdev = stdDev;
	m_AreaInfo[3].nPoints = acc;

	albaLogMessage("\Bone total area:\n Mean:%f\n Min:%f\n Max:%f\n stdDev:%f", mean, minValue, maxValue, stdDev);



	return true;
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::CalculateTailDensityInfo(vtkImageData *rg)
{
	double *spacing = rg->GetSpacing();

	int currentPoint[3];

	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	for (int i = 0; i < 3; i++)
	{
		std::vector<double> values;
		double sumDensity = 0;
		double acc = 0;
		double minValue = VTK_DOUBLE_MAX, maxValue = VTK_DOUBLE_MIN;

		/*
		albaVMELandmarkCloud *lCloud;
		albaNEW(lCloud);
		lCloud->SetName("PointMean");
		/**/

		for (int z = m_Tail.zRange.l; z <= m_Tail.zRange.r; z++)
			for (int y = m_Tail.yRange.l; y <= m_Tail.yRange.r; y++)
				for (int x = m_Tail.xRange.l; x <= m_Tail.xRange.r; x++)
				{
				
						currentPoint[0] = x;
						currentPoint[1] = y;
						currentPoint[2] = z;
						vtkIdType pointId = rg->ComputePointId(currentPoint);
						double scalar = scalars->GetTuple1(pointId);
						sumDensity += scalar;
						acc += 1;
						maxValue = MAX(scalar, maxValue);
						minValue = MIN(scalar, minValue);
						values.push_back(scalar);

						/*
						if (!(((int)acc) % 15) &&  (z == cyl.zRange.l || z == cyl.zRange.r)) {
						double pCoord[3];
						pCoord[0] = xCoordinates->GetTuple1(x);
						pCoord[1] = yCoordinates->GetTuple1(y);
						pCoord[2] = zCoordinates->GetTuple1(z);

						albaString lName;
						lName.Printf("Point %d ", (int)acc);

						lCloud->AppendLandmark(pCoord[0], pCoord[1], pCoord[2], lName.GetCStr());
						}
						/**/
				}

		double mean = sumDensity / acc;
		double devSum = 0;
		for (int i = 0; i < acc; i++)
		{
			devSum += (values[i] - mean) * (values[i] - mean);
		}
		double stdDev = sqrt(devSum / acc);

		m_AreaInfo[i].mean = mean;
		m_AreaInfo[i].min = minValue;
		m_AreaInfo[i].max = maxValue;
		m_AreaInfo[i].sdtdev = stdDev;
		m_AreaInfo[i].nPoints = acc;

		albaLogMessage("\nTail area #%d:\n Mean:%f\n Min:%f\n Max:%f\n stdDev:%f", i, mean, minValue, maxValue, stdDev);

		/*
		lCloud->ReparentTo(m_Volume);
		/**/
	}

	return true;
}

//----------------------------------------------------------------------------
double albaOpESPCalibration::GetTailCenter(vtkImageData *rg, int side, double x, double y, double z)
{
	vtkDataArray* scalars = rg->GetPointData()->GetScalars();

	vtkIdType startCenter = rg->FindPoint(x, y, z);
	double *origin = rg->GetOrigin();
	double *spacing = rg->GetSpacing();

	int *dims = rg->GetDimensions();
	int zPos = startCenter / (dims[0]*dims[1]);
	int yResidue = startCenter - (zPos * dims[0] * dims[1]);
	int yPos = yResidue / dims[0];
	int xPos = yResidue - (yPos * dims[0]);
	bool onTail = true;

	int l, r;
	if (side == SIDE_X)
	{
		l = xPos;
		while (onTail)
		{
			vtkIdType currentPoint = zPos * dims[0] * dims[1] + yPos * dims[0] + l;
			onTail = scalars->GetTuple1(currentPoint) > CUT_OFF_WINGS;
			l--;
		}

		onTail = true;
		r = xPos;
		while (onTail)
		{
			vtkIdType currentPoint = zPos * dims[0] * dims[1] + yPos * dims[0] + r;
			onTail = scalars->GetTuple1(currentPoint) > CUT_OFF_WINGS;
			r++;
		}
		
		//TODO ADD CHECK ON SIZE

		double center = (l + r) / 2.0;
		return origin[0] + center*spacing[0];
	
	}
	else
	{
		l = yPos;
		while (onTail)
		{
			vtkIdType currentPoint = zPos * dims[0] * dims[1] + l * dims[0] + xPos;
			onTail = scalars->GetTuple1(currentPoint) > CUT_OFF_WINGS;
			l--;
		}

		onTail = true;
		r = yPos;
		while (onTail)
		{
			vtkIdType currentPoint = zPos * dims[0] * dims[1] + l * dims[0] + xPos;
			onTail = scalars->GetTuple1(currentPoint) > CUT_OFF_WINGS;
			r++;
		}

		//TODO ADD CHECK ON SIZE

		double center = (l + r) / 2.0;
		return origin[1] + center*spacing[1];

	}
}

//----------------------------------------------------------------------------
bool albaOpESPCalibration::FitPoints()
{
	int nPoints = 4;
	double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

	double expected[4] = EXPECTED_VALUES;

	for (int i = 0; i < nPoints; i++) 
	{
		double x = m_AreaInfo[i].mean;
		double y = expected[i];

		sumX += x;
		sumY += y;
		sumXY += x * y;
		sumX2 += x * x;
	}
	double xMean = sumX / nPoints;
	double yMean = sumY / nPoints;
	double denominator = sumX2 - sumX * xMean;
	// You can tune the eps (1e-7) below for your specific task
	if (std::fabs(denominator) < 1e-7) 
	{
		// Fail: it seems a vertical line
		return false;
	}
	m_Slope = (sumXY - sumX * yMean) / denominator;
	m_Intercept = yMean - m_Slope * xMean;

	//calculate squared residues, their sum etc.
	double yRes = 0;
	double yResSum = 0;
	double res = 0;
	double resSum = 0;
	for (int i = 0; i < 4; i++)
	{
		double x = m_AreaInfo[i].mean;
		double y = expected[i];

		//current (y_i - a0 - a1 * x_i)^2
		yRes = pow((y - m_Intercept - (m_Slope * x)), 2);

		//sum of (y_i - a0 - a1 * x_i)^2
		yResSum += yRes;

		//current residue squared (y_i - AVGy)^2
		res = pow(y - yMean, 2);

		//sum of squared residues
		resSum += res;

	}

	//calculate r^2 coefficient of determination
	m_RSquare = (resSum - yResSum) / resSum;

	albaLogMessage("RESULT: \n Slope:%f \n Intercept:%f", m_Slope, m_Intercept);
	return true;
}

//----------------------------------------------------------------------------
void albaOpESPCalibration::CreateGui()
{
	// Interface:
	m_Gui = new albaGUI(this);
	m_Gui->Divider();
	m_Gui->Label("Calibration:");
	m_Gui->Label("(mgHA/cm^3 = slope * GV + intercept)");
	m_Gui->Double(-1, "Slope", &m_Slope,VTK_DOUBLE_MIN,VTK_DOUBLE_MAX,4);
	m_Gui->Double(-1, "Intercept", &m_Intercept, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 4);
	m_Gui->Double(-1, "R^2", &m_RSquare, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 4);
	m_Gui->Label("");
	m_Gui->Divider(1);

			
	m_Gui->Label("Area 200:");
	m_Gui->Double(-1, "Mean", &m_AreaInfo[0].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[0].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[0].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[0].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);

	m_Gui->Label("");
	m_Gui->Label("Area 100:");
	m_Gui->Double(-1, "Mean", &m_AreaInfo[1].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[1].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[1].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[1].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);

	m_Gui->Label("");
	m_Gui->Label("Area 50:");
	m_Gui->Double(-1, "Mean", &m_AreaInfo[2].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[2].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[2].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[2].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);

	m_Gui->Label("");
	m_Gui->Label("Area 800:");
	m_Gui->Label("Area A:");
	m_Gui->Double(-1, "Mean", &m_BoneMean[0], VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Label("Area B:");
	m_Gui->Double(-1, "Mean", &m_BoneMean[1], VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Label("Full area:");
	m_Gui->Double(-1, "Mean", &m_AreaInfo[3].mean, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "StdDev", &m_AreaInfo[3].sdtdev, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Min", &m_AreaInfo[3].min, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);
	m_Gui->Double(-1, "Max", &m_AreaInfo[3].max, VTK_DOUBLE_MIN, VTK_DOUBLE_MAX, 2);

	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->Button(ID_GENARATE_REPORT, "Generate Report");

	m_Gui->Enable(-1, false);

	m_Gui->Label("");
	m_Gui->Divider(1);
	m_Gui->OkCancel();

	ShowGui();
}

//----------------------------------------------------------------------------
char ** albaOpESPCalibration::GetIcon()
{
#include "pic/MENU_CALIBRATION_ICON.xpm"
	return MENU_CALIBRATION_ICON_xpm;
}

// Create Report
//----------------------------------------------------------------------------
bool albaOpESPCalibration::CreateReport()
{ 
	/*
	albaString fileNameFullPath = albaGetDocumentsDirectory().c_str();
	fileNameFullPath.Append("\\NewReport.rtf");

	albaString wildc = "Report file (*.rtf)|*.rtf; |Report file (*.csv)|*.csv";
	albaString newFileName = albaGetSaveFile(fileNameFullPath.GetCStr(), wildc).c_str();

	if (newFileName == "") return false;

	//////////////////////////////////////////////////////////////////////////

	m_Parser = new albaHTMLTemplateParser;

	wxString pathName, fileName, extension;
	wxSplitPath(newFileName, &pathName, &fileName, &extension);
	wxString templatePath = appUtils::GetConfigDirectory().c_str();
	templatePath += "\\Templates\\";

	wxString templateFile = (templatePath + "ReportCalibrationTemplate." + extension.c_str()).c_str();
	m_Parser->SetTemplateFromFile(templateFile);

	//////////////////////////////////////////////////////////////////////////
	// Write Data

	time_t rawtime;
	struct tm * timeinfo;  time(&rawtime);
	timeinfo = localtime(&rawtime);

	wxString reportDate;
	reportDate.Printf("%d-%d-%d", timeinfo->tm_mday, timeinfo->tm_mon + 1, +1900 + timeinfo->tm_year);

	m_Parser->AddVar("Cal_ReportDate", reportDate);

	m_Parser->AddVar("Cal_Slope", m_Slope);
	m_Parser->AddVar("Cal_Intercept", m_Intercept);
	m_Parser->AddVar("Cal_R2", m_RSquare);

	m_Parser->AddVar("Cal_Area200_Mean", m_AreaInfo[0].mean);
	m_Parser->AddVar("Cal_Area200_StdDev", m_AreaInfo[0].sdtdev);
	m_Parser->AddVar("Cal_Area200_Min", m_AreaInfo[0].min);
	m_Parser->AddVar("Cal_Area200_Max", m_AreaInfo[0].max);

	m_Parser->AddVar("Cal_Area100_Mean", m_AreaInfo[1].mean);
	m_Parser->AddVar("Cal_Area100_StdDev", m_AreaInfo[1].sdtdev);
	m_Parser->AddVar("Cal_Area100_Min", m_AreaInfo[1].min);
	m_Parser->AddVar("Cal_Area100_Max", m_AreaInfo[1].max);

	m_Parser->AddVar("Cal_Area50_Mean", m_AreaInfo[2].mean);
	m_Parser->AddVar("Cal_Area50_StdDev", m_AreaInfo[2].sdtdev);
	m_Parser->AddVar("Cal_Area50_Min", m_AreaInfo[2].min);
	m_Parser->AddVar("Cal_Area50_Max", m_AreaInfo[2].max);
	
	m_Parser->AddVar("Cal_Area800A_Mean", m_BoneMean[0]);
	m_Parser->AddVar("Cal_Area800B_Mean", m_BoneMean[1]);

	m_Parser->AddVar("Cal_FullArea_Mean", m_AreaInfo[3].mean);
	m_Parser->AddVar("Cal_FullArea_StdDev", m_AreaInfo[3].sdtdev);
	m_Parser->AddVar("Cal_FullArea_Min", m_AreaInfo[3].min);
	m_Parser->AddVar("Cal_FullArea_Max", m_AreaInfo[3].max);

	//m_View = ((appViewOrthoSlice*)m_View)->GetSubView(0); // Select SubView
	m_View->CameraUpdate();

	m_Parser->AddImageVar("Cal_Image", m_View);

	//////////////////////////////////////////////////////////////////////////

	m_Parser->Parse();
	m_Parser->WriteOutputFile(newFileName.GetCStr());

	delete m_Parser;

	// Open Report File
	wxString url = "file:///";
	url = url + newFileName.GetCStr();
	url.Replace("\\", "/");
	albaLogMessage("Opening %s", url.c_str());
	wxString command = "rundll32.exe url.dll,FileProtocolHandler ";
	command = command + url;
	wxExecute(command);
	*/
	return true;
	
}


//----------------------------------------------------------------------------
void albaOpESPCalibration::AddCalibrationToDB()
{
	/*
	appLogic *logic = (appLogic *)GetLogicManager();
	appCalibrationDB * calbibrationDB = logic->GetCalbibrationDB();

	Calibration cal;

	if (appCalibrationDB::GetCalibrationKeysFromVolume(m_Volume,cal) == false)
		return;

	cal.m_Slope = m_Slope;
	cal.m_Intercept = m_Intercept;
	cal.m_RSquared = m_RSquare;

	cal.m_A200Mean = m_AreaInfo[0].mean;
	cal.m_A200StdDev = m_AreaInfo[0].sdtdev;
	cal.m_A200Min = m_AreaInfo[0].min;
	cal.m_A200Max = m_AreaInfo[0].max;
	
	cal.m_A100Mean = m_AreaInfo[1].mean;
	cal.m_A100StdDev = m_AreaInfo[1].sdtdev;
	cal.m_A100Min = m_AreaInfo[1].min;
	cal.m_A100Max = m_AreaInfo[1].max;

	cal.m_A50Mean = m_AreaInfo[2].mean;
	cal.m_A50StdDev = m_AreaInfo[2].sdtdev;
	cal.m_A50Min = m_AreaInfo[2].min;
	cal.m_A50Max = m_AreaInfo[2].max;

	cal.m_A800Mean = m_AreaInfo[3].mean;
	cal.m_A800StdDev = m_AreaInfo[3].sdtdev;
	cal.m_A800Min = m_AreaInfo[3].min;
	cal.m_A800Max = m_AreaInfo[3].max;

	cal.m_A800aMean = m_BoneMean[0];
	cal.m_A800bMean = m_BoneMean[1];

	calbibrationDB->AddCalibration(cal);
	*/
}



