/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: vtkALBALandmarkCloudPolydataFilter.cxx,v $
Language:  C++
Date:      $Date: 2009-10-08 13:44:29 $
Version:   $Revision: 1.1.2.5 $
Authors:   Ken Martin, Will Schroeder, Bill Lorensen, Gianluigi Crimi
==========================================================================
Copyright (c) 2012
=========================================================================*/

#ifndef __vtkALBATubeFilter_h
#define __vtkALBATubeFilter_h

#include "vtkPolyDataAlgorithm.h"

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
class vtkALBATubeFilter : public vtkPolyDataAlgorithm
{
public:
	vtkTypeMacro(vtkALBATubeFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  /**
   * Construct object with radius 0.5, radius variation turned off, the
   * number of sides set to 3, and radius factor of 10.
   */
	static vtkALBATubeFilter *New();

  //@{
  /**
   * Set the minimum tube radius (minimum because the tube radius may vary).
   */
	vtkSetClampMacro(Radius, double, 0.0, VTK_DOUBLE_MAX);
	vtkGetMacro(Radius, double);
  //@}

  //@{
  /**
   * Turn on/off the variation of tube radius with scalar value.
   */
	vtkSetClampMacro(VaryRadius, int,
		VTK_VARY_RADIUS_OFF, VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR);
	vtkGetMacro(VaryRadius, int);
	void SetVaryRadiusToVaryRadiusOff()
    {this->SetVaryRadius(VTK_VARY_RADIUS_OFF);};
	void SetVaryRadiusToVaryRadiusByScalar()
    {this->SetVaryRadius(VTK_VARY_RADIUS_BY_SCALAR);};
	void SetVaryRadiusToVaryRadiusByVector()
    {this->SetVaryRadius(VTK_VARY_RADIUS_BY_VECTOR);};
	void SetVaryRadiusToVaryRadiusByAbsoluteScalar()
    {this->SetVaryRadius(VTK_VARY_RADIUS_BY_ABSOLUTE_SCALAR);};
	const char *GetVaryRadiusAsString();
  //@}

  //@{
  /**
   * Set the number of sides for the tube. At a minimum, number of sides is 3.
   */
	vtkSetClampMacro(NumberOfSides, int, 3, VTK_INT_MAX);
	vtkGetMacro(NumberOfSides, int);
  //@}

  //@{
  /**
   * Set the maximum tube radius in terms of a multiple of the minimum radius.
   */
	vtkSetMacro(RadiusFactor, double);
	vtkGetMacro(RadiusFactor, double);
  //@}

  //@{
  /**
   * Set the default normal to use if no normals are supplied, and the
   * DefaultNormalOn is set.
   */
	vtkSetVector3Macro(DefaultNormal, double);
	vtkGetVectorMacro(DefaultNormal, double, 3);
  //@}

  //@{
  /**
   * Set a boolean to control whether to use default normals.
   * DefaultNormalOn is set.
   */
	vtkSetMacro(UseDefaultNormal, int);
	vtkGetMacro(UseDefaultNormal, int);
	vtkBooleanMacro(UseDefaultNormal, int);
  //@}

  //@{
  /**
   * Set a boolean to control whether tube sides should share vertices.
   * This creates independent strips, with constant normals so the
   * tube is always faceted in appearance.
   */
	vtkSetMacro(SidesShareVertices, int);
	vtkGetMacro(SidesShareVertices, int);
	vtkBooleanMacro(SidesShareVertices, int);
  //@}

  //@{
  /**
   * Turn on/off whether to cap the ends with polygons. Initial value is off.
   */
	vtkSetMacro(Capping, int);
	vtkGetMacro(Capping, int);
	vtkBooleanMacro(Capping, int);
  //@}

  //@{
  /**
   * Control the striping of the tubes. If OnRatio is greater than 1,
   * then every nth tube side is turned on, beginning with the Offset
   * side.
   */
	vtkSetClampMacro(OnRatio, int, 1, VTK_INT_MAX);
	vtkGetMacro(OnRatio, int);
  //@}

  //@{
  /**
   * Control the striping of the tubes. The offset sets the
   * first tube side that is visible. Offset is generally used with
   * OnRatio to create nifty striping effects.
   */
	vtkSetClampMacro(Offset, int, 0, VTK_INT_MAX);
	vtkGetMacro(Offset, int);
  //@}

  //@{
  /**
   * Control whether and how texture coordinates are produced. This is
   * useful for striping the tube with length textures, etc. If you
   * use scalars to create the texture, the scalars are assumed to be
   * monotonically increasing (or decreasing).
   */
	vtkSetClampMacro(GenerateTCoords, int, VTK_TCOORDS_OFF,
		VTK_TCOORDS_FROM_SCALARS);
	vtkGetMacro(GenerateTCoords, int);
	void SetGenerateTCoordsToOff()
    {this->SetGenerateTCoords(VTK_TCOORDS_OFF);}
	void SetGenerateTCoordsToNormalizedLength()
    {this->SetGenerateTCoords(VTK_TCOORDS_FROM_NORMALIZED_LENGTH);}
	void SetGenerateTCoordsToUseLength()
    {this->SetGenerateTCoords(VTK_TCOORDS_FROM_LENGTH);}
	void SetGenerateTCoordsToUseScalars()
    {this->SetGenerateTCoords(VTK_TCOORDS_FROM_SCALARS);}
	const char *GetGenerateTCoordsAsString();
  //@}

  //@{
  /**
   * Control the conversion of units during the texture coordinates
   * calculation. The TextureLength indicates what length (whether
   * calculated from scalars or length) is mapped to the [0,1)
   * texture space.
   */
	vtkSetClampMacro(TextureLength, double, 0.000001, VTK_INT_MAX);
	vtkGetMacro(TextureLength, double);
  //@}

  //@{
  /**
   * Set/get the desired precision for the output types. See the documentation
   * for the vtkAlgorithm::DesiredOutputPrecision enum for an explanation of
   * the available precision settings.
   */
  vtkSetMacro(OutputPointsPrecision,int);
  vtkGetMacro(OutputPointsPrecision,int);
  //@}

protected:
	vtkALBATubeFilter();
	~vtkALBATubeFilter() {}

	// Usual data generation method
	int RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector);

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
  int OutputPointsPrecision;
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
	vtkALBATubeFilter(const vtkALBATubeFilter&);  // Not implemented.
	void operator=(const vtkALBATubeFilter&);  // Not implemented.
};

#endif
