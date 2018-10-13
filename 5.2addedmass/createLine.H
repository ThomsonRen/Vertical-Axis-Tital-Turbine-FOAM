// creat the actruator point(by given the location of each point)
// find the mindistance of the ALM points to cell center, and record its corresponding minDis cellID.
// user can assgin the number of ALM points in the disk dictionary.
//here we assume that that rotating deriction is counter-clockwise

//1.variable declaration
scalar TIME = runTime.value();
scalar deltaT = runTime.deltaT().value();
//int AA = point_of_each_line;
List<List<vector> >LinePointTable(bladeNum,List<vector>(point_of_each_line,vector::zero));
List<vector> ShaftPoint(point_of_each_line,vector::zero);
scalar phaseUnit = Foam::constant::mathematical::twoPi/bladeNum;
scalar lineUnit = height/point_of_each_line;
scalar phase1 = initialPhase + Foam::constant::mathematical::twoPi*TIME/T2;   // phase of the first blade
scalarField phase(bladeNum,0.0); // phases of all the blades
int total_point_num = point_of_each_line * bladeNum;
List<scalar> mindisLocal(total_point_num,0.0);
List<scalar> mindisGlobal(total_point_num,0.0);
List<scalar> mindisCell(total_point_num,0.0);



//2.calculate phase of each blade, the first one is phase1
forAll(phase,i)
	{
		phase[i] = phase1 + i*phaseUnit;
	}

//3.calculate the location of all points on the actruator line as well as the shaft
for(int i = 0;i < bladeNum;i++)
{
	for(int j = 0;j< point_of_each_line;j++)
	{
		LinePointTable[i][j].x() = turbineCenter_x + 0.5*bladeDiameter*Foam::cos(phase[i]);
		LinePointTable[i][j].y() = turbineCenter_y + 0.5*bladeDiameter*Foam::sin(phase[i]);
		LinePointTable[i][j].z() = turbineCenter_z - 0.5*height + j*lineUnit;
		//Info<<"LinePointTable["<<i+1<<"]["<<j+1<<"] = "<< LinePointTable[i][j]<<endl;
	}
} 


for(int j = 0;j <point_of_each_line;j++ )
{
	ShaftPoint[j].x() = turbineCenter_x;
	ShaftPoint[j].y() = turbineCenter_y;
	ShaftPoint[j].z() = turbineCenter_z - 0.5*height + j*lineUnit;
}



//4.Find the nearest cell number of each point and record its min distance(Globally)
int iter = 0;
for(int i=0;i<bladeNum;i++)
{
	for(int j=0;j<point_of_each_line;j++)
	{
	scalar minDistance = 1e8;
	scalar cellID = -1;
	scalar tempdistance;
	forAll(sphereCellsI,cellI)
		{
			const vector& cellCenter = mesh.C()[cellI];
			tempdistance = pow((cellCenter[0] - LinePointTable[i][j].x()),2)+pow((cellCenter[1] - LinePointTable[i][j].y()),2)+pow((cellCenter[2] - LinePointTable[i][j].z()),2);
			if (tempdistance < minDistance)
			{
				minDistance = tempdistance;
				cellID = cellI;
			}
		}
		mindisLocal[iter] = minDistance;
		mindisGlobal[iter] = minDistance;
		mindisCell[iter] = cellID;
		iter++;
		//Info<<"nearestCell["<<i+1<<"]["<<j+1<<"] = "<< nearestCell[i][j]<<endl;
		//Info<<"nearestdistance["<<i+1<<"]["<<j+1<<"] = "<< nearestdistance[i][j]<<endl;
	}
}
// put the result of all the processor together and find the Global minDis
Pstream::gather(mindisGlobal,minOp<List<scalar> >());
Pstream::scatter(mindisGlobal);

for(int i=0;i<total_point_num;i++)
{
//Info<<"mindisGlobal["<<i+1<<"] = "<< mindisGlobal[i]<<endl;
}


// After change the mindisGloal, change its cellID as well
iter = 0;
for(int i=0;i<bladeNum;i++)
{
	for(int j=0;j<point_of_each_line;j++)
	{
	if(mindisGlobal[iter] != mindisLocal[iter])	
		{
		mindisCell[iter] = -1;
		}
	iter++;
	}
}

