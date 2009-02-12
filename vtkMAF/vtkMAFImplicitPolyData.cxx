/*=========================================================================
ToDo:
-	Needs to be restructured in line with vtk principles of on demand
    execution. Eg BuildLinks and BuildLocator on first call to Evaluate*?.
    Handle input modification correctly etc.
-   Drop internal triangle filter and just check cell type as encountered?.
-   Get working with CellLocator. Currently crashes every time, although
    a large number of cells are located successfully. Too hard to debug,
    maybe try a tiny dataset.
=========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkMAFImplicitPolyData.h"
#include "vtkPolygon.h"

vtkCxxRevisionMacro(vtkMAFImplicitPolyData, "$Revision: 1.1.2.1 $");
vtkStandardNewMacro(vtkMAFImplicitPolyData);

// Constructor
//----------------------------------------------------------------------------
vtkMAFImplicitPolyData::vtkMAFImplicitPolyData()
//----------------------------------------------------------------------------
{
	this->NoGradient[0] = 0.0;
	this->NoGradient[1] = 0.0;
	this->NoGradient[2] = 1.0;

	this->Tri     = NULL;
	this->Input   = NULL;
  this->Locator = NULL;
  this->Poly    = NULL;
  this->Cells   = NULL;

  this->EvaluateBoundsSet=0;
}
//----------------------------------------------------------------------------
void vtkMAFImplicitPolyData::SetInput(vtkPolyData *input) 
//----------------------------------------------------------------------------
{
  if( this->Input != input ) 
  {
    vtkDebugMacro( <<" setting Input to " << (void *)input );

	  // use a tringle filter on the polydata input
    // this is done to filter out lines and vertices to leave only
    // polygons which are required by this algorithm for cell normals
    if( this->Tri == NULL ) 
    {
		  this->Tri = vtkTriangleFilter::New();
		  this->Tri->PassVertsOff();
		  this->Tri->PassLinesOff();
    }
	  this->Tri->SetInput( input );
    this->Tri->Update();

    this->Input = this->Tri->GetOutput();
    this->Input->BuildLinks();	// to enable calls to GetPointCells
	  this->NoValue = this->Input->GetLength();

    if( this->Locator != NULL ) this->Locator->Delete();
    Locator = PointLocator::New();
    this->Locator->SetDataSet( this->Input );
    this->Locator->BuildLocator();

    if( this->Cells   != NULL ) this->Cells->Delete();
	  this->Cells = vtkIdList::New();
    this->Cells->SetNumberOfIds( this->Input->GetNumberOfCells() );

    this->Poly = vtkPolygon::New();
    this->Modified();
  }
}
// used to extend bounds of point locator
//----------------------------------------------------------------------------
void vtkMAFImplicitPolyData::SetEvaluateBounds( double eBounds[6] )
//----------------------------------------------------------------------------
{
  int i;
  for( i=0; i<6; i++ ) 
    this->EvaluateBounds[i] = eBounds[i];
  this->EvaluateBoundsSet = 1;
}
//----------------------------------------------------------------------------
unsigned long vtkMAFImplicitPolyData::GetMTime()
//----------------------------------------------------------------------------
{
  unsigned long mTime = this->vtkImplicitFunction::GetMTime();
  unsigned long inputMTime;

  if ( this->Input != NULL )
  {
    this->Input->Update ();
    inputMTime = this->Input->GetMTime();
    mTime = ( inputMTime > mTime ? inputMTime : mTime );
  }

  return mTime;
}
// Destructor
//----------------------------------------------------------------------------
vtkMAFImplicitPolyData::~vtkMAFImplicitPolyData()
//----------------------------------------------------------------------------
{
  if( this->Tri     != NULL ) this->Tri->Delete();
  if( this->Locator != NULL ) this->Locator->Delete();
  if( this->Poly    != NULL ) this->Poly->Delete();
  if( this->Cells   != NULL ) this->Cells->Delete();
}
// Evaluate for point x[3].
// Method using combination of implicit planes
//----------------------------------------------------------------------------
double vtkMAFImplicitPolyData::EvaluateFunction(double x[3])
//----------------------------------------------------------------------------
{
	// See if data set with polygons has been specified
	if( this->Input == NULL || Input->GetNumberOfCells() == 0 ) 
  {
	  vtkErrorMacro(<<"No polygons to evaluate function!");
    return this->NoValue;
  }

	int cellNum, pid;
	vtkCell *cell;
	double dot, ret = -VTK_LARGE_FLOAT, cNormal[3], closestPoint[3];

    // get point id of closest point in data set
	pid = this->Locator->FindClosestPoint( x );
	if( pid != -1 ) 
  {
    this->Input->GetPoint( pid, closestPoint );
    // get cells it belongs to
		this->Input->GetPointCells( pid, Cells );
    // for each cell
  	for( cellNum=0; cellNum<Cells->GetNumberOfIds(); cellNum++ ) 
    {
    	cell = this->Input->GetCell( Cells->GetId( cellNum ) );
      // get cell normal
      Poly->ComputeNormal( cell->GetPoints(), cNormal );
      dot = ( cNormal[0]*(x[0]-closestPoint[0]) +
              cNormal[1]*(x[1]-closestPoint[1]) +
              cNormal[2]*(x[2]-closestPoint[2]) );

      if( dot > ret ) ret = dot;
    }
  }
  if( ret == -VTK_LARGE_FLOAT ) ret = NoValue;
  return ret;
}
// Evaluate function gradient at point x[3].
//----------------------------------------------------------------------------
void vtkMAFImplicitPolyData::EvaluateGradient( double x[3], double n[3] )
//----------------------------------------------------------------------------
{
	int i;
	// See if data set with polygons has been specified
	if( this->Input == NULL || Input->GetNumberOfCells() == 0 ) 
  {
	  vtkErrorMacro(<<"No polygons to evaluate gradient!");
    for( i=0; i<3; i++ ) n[i] = this->NoGradient[i];
    return;
  }

	int cellNum, pid;
	vtkCell *cell;
	double dot, ret = -VTK_LARGE_FLOAT, cNormal[3], closestPoint[3];

    // get point id of closest point in data set
	pid = this->Locator->FindClosestPoint( x );
	if( pid != -1 ) 
  {
   	this->Input->GetPoint( pid, closestPoint );
   	// get cells it belongs to
		this->Input->GetPointCells( pid, Cells );
   	// for each cell
 		for( cellNum=0; cellNum<Cells->GetNumberOfIds(); cellNum++ ) 
    {
    	cell = this->Input->GetCell( Cells->GetId( cellNum ) );
      // get cell normal
      Poly->ComputeNormal( cell->GetPoints(), cNormal );
      dot = ( cNormal[0]*(x[0]-closestPoint[0]) +
              cNormal[1]*(x[1]-closestPoint[1]) +
              cNormal[2]*(x[2]-closestPoint[2]) );

      if( dot > ret ) 
        for( i=0; i<3; i++ ) n[i] = cNormal[i];
    }
  }
  if( ret == -VTK_LARGE_FLOAT ) 
    for( i=0; i<3; i++ ) n[i] = this->NoGradient[i];
}
//----------------------------------------------------------------------------
void vtkMAFImplicitPolyData::PrintSelf(ostream& os, vtkIndent indent)
//----------------------------------------------------------------------------
{
  vtkImplicitFunction::PrintSelf(os,indent);

  os << indent << "No polydata Value: " << this->NoValue << "\n";
  os << indent << "No polydata Gradient: (" << this->NoGradient[0] << ", "
     << this->NoGradient[1] << ", " << this->NoGradient[2] << ")\n";

  if ( this->Input )
    os << indent << "Input : " << this->Input << "\n";
  else
    os << indent << "Input : (none)\n";
}
