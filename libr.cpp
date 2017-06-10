#include "palabos2D.h"
#include "palabos2D.hh"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "libr.h"

using namespace plb;
using namespace plb::descriptors;
using namespace std;

typedef double T;
#define DESCRIPTOR D2Q9Descriptor

std::string InlOutl;
plint x1i, y1i, x2i, y2i, x1o, y1o, x2o, y2o, xCalc, yCalc;

T poiseuilleVelocity(plint iY, IncomprFlowParam<T> const& parameters) {
    T y = (T)iY / parameters.getResolution();
    return 4.*parameters.getLatticeU() * (y-y*y);
}

T poiseuillePressure(plint iX, IncomprFlowParam<T> const& parameters) {
    T Lx = parameters.getNx()-1;
    T Ly = parameters.getNy()-1;
    return 8.*parameters.getLatticeNu()*parameters.getLatticeU() / (Ly*Ly) * (Lx/(T)2-(T)iX);
}

T poiseuilleDensity(plint iX, IncomprFlowParam<T> const& parameters) {
    return poiseuillePressure(iX,parameters)*DESCRIPTOR<T>::invCs2 + (T)1;
}

template<typename T>
class PoiseuilleVelocity {
public:
    PoiseuilleVelocity(IncomprFlowParam<T> parameters_)
        : parameters(parameters_)
    { }
    void operator()(plint iX, plint iY, Array<T,2>& u) const {
        u[0] = poiseuilleVelocity(iY, parameters);
        u[1] = T();
    }
private:
    IncomprFlowParam<T> parameters;
};

template<typename T>
class PoiseuilleDensity {
public:
    PoiseuilleDensity(IncomprFlowParam<T> parameters_)
        : parameters(parameters_)
    { }
    T operator()(plint iX, plint iY) const {
        return poiseuilleDensity(iX,parameters);
    }
private:
    IncomprFlowParam<T> parameters;
};

template<typename T>
class PoiseuilleDensityAndZeroVelocity {
public:
    PoiseuilleDensityAndZeroVelocity(IncomprFlowParam<T> parameters_)
        : parameters(parameters_)
    { }
    void operator()(plint iX, plint iY, T& rho, Array<T,2>& u) const {
        rho = poiseuilleDensity(iX,parameters);
        u[0] = T();
        u[1] = T();
    }
private:
    IncomprFlowParam<T> parameters;
};

enum InletOutletT {pressure, velocity};

T computeRMSerror ( MultiBlockLattice2D<T,DESCRIPTOR>& lattice,
                    IncomprFlowParam<T> const& parameters )
{
    MultiTensorField2D<T,2> analyticalVelocity(lattice);
    setToFunction( analyticalVelocity, analyticalVelocity.getBoundingBox(),
                   PoiseuilleVelocity<T>(parameters) );
    MultiTensorField2D<T,2> numericalVelocity(lattice);
    computeVelocity(lattice, numericalVelocity, lattice.getBoundingBox());

           // Divide by lattice velocity to normalize the error
    return 1./parameters.getLatticeU() *
           // Compute RMS difference between analytical and numerical solution
               std::sqrt( computeAverage( *computeNormSqr(
                              *subtract(analyticalVelocity, numericalVelocity)
                         ) ) );
}

class Setup {

public:
    Setup(IncomprFlowParam<T> const& parameters){
        Nx = parameters.getNx();
        Ny = parameters.getNy();
    }

    void setInlet(int x1, int y1, int x2, int y2){
            inlet = Box2D(x1, x2, y1, y2);
    }

    void setOutlet(int x1, int y1, int x2, int y2){
            outlet = Box2D(x1, x2, y1, y2);
    }

    void setInletOutlet(string s){
        if(s == "pressure") inletOutlet = pressure;
        else inletOutlet = velocity;
    }

    SparseBlockStructure2D sparseSetup(plint n){
        SparseBlockStructure2D sparseBlock(Nx, Ny);
        /*for(plint i = 0; i < n; i++)
            for(plint j = 0; j < n; j++)
                if(!(i<3&&i>7&&j>3)){//is point in wall
                    if(i != n - 1) sparseBlock.addBlock(Box2D(Nx*i/n, Nx*(i+1)/n - 1, Ny*j/n, Ny*(j + 1)/n - 1),
                                              sparseBlock.nextIncrementalId());
                    else sparseBlock.addBlock(Box2D(Nx*i/n, Nx*(i+1)/n, Ny*j/n, Ny*(j + 1)/n),
                                              sparseBlock.nextIncrementalId());
                }*/
        sparseBlock.addBlock(Box2D(0, Nx/4, 0, Ny),
                             sparseBlock.nextIncrementalId());
        sparseBlock.addBlock(Box2D(0, Nx, 0, Ny/2),
                             sparseBlock.nextIncrementalId());
        sparseBlock.addBlock(Box2D(Nx*3/4, Nx, 0, Ny),
                             sparseBlock.nextIncrementalId());
        /*sparseBlock.addBlock(Box2D(0, Nx/4, 0, Ny),
                                   sparseBlock.nextIncrementalId());
        sparseBlock.addBlock(Box2D(0, Nx/4, 0, Ny),
                                   sparseBlock.nextIncrementalId());*/
        return sparseBlock;
    }

    void channelSetup( MultiBlockLattice2D<T,DESCRIPTOR>& lattice,
                   IncomprFlowParam<T> const& parameters,
                   OnLatticeBoundaryCondition2D<T,DESCRIPTOR>& boundaryCondition){
    const plint nx = parameters.getNx();
    const plint ny = parameters.getNy();

    boundaryCondition.setVelocityConditionOnBlockBoundaries (
                               lattice, Box2D(0, nx-1, 0, 0),
                                boundary::freeslip );
    boundaryCondition.setVelocityConditionOnBlockBoundaries (
                               lattice, Box2D(0, nx-1, ny-1, ny-1),
                                boundary::freeslip );

    // Pressure resp. velocity boundary condition on the inlet and outlet.
    if (inletOutlet == pressure) {
        // Note: pressure boundary conditions are currently implemented
        //   only for edges of the boundary, and not for corner nodes.
        boundaryCondition.setPressureConditionOnBlockBoundaries (
                lattice, inlet);
        boundaryCondition.setPressureConditionOnBlockBoundaries (
                lattice, outlet, boundary::outflow);
    }
    else {
        boundaryCondition.setVelocityConditionOnBlockBoundaries (
                lattice, inlet);
        boundaryCondition.setVelocityConditionOnBlockBoundaries (
                lattice, outlet, boundary::outflow);
    }

    setBoundaryDensity (
            lattice, lattice.getBoundingBox(),
            PoiseuilleDensity<T>(parameters) );
    setBoundaryVelocity (
            lattice, lattice.getBoundingBox(),
            PoiseuilleVelocity<T>(parameters) );
    initializeAtEquilibrium (
           lattice, lattice.getBoundingBox(),
           PoiseuilleDensityAndZeroVelocity<T>(parameters) );
    lattice.initialize();
}

    void writeGif(MultiBlockLattice2D<T,DESCRIPTOR>& lattice, plint iter){
        const plint imSize = 600;//change this: resolution
        ImageWriter<T> imageWriter("leeloo");

        imageWriter.writeScaledGif(createFileName("u", iter, 6),
                               *computeVelocityNorm(lattice),
                               imSize, imSize );
    }

private:
    Box2D inlet;
    Box2D outlet;
    plint Nx;
    plint Ny;
    InletOutletT inletOutlet;
};


int testSob(int argc, char* argv[]) {
    plbInit(&argc, &argv);
    global::directories().setOutputDir("./tmp/");

    if (global::argc() != 12) {
        pcout << "Error missing some input parameter\n";
        exit (EXIT_FAILURE);
    }
    try {
        global::argv(1).read(InlOutl);

        global::argv(2).read(x1i);
        global::argv(3).read(y1i);
        global::argv(4).read(x2i);
        global::argv(5).read(y2i);

        global::argv(6).read(x1o);
        global::argv(7).read(y1o);
        global::argv(8).read(x2o);
        global::argv(9).read(y2o);

        global::argv(10).read(xCalc);
        global::argv(11).read(yCalc);

    }
    catch(PlbIOException& except) {
        pcout << except.what() << std::endl;
        pcout << "Reasonable parameters: velocity 0 0 4 600 776 0 800 600 400 300";
        exit (EXIT_FAILURE);
    }

    plint envelopeWidth = 1;
    IncomprFlowParam<T> parameters(
            (T) 2e-2,  // uMax
            (T) 5.,    // Re
            100,        // N
            8.,        // lx //change this: lenght
            6.         // ly //change this: heigth
    );
    const T imSave   = (T)0.01;//change this: interval
    const T maxT     = (T)3.0;//change this: total time

    Setup obj(parameters);

    obj.setInletOutlet(InlOutl);//change this
    obj.setInlet(x1i, y1i, x2i, y2i);//change this
    obj.setOutlet(x1o, y1o, x2o, y2o);//change this

    MultiBlockLattice2D<T, DESCRIPTOR> lattice (
        MultiBlockManagement2D (
            obj.sparseSetup(60),//change this
            defaultMultiBlockPolicy2D().getThreadAttribution(), envelopeWidth),
        defaultMultiBlockPolicy2D().getBlockCommunicator(),
        defaultMultiBlockPolicy2D().getCombinedStatistics(),
        defaultMultiBlockPolicy2D().getMultiCellAccess<T,DESCRIPTOR>(),
        new BGKdynamics<T,DESCRIPTOR>(parameters.getOmega())
    );

    OnLatticeBoundaryCondition2D<T,DESCRIPTOR>*
        boundaryCondition = createLocalBoundaryCondition2D<T,DESCRIPTOR>();

    obj.channelSetup(lattice, parameters, *boundaryCondition);

    for (plint iT=0; iT*parameters.getDeltaT()<maxT; ++iT) {
       if (iT%parameters.nStep(imSave)==0) {
            pcout << "Saving Gif ..." << endl;
            obj.writeGif(lattice, iT);
            pcout << "step " << iT
                  << "; t=" << iT*parameters.getDeltaT()
                  << "; RMS error=" << computeRMSerror(lattice, parameters);
            Array<T,2> uCenter;
            lattice.get(xCalc,yCalc).computeVelocity(uCenter);
            pcout << "; center velocity=" << uCenter[0]/parameters.getLatticeU();
            pcout << "; center pressure=" << lattice.get(xCalc,yCalc).computePressure() << endl;
        }
        lattice.collideAndStream();
    }
    delete boundaryCondition;
    system("pause");
}
