#include "StentModelSource.h"

StentModelSource::StentModelSource(void)
{
	stentDiameter = 2.0;
	crownLength = 2.2;
	linkLength = 0.8;
	strutsNumber = 15;
	crownNumber = 10;
	linkNumber = 3;
	linkConnection = peak2peak;
	stentConfiguration = OutOfPhase;
	linkOrientation = None;
	linkAlignment =3;

	startPosition[0] = 0.0;
	startPosition[1] = 0.0;
	startPosition[2] = 0.0;
	direction[0] = 1.0;
	direction[1] = 0.0;
	direction[2] = 0.0;

	centerLine = NULL;
	crownAdded = false;

	simplexMesh = SimplexMeshType::New();
}

StentModelSource::~StentModelSource(void)
{
	delete [] centerLine;
}

void StentModelSource::setStentDiameter(double value){
	stentDiameter = value;
}
void StentModelSource::setCrownLength(double value){
	crownLength = value;
}
void StentModelSource::setStrutAngle(double value){
	strutAngle = value;
}
void StentModelSource::setLinkLength(double value){
	linkLength = value;
}
void StentModelSource::setStrutsNumber(int value){
	strutsNumber = value;
}
void StentModelSource::setLinkConnection(LinkConnectionType value){
	linkConnection = value;
}
void StentModelSource::setLinkAlignment(unsigned int value){
	linkAlignment = value;
}
void StentModelSource::setStentConfiguration(StentConfigurationType value){
	stentConfiguration = value;
}
void StentModelSource::setLinkOrientation(LinkOrientationType value){
	linkOrientation = value;
}
void StentModelSource::setCrownNumber(int value){
	crownNumber = value;
}
void StentModelSource::setLinkNumber(int value){
	linkNumber = value;
}

void StentModelSource::setStartPosition(double *value){
	startPosition[0] = value[0];
	startPosition[1] = value[1];
	startPosition[2] = value[2];
}
void StentModelSource::setDirection(double *value){
	direction[0] = value[0];
	direction[1] = value[1];
	direction[2] = value[2];
}
void StentModelSource::setCenterLine (double value[][3]){
   //----centreLine上的结点采样交替按照距离crownLength,linLength进行----
}
void StentModelSource::setCenterLinePolydata(vtkPolyData *line){
	m_centerline = line;
	vtkPoints *points = m_centerline->GetPoints();
	vtkIdType num = points->GetNumberOfPoints();
	centerLine = new double [num][3];

	for (int i=0;i<num;i++)
	{
		points->GetPoint(i, centerLine[i]);
	}
}

double StentModelSource::getStrutLength(){
	strutLength = crownLength/cos(strutAngle/2);;
	return strutLength;
}
double StentModelSource::getLinkLength(){
	return linkLength;
}

void StentModelSource::createStent(){
	init();
	createStentSimplexMesh();
	createStruts();
	createLinks();
}

void StentModelSource::init(){
	nCrownSimplex = crownNumber + 2;

	strutAngle = 2*atan(stentDiameter*Pi/strutsNumber/2/crownLength);
	strutLength = crownLength/cos(strutAngle/2);
	
	//centreLine上的结点采样交替按照距离crownLength,distanceBetweenCrown进行
	double distanceBetweenCrown = linkLength;
	double horizontalLength = 0;
	if(linkOrientation == None){
		if(linkConnection == peak2peak)
			distanceBetweenCrown = linkLength;
		else if (linkConnection == valley2valley)
			distanceBetweenCrown = linkLength-2*crownLength;
		else
			distanceBetweenCrown = linkLength-crownLength;
	}else{
		if(linkConnection == peak2peak){
			horizontalLength = sqrt(linkLength*linkLength
			                    - (crownLength*tan(strutAngle/2)*2)*(crownLength*tan(strutAngle/2)*2));
			distanceBetweenCrown = horizontalLength;
		}
		else if (linkConnection == valley2valley){
			horizontalLength = sqrt(linkLength*linkLength
			                    - (crownLength*tan(strutAngle/2)*2)*(crownLength*tan(strutAngle/2)*2));
			distanceBetweenCrown = horizontalLength-2*crownLength;
		}
		else {
			horizontalLength = sqrt(linkLength*linkLength
			                    - (crownLength*tan(strutAngle/2))*(crownLength*tan(strutAngle/2)));
			distanceBetweenCrown = horizontalLength-crownLength;
		}
	}

	double magnitude = sqrt(direction[0]*direction[0]
	                      + direction[1]*direction[1]
						  + direction[2]*direction[2]);
    direction[0] /= magnitude;
	direction[1] /= magnitude;
	direction[2] /= magnitude;
	nCenterVertex = 2*nCrownSimplex;
//temp remove
	if(centerLine == NULL){
		double position = -1*(crownLength + distanceBetweenCrown);
		centerLine = new double [nCenterVertex][3];
		for(int i=0;i<nCenterVertex;i++){
			centerLine[i][0] = startPosition[0] + position*direction[0];
			centerLine[i][1] = startPosition[1] + position*direction[1];
			centerLine[i][2] = startPosition[2] + position*direction[2];
			if(i%2==0) 
				position += crownLength;
			else
				position += distanceBetweenCrown;

		}
	


	}	
//temp remove over
}

void StentModelSource::createStentSimplexMesh(){
    
	//----------------------------------------------------
	//-----sample vertices of simplex mesh for stent--------
	//------------------------------------------------------
	int i,j;
	double normalCircle[3];	
	double theta = Pi/strutsNumber;
	
	//第一圈和最后一圈按照strutsNumber采样，其他两倍采样
	int sampleNumberPerCircle = 2*strutsNumber;
	
	//存储至数组时，先存第一圈和最后一圈，之后依次存储其他圈，即最后一圈index，从strutsNumber至2*strutsNumber-1
	int sampleArraySize = sampleNumberPerCircle * (nCenterVertex-1); 
	double (*sampleArray)[3] = new double[sampleArraySize][3];

	for(i=0;i<nCenterVertex;i=i+2){
		normalCircle[0] = centerLine[i+1][0] - centerLine[i][0];
		normalCircle[1] = centerLine[i+1][1] - centerLine[i][1];
		normalCircle[2] = centerLine[i+1][2] - centerLine[i][2];
		
		double UVector[3], VVector[3],magnitude;
		//normal与（0,1，0）的叉乘
		/*UVector[0] = -1.0*normalCircle[2];;
		UVector[1] = 0;
		UVector[2] = normalCircle[0];*/
		UVector[0] = 0;
		UVector[1] = normalCircle[2];
		UVector[2] = -normalCircle[1];

		if((fabs(UVector[0])+ fabs(UVector[1]) + fabs(UVector[2])) < 0.05){
			UVector[0] = normalCircle[1];
			UVector[1] = -normalCircle[0];
			UVector[2] = 0;
		} 
		magnitude = sqrt(UVector[0]*UVector[0]+UVector[1]*UVector[1] + UVector[2]*UVector[2]);
		UVector[0] *= (stentDiameter/2.0/magnitude);
		UVector[1] *= (stentDiameter/2.0/magnitude);
		UVector[2] *= (stentDiameter/2.0/magnitude);
		//normal与UVector的叉乘
		VVector[0] = normalCircle[1]*UVector[2]-normalCircle[2]*UVector[1];
		VVector[1] = normalCircle[2]*UVector[0]-normalCircle[0]*UVector[2];
		VVector[2] = normalCircle[0]*UVector[1]-normalCircle[1]*UVector[1];
		magnitude =sqrt(VVector[0]*VVector[0] + VVector[1]*VVector[1] + VVector[2]*VVector[2]);
		VVector[0] *= (stentDiameter/2.0/magnitude);
		VVector[1] *= (stentDiameter/2.0/magnitude);
		VVector[2] *= (stentDiameter/2.0/magnitude);

		if(i==0){
			for(int n=0;n<strutsNumber;n++){//第0圈
				sampleArray[n][0] = centerLine[i][0] + cos(theta*n*2)*UVector[0] + sin(theta*n*2)*VVector[0];
				sampleArray[n][1] = centerLine[i][1] + cos(theta*n*2)*UVector[1] + sin(theta*n*2)*VVector[1];
				sampleArray[n][2] = centerLine[i][2] + cos(theta*n*2)*UVector[2] + sin(theta*n*2)*VVector[2];
			}
			for(int n=0;n<sampleNumberPerCircle;n++){//第一圈
				sampleArray[sampleNumberPerCircle+n][0] = centerLine[i+1][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
				sampleArray[sampleNumberPerCircle+n][1] = centerLine[i+1][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
				sampleArray[sampleNumberPerCircle+n][2] = centerLine[i+1][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];
			}
			continue;
		}
		if(i == nCenterVertex-2){
			for(int n = 0;n<sampleNumberPerCircle;n++){//倒数第二圈
				sampleArray[i*sampleNumberPerCircle+n][0] = centerLine[i][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
				sampleArray[i*sampleNumberPerCircle+n][1] = centerLine[i][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
				sampleArray[i*sampleNumberPerCircle+n][2] = centerLine[i][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];
			}
			for(int n=0;n<strutsNumber;n++){//最后一圈
				sampleArray[strutsNumber+n][0] = centerLine[i+1][0] + cos(theta*n*2)*UVector[0] + sin(theta*n*2)*VVector[0];
				sampleArray[strutsNumber+n][1] = centerLine[i+1][1] + cos(theta*n*2)*UVector[1] + sin(theta*n*2)*VVector[1];
				sampleArray[strutsNumber+n][2] = centerLine[i+1][2] + cos(theta*n*2)*UVector[2] + sin(theta*n*2)*VVector[2];
			}
			continue;
		}

		for(int n = 0;n<sampleNumberPerCircle;n++){
			sampleArray[i*sampleNumberPerCircle+n][0] = centerLine[i][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
			sampleArray[i*sampleNumberPerCircle+n][1] = centerLine[i][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
			sampleArray[i*sampleNumberPerCircle+n][2] = centerLine[i][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];
			sampleArray[(i+1)*sampleNumberPerCircle+n][0] = centerLine[i+1][0] + cos(theta*n)*UVector[0] + sin(theta*n)*VVector[0];
			sampleArray[(i+1)*sampleNumberPerCircle+n][1] = centerLine[i+1][1] + cos(theta*n)*UVector[1] + sin(theta*n)*VVector[1];
			sampleArray[(i+1)*sampleNumberPerCircle+n][2] = centerLine[i+1][2] + cos(theta*n)*UVector[2] + sin(theta*n)*VVector[2];

		}
	}

//--------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------
	// Add our vertices to the mesh.
	PointType point;
	//Point3D curPoint;
	for(i=0; i < sampleArraySize ; ++i)
    {
		point[0] = sampleArray[i][0];
		point[1] = sampleArray[i][1];
		point[2] = sampleArray[i][2];
		simplexMesh->SetPoint(i, point);
		simplexMesh->SetGeometryData(i, new SimplexMeshGeometryType );

		/*curPoint.x = sampleArray[i][0];
		curPoint.y = sampleArray[i][1];
		curPoint.z = sampleArray[i][2];
		pointList.push_back(curPoint);*/
    }
	delete[] sampleArray;
	
	// Specify the method used for allocating cells
   simplexMesh->SetCellsAllocationMethod( SimplexMeshType::CellsAllocatedDynamicallyCellByCell );
   
   // AddEdge,AddNeighbor,and add the symmetric relationships
   //-----------------------------------------------------------------------------------
   //--注意neighbor的添加顺序，保证三个相连点以逆时针排列，比保证计算出的法向量朝向向外---
   //----------------------------------------------------------------------------------

   //-----------------
	//AddEdge
	//第0圈 vertical
   for(j=0;j<strutsNumber-1;j++){
		simplexMesh->AddEdge(j,j+1);
	}
	simplexMesh->AddEdge(j,0);
	//第0圈 horizontal
   for(j=0;j<strutsNumber;j++){
		   simplexMesh->AddEdge(j,sampleNumberPerCircle+j*2);
   }
	//中间 vertical
   for(i=1;i<nCenterVertex-1;i++){
	  for(j=0;j<sampleNumberPerCircle-1;j++){
		  simplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
	  }
	  simplexMesh->AddEdge(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
   }
   //中间 horizontal
   for(i=1;i<nCenterVertex-1;i=i+2){
	   for(j=1;j<sampleNumberPerCircle;j=j+2){
		   simplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
	   }
   }
   for(i=2;i<nCenterVertex-2;i=i+2){
	   for(j=0;j<sampleNumberPerCircle;j=j+2){
		   simplexMesh->AddEdge(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
	   }
   }
	//最后一圈 vertical
	for(j=strutsNumber;j<2*strutsNumber-1;j++){
		simplexMesh->AddEdge(j,j+1);
	}
	simplexMesh->AddEdge(j,strutsNumber);
   
   //最后 horizontal
   i=nCenterVertex-2;
   for(j=0;j<strutsNumber;j++){
		simplexMesh->AddEdge(i*sampleNumberPerCircle+j*2,strutsNumber+j);
	}

//-------------------------------------------------------------------
   //AddNeighbor
   //第0圈
   for(j=1;j<strutsNumber-1;j++){
	   simplexMesh->AddNeighbor(j,j-1);
	   simplexMesh->AddNeighbor(j,j+1);
	   simplexMesh->AddNeighbor(j,sampleNumberPerCircle+j*2);
   }
   simplexMesh->AddNeighbor(0,strutsNumber-1);
   simplexMesh->AddNeighbor(0,1);
   simplexMesh->AddNeighbor(0,sampleNumberPerCircle);
   simplexMesh->AddNeighbor(strutsNumber-1,strutsNumber-2);
   simplexMesh->AddNeighbor(strutsNumber-1,0);
   simplexMesh->AddNeighbor(strutsNumber-1,sampleNumberPerCircle+(strutsNumber-1)*2);
   //第一圈
   i=1;  
   for(j=1;j<sampleNumberPerCircle-2;j=j+2){
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
   }
   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
   
   for(j=2;j<sampleNumberPerCircle-1;j=j+2){
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,j/2);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
   }
   simplexMesh->AddNeighbor(i*sampleNumberPerCircle, 0);
   simplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
   simplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
   //simplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+j-1);
   //中间
   for(i=2;i<nCenterVertex-2;i=i+2){
     for(j=2;j<sampleNumberPerCircle-1;j=j+2){
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
	}
	simplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
    simplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
    simplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle);
	for(j=1;j<sampleNumberPerCircle-2;j=j+2){
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
	}
    simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
	simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    
   }
   for(i=3;i<nCenterVertex-2;i=i+2){
	   for(j=1;j<sampleNumberPerCircle-2;j=j+2){
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
	   } 
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
       simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i+1)*sampleNumberPerCircle+j);
	   for(j=2;j<sampleNumberPerCircle-1;j=j+2){
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);		
	   }
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i-1)*sampleNumberPerCircle);
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
		simplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
   }
   //倒数第二圈
   i= nCenterVertex-2;
   for(j=1;j<sampleNumberPerCircle-2;j=j+2){
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
	}
    simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,(i-1)*sampleNumberPerCircle+j);
	simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle);
    simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
    for(j=2;j<sampleNumberPerCircle-1;j=j+2){
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j-1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,i*sampleNumberPerCircle+j+1);
	   simplexMesh->AddNeighbor(i*sampleNumberPerCircle+j,strutsNumber+j/2);
	}
	simplexMesh->AddNeighbor(i*sampleNumberPerCircle,(i+1)*sampleNumberPerCircle-1);
    simplexMesh->AddNeighbor(i*sampleNumberPerCircle,i*sampleNumberPerCircle+1);
    simplexMesh->AddNeighbor(i*sampleNumberPerCircle,strutsNumber);
	//最后一圈
	for(j=1;j<strutsNumber-1;j++){
		simplexMesh->AddNeighbor(strutsNumber+j,i*sampleNumberPerCircle+j*2);
		simplexMesh->AddNeighbor(strutsNumber+j,strutsNumber+j+1);
		simplexMesh->AddNeighbor(strutsNumber+j,strutsNumber+j-1);
	}
	simplexMesh->AddNeighbor(strutsNumber+j,i*sampleNumberPerCircle+j*2);
	simplexMesh->AddNeighbor(strutsNumber+j,strutsNumber);
	simplexMesh->AddNeighbor(strutsNumber+j,strutsNumber+j-1);
	simplexMesh->AddNeighbor(strutsNumber,i*sampleNumberPerCircle);
	simplexMesh->AddNeighbor(strutsNumber,strutsNumber+1);
	simplexMesh->AddNeighbor(strutsNumber,sampleNumberPerCircle-1);

//------------------------------------
   //AddFace
  OutputCellAutoPointer m_NewSimplexCellPointer;
  //第0圈cells
  for(j=0;j<strutsNumber-1;j++){
	m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
	m_NewSimplexCellPointer->SetPointId(0, j);
	m_NewSimplexCellPointer->SetPointId(1, j+1);
	m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle+j*2+2);
	m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
	m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
	simplexMesh->AddFace(m_NewSimplexCellPointer);		
  }
   m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
   m_NewSimplexCellPointer->SetPointId(0, j);
   m_NewSimplexCellPointer->SetPointId(1, 0);
   m_NewSimplexCellPointer->SetPointId(2, sampleNumberPerCircle);
   m_NewSimplexCellPointer->SetPointId(3, sampleNumberPerCircle+j*2+1);
   m_NewSimplexCellPointer->SetPointId(4, sampleNumberPerCircle+j*2);
   simplexMesh->AddFace(m_NewSimplexCellPointer);
   //最后一圈cells
   i=nCenterVertex-2;
	for(j=0;j<strutsNumber-1;j++){
		   m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
		   m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j*2);
		   m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j*2+1);
		   m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j*2+2);
		   m_NewSimplexCellPointer->SetPointId(3, strutsNumber+j+1);
		   m_NewSimplexCellPointer->SetPointId(4, strutsNumber+j);
		   simplexMesh->AddFace(m_NewSimplexCellPointer);		
	   }
	   m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
	   m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j*2);
	   m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j*2+1);
	   m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle);
	   m_NewSimplexCellPointer->SetPointId(3, strutsNumber);
	   m_NewSimplexCellPointer->SetPointId(4, strutsNumber+j);
	   simplexMesh->AddFace(m_NewSimplexCellPointer);

//中间各圈cells
  for(i=2;i<nCenterVertex-2;i=i+2){
	   for(j=0;j<sampleNumberPerCircle-2;j=j+2){
		   m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
		   m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
		   m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
		   m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j+2);
		   m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+j+2);
		   m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
		   m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
		   simplexMesh->AddFace(m_NewSimplexCellPointer);		
	   }
	   m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
	   m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
	   m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
	   m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle);
	   m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle);
	   m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
	   m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
	   simplexMesh->AddFace(m_NewSimplexCellPointer);
   }
  //连接struts与struts
   for(i=1;i<nCenterVertex-1;i=i+2){
	   for(j=1;j<sampleNumberPerCircle-2;j=j+2){
		   m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
		   m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
		   m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle+j+1);
		   m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+j+2);
		   m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+j+2);
		   m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle+j+1);
		   m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
		   simplexMesh->AddFace(m_NewSimplexCellPointer);		
	   }
	    m_NewSimplexCellPointer.TakeOwnership(new OutputPolygonType);
		m_NewSimplexCellPointer->SetPointId(0, i*sampleNumberPerCircle+j);
		m_NewSimplexCellPointer->SetPointId(1, i*sampleNumberPerCircle);
		m_NewSimplexCellPointer->SetPointId(2, i*sampleNumberPerCircle+1);
		m_NewSimplexCellPointer->SetPointId(3, (i+1)*sampleNumberPerCircle+1);
		m_NewSimplexCellPointer->SetPointId(4, (i+1)*sampleNumberPerCircle);
		m_NewSimplexCellPointer->SetPointId(5, (i+1)*sampleNumberPerCircle+j);
		simplexMesh->AddFace(m_NewSimplexCellPointer);	
   }
 // simplexMesh->BuildCellLinks();
}




void StentModelSource::createStruts(){
	Strut theStrut;
	int i,j;
	int sampleNumberPerCircle = 2*strutsNumber;
   
	//偶数圈，从0开始数	
	for(j=2;j<nCenterVertex-2;j=j+4){
		for(i=0;i<sampleNumberPerCircle-2;i=i+2){
			theStrut.startVertex = sampleNumberPerCircle*j+i;
			theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
			strutsList.push_back(theStrut);
			theStrut.startVertex = sampleNumberPerCircle*j+i+2;
			theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
			strutsList.push_back(theStrut);		
		}
		theStrut.startVertex = sampleNumberPerCircle*j+i;
		theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
		strutsList.push_back(theStrut);
		theStrut.startVertex = sampleNumberPerCircle*j;
		theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
		strutsList.push_back(theStrut);
	}
		
	//奇数圈,从0开始数
	if(stentConfiguration == OutOfPhase){
		for(j=4;j<nCenterVertex-2;j=j+4){
			for(i=1;i<sampleNumberPerCircle-2;i=i+2){
				theStrut.startVertex = sampleNumberPerCircle*j+i;
				theStrut.endVertex = sampleNumberPerCircle*(j+1)+i-1;
				strutsList.push_back(theStrut);
				theStrut.startVertex = sampleNumberPerCircle*j+i;
				theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
				strutsList.push_back(theStrut);		
			}
			theStrut.startVertex = sampleNumberPerCircle*j+i;
			theStrut.endVertex = sampleNumberPerCircle*(j+1)+i-1;
			strutsList.push_back(theStrut);
			theStrut.startVertex = sampleNumberPerCircle*j+i;
			theStrut.endVertex = sampleNumberPerCircle*(j+1);
			strutsList.push_back(theStrut);
		}
	} else if (stentConfiguration == InPhase){
		for(j=4;j<nCenterVertex-2;j=j+4){
			for(i=0;i<sampleNumberPerCircle-2;i=i+2){
				theStrut.startVertex = sampleNumberPerCircle*j+i;
				theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
				strutsList.push_back(theStrut);
				theStrut.startVertex = sampleNumberPerCircle*j+i+2;
				theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
				strutsList.push_back(theStrut);		
			}
			theStrut.startVertex = sampleNumberPerCircle*j+i;
			theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
			strutsList.push_back(theStrut);
			theStrut.startVertex = sampleNumberPerCircle*j;
			theStrut.endVertex = sampleNumberPerCircle*(j+1)+i+1;
			strutsList.push_back(theStrut);	
		}
	}
}

/*-------------
different link types: 12 kinds + different alignment
6 kinds for out of phase:p2p+N; v2v+N; p2v+1;p2v-1;v2p+1;v2p-1
6 kinds for in phase:p2v+N; v2P+N; p2p+1;p2p-1;v2v+1;v2v-1
-------------------*/
void StentModelSource::createLinks(){
	Strut theLink;
	int i,j;
	int sampleNumberPerCircle = 2*strutsNumber;
	int interval = (strutsNumber / linkNumber)*2;
	int indexFirstLink=0;
	if(crownAdded) crownNumber--;
	int indexCurrent;

	if(stentConfiguration == OutOfPhase){
		
		if(linkConnection == peak2peak && linkOrientation == None){// out of phase + p2p + N
			
			indexFirstLink = 1;
			for(j=1;j<nCrownSimplex-2;j++){
				for(i=0;i<linkNumber;i=i++){
					indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
					theLink.startVertex = sampleNumberPerCircle * (j*2+1) + indexCurrent;
					theLink.endVertex = sampleNumberPerCircle *(j*2+2) + indexCurrent;
					linkList.push_back(theLink);
				}
				indexFirstLink = (indexFirstLink + (linkAlignment*2-1))%sampleNumberPerCircle;
			}

		}else if(linkConnection == valley2valley && linkOrientation == None){//out of phase + v2v + N

			indexFirstLink = 0;

			for(j=1;j<nCrownSimplex-2;j++){
				for(i=0;i<linkNumber;i++){
					indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
					theLink.startVertex = sampleNumberPerCircle * (j*2) + indexCurrent;
					theLink.endVertex = sampleNumberPerCircle *(j*2+3) + indexCurrent;
					linkList.push_back(theLink);
				}
				indexFirstLink = (indexFirstLink + (linkAlignment*2-1))%sampleNumberPerCircle;
			}

		
		}else if(linkConnection == peak2valley && linkOrientation != None){ //out of phase, p2v, +/-1
			
			int offset;
			if(linkOrientation == PositiveOne) offset = 1;
			if(linkOrientation == NegativeOne) offset = -1;
			
			indexFirstLink = 1;

			int indexCurrent2;
			for(j=1;j<nCrownSimplex-2;j++){
				for(i=0;i<linkNumber;i++){
					indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
					indexCurrent2 = (indexCurrent + offset + sampleNumberPerCircle)%sampleNumberPerCircle;
					theLink.startVertex = sampleNumberPerCircle * (j*2+1) + indexCurrent;
					theLink.endVertex = sampleNumberPerCircle *(j*2+3) + indexCurrent2;
					linkList.push_back(theLink);
				}
				indexFirstLink = (indexFirstLink + (linkAlignment*2-1))%sampleNumberPerCircle;
			}

		}else if(linkConnection == valley2peak && linkOrientation != None){
			int offset;
			if(linkOrientation == PositiveOne) offset = 1;
			if(linkOrientation == NegativeOne) offset = -1;
			
			indexFirstLink = 0;
			int indexCurrent2;
			for(j=1;j<nCrownSimplex-2;j++){
				for(i=0;i<linkNumber;i++){
					indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
					indexCurrent2 = (indexCurrent + offset + sampleNumberPerCircle)%sampleNumberPerCircle;
					theLink.startVertex = sampleNumberPerCircle * (j*2) + indexCurrent;
					theLink.endVertex = sampleNumberPerCircle*(j*2+2) + indexCurrent2;
					linkList.push_back(theLink);
				}
				indexFirstLink = (indexFirstLink + (linkAlignment*2-1))%sampleNumberPerCircle;
			}
		
		}else{
			std::cout << "please check link settings"<<std::endl;
		}
	
	}else{ //in phase
		if(linkConnection == peak2valley && linkOrientation == None){
				
			indexFirstLink = 1;
			for(j=1;j<nCrownSimplex-2;j++){
				for(i=0;i<linkNumber;i++){
					indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
					theLink.startVertex = sampleNumberPerCircle*(j*2+1) + indexCurrent;
					theLink.endVertex = sampleNumberPerCircle*(j*2+3) + indexCurrent;
					linkList.push_back(theLink);
				}
				indexFirstLink = (indexFirstLink + (linkAlignment*2))%sampleNumberPerCircle;
			}
	
		}else if(linkConnection == valley2peak && linkOrientation == None){
			
			indexFirstLink = 0;
			for(j=1;j<nCrownSimplex-2;j++){
				for(i=0;i<linkNumber;i++){
					indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
					theLink.startVertex = sampleNumberPerCircle*(j*2) + indexCurrent;
					theLink.endVertex = sampleNumberPerCircle*(j*2+2) + indexCurrent;
					linkList.push_back(theLink);
				}
				indexFirstLink = (indexFirstLink + (linkAlignment*2))%sampleNumberPerCircle;
			}
		
		}else if(linkConnection == peak2peak && linkOrientation != None){
			int offset;
			if(linkOrientation == PositiveOne) offset = 1;
			if(linkOrientation == NegativeOne) offset = -1;

			indexFirstLink = 1;
			int indexCurrent2;
			for(j=1;j<nCrownSimplex-2;j++){
				for(i=0;i<linkNumber;i++){
					indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
					indexCurrent2 = (indexCurrent + offset + sampleNumberPerCircle)%sampleNumberPerCircle;
					theLink.startVertex = sampleNumberPerCircle * (j*2+1) + indexCurrent;
					theLink.endVertex = sampleNumberPerCircle*(j*2+2) + indexCurrent2;
					linkList.push_back(theLink);
				}
				indexFirstLink = (indexFirstLink + (linkAlignment*2))%sampleNumberPerCircle;
			}
		
		}else if(linkConnection == valley2valley && linkOrientation != None){
			int offset;
			if(linkOrientation == PositiveOne) offset = 1;
			if(linkOrientation == NegativeOne) offset = -1;

			indexFirstLink = 0;
			int indexCurrent2;
	
			for(j=1;j<nCrownSimplex-2;j++){
				for(i=0;i<linkNumber;i++){
					indexCurrent = (indexFirstLink + i*interval)%sampleNumberPerCircle;
					indexCurrent2 = (indexCurrent + offset + sampleNumberPerCircle)%sampleNumberPerCircle;
					theLink.startVertex = sampleNumberPerCircle*(j*2) + indexCurrent;
					theLink.endVertex = sampleNumberPerCircle*(j*2+3) + indexCurrent2;
					linkList.push_back(theLink);
				}
				indexFirstLink = (indexFirstLink + (linkAlignment*2))%sampleNumberPerCircle;
			}
		
		}else{
			std::cout << "please check link settings"<<std::endl;
		}	
	}
}