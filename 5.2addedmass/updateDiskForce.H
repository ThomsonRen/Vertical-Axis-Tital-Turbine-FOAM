// using interplation to get the velocity of each ALM points
// calculate the body force induced by the ALM points
// project the body force onto cells
// calculat the power and torque

//1.variables declarition
gradU = fvc::grad(U);
List<vector> VectorsLocal(total_point_num,vector::zero);//local speed


List<List<vector> > RelativeU(bladeNum,List<vector>(AA,vector::zero));
List<List<scalar> > attackAng(bladeNum,List<scalar>(AA,0.0));

List<List<scalar> > Cl(bladeNum,List<scalar>(AA,0.0));
List<List<scalar> > Cd(bladeNum,List<scalar>(AA,0.0));
List<List<scalar> > Umag(bladeNum,List<scalar>(AA,0.0));
List<List<scalar> > Re(bladeNum,List<scalar>(AA,0.0));
scalarField alpha(AirfoilTable[0].size(),0.0);
scalarField Cl_old(AirfoilTable[0].size(),0.0);
scalarField Cd_old(AirfoilTable[0].size(),0.0);
List<vector> bladeAlignedCoor1(bladeNum,vector::zero);
List<vector> bladeAlignedCoor2(bladeNum,vector::zero);
scalar projectionRadius = epsilon * Foam::sqrt(Foam::log(1.0/0.001));
//scalar projectionRadius = epsilon * 2;
List<scalar> record_dx(total_point_num,0.0);

//Gasuu projection radius,where the parameter epsilon can be adjusted 

//2.Read the data of Airfoil and calculate the coor system
//for(int i=0; i<AirfoilTable[0].size(); i++)
//{
//	alpha[i] = AirfoilTable[0][i];
//	Cl_old[i] = AirfoilTable[1][i];
//	Cd_old[i] = AirfoilTable[2][i];
//}
alpha = AirfoilTable[0];
Cl_old = AirfoilTable[1];
Cd_old = AirfoilTable[2];
//here we assume that the rotating is counter-clockwise
for(int i=0; i< bladeNum;i++)
	{
	bladeAlignedCoor1[i] = (LinePointTable[i][0] - ShaftPoint[0])^ShaftNaxial;//normally, ShaftNaxial is (0 0 1)
	bladeAlignedCoor2[i] = LinePointTable[i][0] - ShaftPoint[0];
	bladeAlignedCoor2[i] = bladeAlignedCoor2[i]/mag(bladeAlignedCoor2[i]);  //y axis
	bladeAlignedCoor1[i] = bladeAlignedCoor1[i]/mag(bladeAlignedCoor1[i]);  //x axis
	}

//3.calculate the local velocity(if the mindisCell is in this processer)
iter = 0;
for(int i=0;i< bladeNum;i++)
	{
		for(int j = 0;j<point_of_each_line;j++ )
		{
			if (mindisCell[iter] != -1) //do this only if the points are in the processor region
			{
				VectorsLocal[iter] = U[mindisCell[iter]];
				vector singlePointVector = LinePointTable[i][j];
				vector dx = singlePointVector - mesh.C()[mindisCell[iter]];
				record_dx[iter] = mag(dx);
				vector dU = dx & gradU[mindisCell[iter]];
				VectorsLocal[iter] += dU;
			}
		iter++;
		}
	}

Pstream::gather(record_dx,maxOp<List<scalar> >());
Pstream::scatter(record_dx);

Pstream::gather(VectorsLocal,sumOp<List<vector> >());
Pstream::scatter(VectorsLocal);

for(int i=0;i< total_point_num;i++)
{
	//Info<<"VectorsLocal["<<i+1<<"]= "<< VectorsLocal[i] <<endl;
	//Info<<"record_dx["<<i+1<<"]= "<< record_dx[i] <<endl;
}

//Info<<"VectorsLocal["<<1<<"]= "<< VectorsLocal[0] <<endl;
//Info<<"VectorsLocal["<<2<<"]= "<< VectorsLocal[0+point_of_each_line] <<endl;
//Info<<"VectorsLocal["<<3<<"]= "<< VectorsLocal[0+2*point_of_each_line] <<endl;
for(int i=0;i< bladeNum;i++)
{
Info<<"VectorsLocal["<<i+1<<"]= "<< VectorsLocal[0+i*point_of_each_line] <<endl;
}



//now we get all the velocity in the actruator points.

//4. calculate the cl,cd and relativeU,attack angle(by interp)
iter = 0;
for(int i=0;i< bladeNum;i++)
	{
		for(int j = 0;j<point_of_each_line;j++ )
		{
		RelativeU[i][j].x() = (VectorsLocal[iter] & bladeAlignedCoor1[i]) + 0.5*bladeDiameter * Foam::constant::mathematical::twoPi / T2;
		RelativeU[i][j].y() = VectorsLocal[iter] & bladeAlignedCoor2[i];
		//Info<<"RelativeU[i][j].x()= "<< RelativeU[i][j].x()<<endl;
	    	//Info<<"RelativeU[i][j].y()= "<< RelativeU[i][j].y()<<endl;
		RelativeU[i][j].z() = 0.0;
		
		Umag[i][j] = Foam::sqrt(pow(RelativeU[i][j].x(),2)+pow(RelativeU[i][j].y(),2));
		Re[i][j] = Umag[i][j] * chord / nu;
		attackAng[i][j] = Foam::atan2(RelativeU[i][j].y(),RelativeU[i][j].x())*360/Foam::constant::mathematical::twoPi;
		
		//Cl[i][j] = interpolateXY(attackAng[i][j],alpha,Cl_old);
		//Cd[i][j] = interpolateXY(attackAng[i][j],alpha,Cd_old);
		Cl[i][j] = interpCl(Re[i][j],attackAng[i][j],Re_list,attack_list,mutiRe_Cl);
		Cd[i][j] = interpCd(Re[i][j],attackAng[i][j],Re_list,attack_list,mutiRe_Cd);
		if (TIME > deltaT)
		{
		scalar deltaV = RelativeU[i][j].y() - RelativeUtemp[i][j].y();
		scalar deltalpha = (attackAng[i][j] - attackAngtemp[i][j])/360*Foam::constant::mathematical::twoPi;
		//Info<<"deltaV "<< deltaV <<endl;
		//Info<<"deltalpha "<< deltalpha <<endl;
		scalar CnAM = (-1)*0.5*Foam::constant::mathematical::twoPi*chord*(RelativeU[i][j].y() - RelativeUtemp[i][j].y())/deltaT/8/Umag[i][j]/Umag[i][j];
		scalar CcAM = 0.5*Foam::constant::mathematical::twoPi * chord * deltalpha /deltaT*RelativeU[i][j].y()/8/Umag[i][j]/Umag[i][j];
		scalar deltaCl = CnAM * Foam::cos(attackAng[i][j]/360*Foam::constant::mathematical::twoPi) + CcAM * Foam::sin(attackAng[i][j]/360*Foam::constant::mathematical::twoPi);
		scalar deltaCD = CnAM * Foam::sin(attackAng[i][j]/360*Foam::constant::mathematical::twoPi) - CcAM * Foam::cos(attackAng[i][j]/360*Foam::constant::mathematical::twoPi);
		Info<<"deltaCl "<< deltaCl <<endl;
		Info<<"deltaCD "<< deltaCD <<endl;
		Cl[i][j] += deltaCl ;
		Cd[i][j] += deltaCD ;
		}
		RelativeUtemp[i][j] = RelativeU[i][j];
		attackAngtemp[i][j] = attackAng[i][j];
		//Info<<"attackAng["<<i+1<<"]["<<j+1<<"] = "<< attackAng[i][j]<<endl;
		//Info<<"Cl["<<i+1<<"]["<<j+1<<"] = "<< Cl[i][j]<<endl;
		//Info<<"Cd["<<i+1<<"]["<<j+1<<"] = "<< Cd[i][j]<<endl;
		iter++;
		}
	}

for (int i=0;i<bladeNum;i++)
{
	Info<<"attackAng["<<i+1<<"]["<<0<<"] = "<< attackAng[i][0]<<endl;
	Info<<"Re["<<i+1<<"]["<<0<<"] = "<< Re[i][0]<<endl;
}



//forAll(mesh.cells(),cellI)
//	{
//		pointForce[cellI] = vector::zero;
//	}

pointForce *= 0.0;//initialize the pointForce
scalar torque = 0;
scalar power = 0;
scalar drag_on_stream = 0;

//5. Calculate the force on the line and project it according to Gauss projection.
//iter = 0;
for(int i=0;i< bladeNum;i++)
	{
	for(int j = 0; j<point_of_each_line;j++ )
		{
		
		//vector dragForce = 0.5 * Cl[i][j] * Umag[i][j]*Umag[i][j]*chord*lineUnit*bladeAlignedCoor1[i];	
		//vector liftForce = 0.5 * Cl[i][j] * Umag[i][j]*Umag[i][j]*chord*lineUnit*bladeAlignedCoor2[i];
		scalar dragForceMag = 0.5 * rho * Cd[i][j] * Umag[i][j]*Umag[i][j]*chord*lineUnit;
		scalar liftForceMag = 0.5 * rho * Cl[i][j] * Umag[i][j]*Umag[i][j]*chord*lineUnit;
		vector dragVector = RelativeU[i][j].x() * bladeAlignedCoor1[i] + RelativeU[i][j].y() * bladeAlignedCoor2[i];
		//Info<<"liftForce["<<i+1<<"]["<<j+1<<"] = "<< liftForce <<endl;
		vector liftVector = ShaftNaxial ^ dragVector;
		//Info<<"liftForceMag["<<i+1<<"]["<<j+1<<"] = "<< liftForceMag <<endl;
		//Info<<"dragForceMag["<<i+1<<"]["<<j+1<<"] = "<< dragForceMag <<endl;
		dragVector = dragVector/mag(dragVector);
		liftVector = liftVector/mag(liftVector);
		//Info<<"liftVectormag["<<i+1<<"]["<<j+1<<"] = "<< mag(liftVector) <<endl;
		vector dragForce = dragForceMag * dragVector;
		vector liftForce = liftForceMag * liftVector;
		//Info<<"liftForce["<<i+1<<"]["<<j+1<<"] = "<< liftForce <<endl;
		//Info<<"dragForce["<<i+1<<"]["<<j+1<<"] = "<< dragForce <<endl;
		vector TotForce = liftForce + dragForce;
		scalar drag_on_blade = - TotForce & bladeAlignedCoor1[i];
		drag_on_stream += TotForce.x();
		//Info<<"TotForce["<<i+1<<"]["<<j+1<<"] = "<< TotForce <<endl;
		torque += drag_on_blade * 0.5 * bladeDiameter;
		//power += torque *  Foam::constant::mathematical::twoPi / T2;
		//Info<<"power["<<i+1<<"]["<<j+1<<"] = "<< power <<endl;
		//Info<<Re_list.size()<<endl;
		forAll(sphereCellsI,cellI)
			{
				const vector& cellCenter = mesh.C()[cellI];
				scalar weightDis = Foam::pow((Foam::pow((cellCenter[0] - LinePointTable[i][j].x()),2) + Foam::pow((cellCenter[1] - LinePointTable[i][j].y()),2)+ Foam::pow((cellCenter[2] - LinePointTable[i][j].z()),2)),0.5);
				if (Foam::pow(weightDis,2) < Foam::pow(projectionRadius,2))
				      {
					scalar weight =  Foam::exp(-Foam::sqr(weightDis/epsilon))/(Foam::pow(epsilon,3)*Foam::pow(Foam::constant::mathematical::pi,1.5));
					//Info<<"weight = "<< weight <<endl;
				       pointForce[cellI] += TotForce* weight ;
				      }
			}
		//Info<<"flag1"<< i << "+" <<j << endl;
		//iter++;
		//Info<<"iter["<<i+1<<"]["<<j+1<<"] = "<< iter <<endl;
		}
	}


power = torque  *  Foam::constant::mathematical::twoPi / T2;
Info<<"totalpower = "<< power <<endl;
Info<<"totaltorque = "<< torque <<endl;
Info<<"drag_on_stream = "<< drag_on_stream <<endl;



