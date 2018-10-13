// according to the Re number and attack angle, from foilData interpelate the cl and cd 


scalar interpCl(scalar Re, scalar attack_angle,List<scalar> Re_list,List<scalar> attack_list,List<List<scalar> > mutiRe_Cl)
{
	int flag = 1e3;
	scalar Cl; scalar Cl1; scalar Cl2;
	//List<scalar> Re_list;
	//List<scalar> attack_list;
	//List<List<scalar> > mutiRe_Cl;
	for(int i=0;i<Re_list.size();i++)
	{
	//Info<<"Re_list[i] = "<< Re_list[i] <<endl;
	if (Re < Re_list[i])
		{
		flag = i;
		break;
		}
	}
	//Info<<"flag = "<< flag <<endl;
	//now we got the index of bigRe
	if (flag == 0)
	{
	Info<<"Re number is too low, please take care of your case"<<endl;
	scalarField alpha_list(attack_list.size(),0.0);alpha_list = attack_list;
	scalarField mutiRe_Cl1(mutiRe_Cl[0].size(),0.0);mutiRe_Cl1 = mutiRe_Cl[0];
	Cl = interpolateXY(attack_angle,alpha_list,mutiRe_Cl1);
	return Cl;
	}
	else
	{
	scalar bigRe = Re_list[flag];
	scalar lowRe = Re_list[flag - 1 ];
	//now we get the Re which will be used to interpelate
	//Cl[i][j] = interpolateXY(attackAng[i][j],alpha,Cl_old);
	scalarField alpha_list(attack_list.size(),0.0);alpha_list = attack_list;
	scalarField mutiRe_Cl1(mutiRe_Cl[0].size(),0.0);mutiRe_Cl1 = mutiRe_Cl[flag];
	scalarField mutiRe_Cl2(mutiRe_Cl[0].size(),0.0);mutiRe_Cl2 = mutiRe_Cl[flag-1];
	Cl1 = interpolateXY(attack_angle,alpha_list,mutiRe_Cl1);
	Cl2 = interpolateXY(attack_angle,alpha_list,mutiRe_Cl2);
	Cl = Cl1 * (Re - lowRe)/( bigRe -lowRe)  +  Cl2 * (bigRe - Re)/( bigRe -lowRe) ;
	return Cl;
	}
}

scalar interpCd(scalar Re, scalar attack_angle,List<scalar> Re_list,List<scalar> attack_list,List<List<scalar> > mutiRe_Cd)
{
	int flag = 1e3;
	scalar Cd; scalar Cd1; scalar Cd2;
	//List<scalar> Re_list;
	//List<scalar> attack_list;
	//List<List<scalar> > mutiRe_Cd;
	for(int i=0;i<Re_list.size();i++)
	{
	//Info<<"Re_list[i] = "<< Re_list[i] <<endl;
	if (Re < Re_list[i])
		{
		flag = i;
		break;
		}
	}
	//Info<<"flag = "<< flag <<endl;
	//now we got the index of bigRe
	if (flag == 0)
	{
	Info<<"Re number is too low, please take care of your case"<<endl;
	scalarField alpha_list(attack_list.size(),0.0);alpha_list = attack_list;
	scalarField mutiRe_Cd1(mutiRe_Cd[0].size(),0.0);mutiRe_Cd1 = mutiRe_Cd[0];
	Cd = interpolateXY(attack_angle,alpha_list,mutiRe_Cd1);
	return Cd;
	}
	else
	{
	scalar bigRe = Re_list[flag];
	scalar lowRe = Re_list[flag - 1 ];
	//now we get the Re which will be used to interpelate
	//Cd[i][j] = interpolateXY(attackAng[i][j],alpha,Cd_old);
	scalarField alpha_list(attack_list.size(),0.0);alpha_list = attack_list;
	scalarField mutiRe_Cd1(mutiRe_Cd[0].size(),0.0);mutiRe_Cd1 = mutiRe_Cd[flag];
	scalarField mutiRe_Cd2(mutiRe_Cd[0].size(),0.0);mutiRe_Cd2 = mutiRe_Cd[flag-1];
	Cd1 = interpolateXY(attack_angle,alpha_list,mutiRe_Cd1);
	Cd2 = interpolateXY(attack_angle,alpha_list,mutiRe_Cd2);
	Cd = Cd1 * (Re - lowRe)/( bigRe -lowRe)  +  Cd2 * (bigRe - Re)/( bigRe -lowRe) ;
	return Cd;
	}
}
