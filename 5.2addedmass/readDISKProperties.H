IOdictionary DISKProperties
(
IOobject
(
    "DISKProperties",
    runTime.time().constant(),
    runTime,
    IOobject::MUST_READ,
    IOobject::NO_WRITE
)
);


List<List<scalar> > AirfoilTable(DISKProperties.lookup("AirfoilTable"));

vector TowingDirection(DISKProperties.lookup("TowingDirection"));
vector ShaftNaxial(DISKProperties.lookup("ShaftNaxial"));
vector Naxial(DISKProperties.lookup("Naxial"));
vector Naxis_;
Naxis_ = Naxial/mag(Naxial);
Info <<"Normal vector of turbine plane is :"<< Naxis_ <<endl;

scalar TSR(readScalar(DISKProperties.lookup("TSR")));
scalar U_inf(readScalar(DISKProperties.lookup("U_inf")));
//scalar T2 = Foam::constant::mathematical::twoPi * 0.5 * ;
//scalar T2(readScalar(DISKProperties.lookup("T2")));//period of the turbine rotation
scalar epsilon(readScalar(DISKProperties.lookup("epsilon")));//Gauss width
scalar bladeNum(readScalar(DISKProperties.lookup("bladeNum")));
scalar height(readScalar(DISKProperties.lookup("height")));//height of each blade
scalar point_of_each_line(readScalar(DISKProperties.lookup("point_of_each_line")));//number of points on each line
int AA = point_of_each_line;
scalar bladeDiameter(readScalar(DISKProperties.lookup("bladeDiameter")));
scalar chord(readScalar(DISKProperties.lookup("chord")));
scalar initialPhase(readScalar(DISKProperties.lookup("initialPhase")));	
scalar turbineCenter_x(readScalar(DISKProperties.lookup("turbineCenter_x")));
scalar turbineCenter_y(readScalar(DISKProperties.lookup("turbineCenter_y")));
scalar turbineCenter_z(readScalar(DISKProperties.lookup("turbineCenter_z")));
scalar sphereRadius(readScalar(DISKProperties.lookup("sphereRadius")));
scalar rho(readScalar(DISKProperties.lookup("rho")));
scalar nu(readScalar(DISKProperties.lookup("nu")));
scalar T2 = Foam::constant::mathematical::twoPi * 0.5 * bladeDiameter /TSR/ U_inf;

vector turbineCenter(DISKProperties.lookup("turbineCenter"));
  
word outputControl = DISKProperties.subDict("globalProperties").lookupOrDefault<word>("outputControl","timeStep");
Info<<"outputControlType :"<<outputControl<<endl;
scalar outputInterval = DISKProperties.subDict("globalProperties").lookupOrDefault<scalar>("outputInterval",1);
Info<<"outputInterval :"<<outputInterval<<endl;



List<List<vector> > RelativeUtemp(bladeNum,List<vector>(AA,vector::zero));//for the calculate of first order difference
List<List<scalar> > attackAngtemp(bladeNum,List<scalar>(AA,0.0));//for the calculate of first order difference


DynamicList<label> sphereCellsI;

forAll(mesh.cells(),cellI)
        {
            if (mag(mesh.C()[cellI] - turbineCenter) <= sphereRadius)
            {
                sphereCellsI.append(cellI);
            }
        }



//*------------------------------------------------------------------------------------------*//
//new code for the use of Re interpelate 4.24

IOdictionary foilData
(
IOobject
(
    "foilData",
    runTime.time().constant(),
    runTime,
    IOobject::MUST_READ,
    IOobject::NO_WRITE
)
);

List<List<scalar> > mutiRe_Cl(foilData.lookup("mutiRe_Cl"));
List<List<scalar> > mutiRe_Cd(foilData.lookup("mutiRe_Cd"));
List<scalar> Re_list(foilData.lookup("Re_list"));
List<scalar> attack_list(foilData.lookup("attack_list"));


//scalar testRe(readScalar(DISKProperties.lookup("testRe")));

