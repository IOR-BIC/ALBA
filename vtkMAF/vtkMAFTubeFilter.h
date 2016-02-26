/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkMAFLandmarkCloudPolydataFilter.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Ken Martin, Will Schroeder, Bill Lorensen, Gianluigi Crimi
==========================================================================
Copyright (c) 2012
=========================================================================*/

#ifndef __vtkMAFTubeFilter_h
#define __vtkMAFTubeFilter_h

#include "vtkPolyDataToPolyDataFilter.h"

#define VTK_VARY_RADIUS_OFF 0
#define VTK_VARY_RADIUS_BY_SCALAR 1
#define VTK_VARY_RADIUS_BY_VECTOR 2
#define VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR 3

#define VTK_TCOORDS_OFF                    0
#define VTK_TCOORDS_FROM_NORMALIZED_LENGTH 1
#define VTK_TCOORDS_FROM_LENGTH            2
#define VTK_TCOORDS_FROM_SCALARS           3

class vtkCellArray;
class vtkCellData;
class vtkDataArray;
class vtkFloatArray;
class vtkPointData;
class vtkPoints;

/*This Class is basically a copy of vtkTubeFilter class except for managing some error cases.
	when two consecutive points on a line are the same point (or are to close) vtk tube filter does not create the output
	this class skips consecutive equal points*/
class VTK_GRAPHICS_EXPORT vtkMAFTubeFilter : public vtkPolyDataToPolyDataFilter
{
public:
	vtkTypeRevisionMacro(vtkMAFTubeFilter, vtkPolyDataToPolyDataFilter);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Construct object with radius 0.5, radius variation turned off, the
	// number of sides set to 3, and radius factor of 10.
	static vtkMAFTubeFilter *New();

	// Description:
	// Set the minimum tube radius (minimum because the tube radius may vary).
	vtkSetClampMacro(Radius, double, 0.0, VTK_DOUBLE_MAX);
	vtkGetMacro(Radius, double);

	// Description:
	// Turn on/off the variation of tube radius with scalar value.
	vtkSetClampMacro(VaryRadius, int,
		VTK_VARY_RADIUS_OFF, VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR);
	vtkGetMacro(VaryRadius, int);
	void SetVaryRadiusToVaryRadiusOff()
	{
		this->SetVaryRadius(VTK_VARY_RADIUS_OFF);
	};
	void SetVaryRadiusToVaryRadiusByScalar()
	{
		this->SetVaryRadius(VTK_VARY_RADIUS_BY_SCALAR);
	};
	void SetVaryRadiusToVaryRadiusByVector()
	{
		this->SetVaryRadius(VTK_VARY_RADIUS_BY_VECTOR);
	};
	void SetVaryRadiusToVaryRadiusByAbsoluteScalar()
	{
		this->SetVaryRadius(VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR);
	};
	const char *GetVaryRadiusAsString();

	// Description:
	// Set the number of sides for the tube. At a minimum, number of sides is 3.
	vtkSetClampMacro(NumberOfSides, int, 3, VTK_LARGE_INTEGER);
	vtkGetMacro(NumberOfSides, int);

	// Description:
	// Set the maximum tube radius in terms of a multiple of the minimum radius.
	vtkSetMacro(RadiusFactor, double);
	vtkGetMacro(RadiusFactor, double);

	// Description:
	// Set the default normal to use if no normals are supplied, and the
	// DefaultNormalOn is set.
	vtkSetVector3Macro(DefaultNormal, double);
	vtkGetVectorMacro(DefaultNormal, double, 3);

	// Description:
	// Set a boolean to control whether to use default normals.
	// DefaultNormalOn is set.
	vtkSetMacro(UseDefaultNormal, int);
	vtkGetMacro(UseDefaultNormal, int);
	vtkBooleanMacro(UseDefaultNormal, int);

	// Description:
	// Set a boolean to control whether tube sides should share vertices.
	// This creates independent strips, with constant normals so the
	// tube is always faceted in appearance.
	vtkSetMacro(SidesShareVertices, int);
	vtkGetMacro(SidesShareVertices, int);
	vtkBooleanMacro(SidesShareVertices, int);

	// Description:
	// Turn on/off whether to cap the ends with polygons.
	vtkSetMacro(Capping, int);
	vtkGetMacro(Capping, int);
	vtkBooleanMacro(Capping, int);

	// Description:
	// Control the striping of the tubes. If OnRatio is greater than 1,
	// then every nth tube side is turned on, beginning with the Offset
	// side.
	vtkSetClampMacro(OnRatio, int, 1, VTK_LARGE_INTEGER);
	vtkGetMacro(OnRatio, int);

	// Description:
	// Control the striping of the tubes. The offset sets the
	// first tube side that is visible. Offset is generally used with
	// OnRatio to create nifty striping effects.
	vtkSetClampMacro(Offset, int, 0, VTK_LARGE_INTEGER);
	vtkGetMacro(Offset, int);

	// Description:
	// Control whether and how texture coordinates are produced. This is
	// useful for striping the tube with length textures, etc. If you
	// use scalars to create the texture, the scalars are assumed to be
	// monotonically increasing (or decreasing).
	vtkSetClampMacro(GenerateTCoords, int, VTK_TCOORDS_OFF,
		VTK_TCOORDS_FROM_SCALARS);
	vtkGetMacro(GenerateTCoords, int);
	void SetGenerateTCoordsToOff()
	{
		this->SetGenerateTCoords(VTK_TCOORDS_OFF);
	}
	void SetGenerateTCoordsToNormalizedLength()
	{
		this->SetGenerateTCoords(VTK_TCOORDS_FROM_NORMALIZED_LENGTH);
	}
	void SetGenerateTCoordsToUseLength()
	{
		this->SetGenerateTCoords(VTK_TCOORDS_FROM_LENGTH);
	}
	void SetGenerateTCoordsToUseScalars()
	{
		this->SetGenerateTCoords(VTK_TCOORDS_FROM_SCALARS);
	}
	const char *GetGenerateTCoordsAsString();

	// Description:
	// Control the conversion of units during the texture coordinates
	// calculation. The TextureLength indicates what length (whether 
	// calculated from scalars or length) is mapped to the [0,1)
	// texture space.
	vtkSetClampMacro(TextureLength, double, 0.000001, VTK_LARGE_INTEGER);
	vtkGetMacro(TextureLength, double);

protected:
	vtkMAFTubeFilter();
	~vtkMAFTubeFilter() {}

	// Usual data generation method
	void Execute();

	double Radius; //minimum radius of tube
	int VaryRadius; //controls radius variation
	int NumberOfSides; //number of sides to create tube
	double RadiusFactor; //maxium allowablew radius
	double DefaultNormal[3];
	int UseDefaultNormal;
	int SidesShareVertices;
	int Capping; //control whether tubes are capped
	int OnRatio; //control the generation of the sides of the tube
	int Offset;  //control the generation of the sides
	int GenerateTCoords; //control texture coordinate generation
	double TextureLength; //this length is mapped to [0,1) texture space

	/* Added Function to clean lines before tube process*/
	void CleanLine(vtkPoints *inPts, vtkIdType npts, vtkIdType *pts, vtkIdType &newNpts, vtkIdType *&newPts);
													
	// Helper methods
	int GeneratePoints(vtkIdType offset, vtkIdType npts, vtkIdType *pts,
		vtkPoints *inPts, vtkPoints *newPts,
		vtkPointData *pd, vtkPointData *outPD,
		vtkFloatArray *newNormals, vtkDataArray *inScalars,
		double range[2], vtkDataArray *inVectors, double maxNorm,
		vtkDataArray *inNormals);
	void GenerateStrips(vtkIdType offset, vtkIdType npts, vtkIdType *pts,
		vtkIdType inCellId, vtkCellData *cd, vtkCellData *outCD,
		vtkCellArray *newStrips);
	void GenerateTextureCoords(vtkIdType offset, vtkIdType npts, vtkIdType *pts,
		vtkPoints *inPts, vtkDataArray *inScalars,
		vtkFloatArray *newTCoords);
	vtkIdType ComputeOffset(vtkIdType offset, vtkIdType npts);

	// Helper data members
	double Theta;

private:
	vtkMAFTubeFilter(const vtkMAFTubeFilter&);  // Not implemented.
	void operator=(const vtkMAFTubeFilter&);  // Not implemented.
};

#endif
