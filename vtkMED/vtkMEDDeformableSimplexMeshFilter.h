/*-------------------------------
* By Xiangyin Ma, Nov 2012
*-----------------------------------
*The deforming filter of simplex mesh
*1. inherite from itk::DeformableSimplexMesh3DFilter
*2. Keep the internal force/smoothing force unchanged
*3. Override External force, compute it as displacement vectors along the direction normal to the simplex surface
*4. Add new force called 'Strut and link length force', to keep the length of the struts and links
*5. Override the ComputeDisplacement() function
*   5.1. Use KD tree to calculate the distance from the simplex vertex to the vessel wall
*        This distance is used to weight the external force and total dispalcement
*   5.2. Sysnchronize the catheter's pulling over with the expansion of a certain circle of simplex vertices
*--------------------------------*/

#include "itkDeformableSimplexMesh3DFilter.h"
#include "itkMesh.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkCovariantVector.h"
#include "vtkMEDStentModelSource.h"
//#include "VesselMeshForTesting.h" 
#include "kdtree.h"
#include <assert.h>
#include "vtkPolyData.h"


#include <set>
#include <vector>
using std::vector;

namespace itk
{
template< class TInputMesh, class TOutputMesh >
class DeformableSimplexMeshFilterImpl :
  public DeformableSimplexMesh3DFilter< TInputMesh, TOutputMesh >
{
public:
  /** Standard "Self" typedefs. */
  typedef DeformableSimplexMeshFilterImpl Self;

  /** Standard "Superclass" typedef. */
  typedef  DeformableSimplexMesh3DFilter< TInputMesh, TOutputMesh > Superclass;

  /** Smart pointer typedef support */
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DeformableSimplexMeshFilterImpl, DeformableSimplexMesh3DFilter);

  /** Some typedefs. */
  typedef TInputMesh  InputMeshType;
  typedef TOutputMesh OutputMeshType;

  typedef typename Superclass::PointType              PointType;
  typedef typename Superclass::GradientIndexType      GradientIndexType;
  typedef typename Superclass::GradientIndexValueType GradientIndexValueType;
  typedef typename Superclass::GradientType           GradientType;
  typedef typename Superclass::GradientImageType      GradientImageType;

  /** Mesh pointer definition. */
  typedef typename InputMeshType::Pointer  InputMeshPointer;
  typedef typename OutputMeshType::Pointer OutputMeshPointer;

  typedef typename InputMeshType::PixelType PixelType;

  typedef Image< PixelType, 3 >                        GradientIntensityImageType;
  typedef typename GradientIntensityImageType::Pointer GradientIntensityImagePointer;

  typedef Image< float, 3 >                               OriginalImageType;
  typedef typename OriginalImageType::IndexType           OriginalImageIndexType;
  typedef typename OriginalImageIndexType::IndexValueType ImageIndexValueType;
  typedef typename OriginalImageType::ConstPointer        OriginalImagePointer;

  typedef vtkMEDStentModelSource::Strut          Strut;
  typedef vector<Strut>::const_iterator    StrutIterator;
  typedef itk::Vector< double, 3 >         VectorType;

  //typedef VesselMeshForTesting::Point      Point;
  //typedef vector<Point>::const_iterator    PointIterator;

  itkSetMacro(StrutLength, double);
  itkGetConstMacro(StrutLength, double);

  itkSetMacro(LinkLength, double);
  itkGetConstMacro(LinkLength, double);

  itkSetMacro(CurIterationNum, int);
  itkGetConstMacro(CurIterationNum, int);

  /*itkSetMacro(m_StrutLength, double);
  itkGetConstMacro(m_StrutLength, double);

  itkSetMacro(m_LinkLength, double);
  itkGetConstMacro(m_LinkLength, double);

  itkSetMacro(m_CurIterationNum, int);
  itkGetConstMacro(m_CurIterationNum, int);
  */
	  void SetStrutLinkIter(StrutIterator StrutStart, StrutIterator StrutEnd,
							StrutIterator LinkStart, StrutIterator LinkEnd);

	  void SetStrutLinkFromCellArray(vtkCellArray *strut,vtkCellArray *link);

	 // void SetVesselPointsKDTree(PointIterator PointStart, PointIterator PointEnd);
	  void SetVesselPointsKDTreeFromPolyData(vtkPolyData *surface);
	  /*to check if point touch vessel wall from inside,1 inside ,0,outside */
	  int isPointInsideVessel(double *stentPoint, double *surfacePoint) ;
	  double computeDirectionFromPoint2Face(double *stentPoint,vtkIdType p1,vtkIdType p2,vtkIdType p3);
	  void SetCenterLocationIdx(vector<int>::const_iterator centerLocationIndex);
	  void SetCenterLocationIdxRef(vector<int> const&ve);
	  void SetTestValue(int value){this->testValue = value;}
	  void vectorSubtract(double *endPoint,double *startPoint,double *result);

protected:
	DeformableSimplexMeshFilterImpl();
	~DeformableSimplexMeshFilterImpl();
	DeformableSimplexMeshFilterImpl(const Self &) {}
	void operator=(const Self &){}

	virtual void ComputeDisplacement();
	virtual void ComputeExternalForce(SimplexMeshGeometry *data);

	double m_StrutLength;
	double m_LinkLength;
 

private:
	int m_CurIterationNum;
	vtkPolyData *m_SurfacePoly;
	VectorType ComputeStrutLengthForce(SimplexMeshGeometry *data, int index);
    VectorType ComputeLinkLengthForce(SimplexMeshGeometry *data, int index);

	double distanceCoefficient;

	// [0],[1] strut neighbor; [2] link neighbor
	int (*StrutNeighbors)[3] ;

	kdtree *KDTree;

	vector<int>::const_iterator centerLocationIdx;
	int isDataChanged ;

	int testValue;

}; // end of class
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "vtkMEDDeformableSimplexMeshFilterImpl.h"
#endif
