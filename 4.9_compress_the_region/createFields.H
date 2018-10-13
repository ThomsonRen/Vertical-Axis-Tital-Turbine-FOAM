Info<< "Reading field p\n" << endl;
volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<<"Reading field gradU\n"<< endl;
volTensorField gradU
(
    IOobject
    (
        "gradU",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::NO_WRITE
    ),
    mesh,
  dimensionedTensor("gradU",dimVelocity/dimLength,tensor::zero)
);




Info << "Creating the force vector, pointForce\n" << endl;//force calculated by the actruator line theroy
    volVectorField pointForce
    (
        IOobject
        (
            "pointForce",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh,
        dimensionedVector("pointForce",dimensionSet(0, 1, -2, 0, 0, 0, 0),vector::zero)
    );

#include "createPhi.H"




label pRefCell = 0;
scalar pRefValue = 0.0;
setRefCell(p, mesh.solutionDict().subDict("PIMPLE"), pRefCell, pRefValue);


singlePhaseTransportModel laminarTransport(U, phi);

autoPtr<incompressible::turbulenceModel> turbulence
(
    incompressible::turbulenceModel::New(U, phi, laminarTransport)
);

word Ctime;
OFstream* thrustFile_;
label outputIndex = 0;
scalar lastOutputTime;
lastOutputTime = runTime.startTime().value();


