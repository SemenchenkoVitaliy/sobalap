#include "libr.h"

using namespace plb;
using namespace plb::descriptors;
using namespace std;

#if defined _WIN32 || _WIN64 || defined __CYGWIN__
    #define OS_WINDOWS
#endif
#if defined OS_WINDOWS
    #define PATH_SEPARATOR "\\"
#else
    #define PATH_SEPARATOR "/"
#endif


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

T computeRMSerror ( MultiBlockLattice2D<T,DESCRIPTOR>& lattice,
                    IncomprFlowParam<T> const& parameters ) {
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

Setup::Setup(){

}

void Setup::setInlet(int x1, int y1, int x2, int y2){
        inlet = Box2D(x2, x1, y2, y1);
}

void Setup::setOutlet(int x1, int y1, int x2, int y2){
    pcout << x1<< " " <<x2 << " "<< y1 << " "<< y2 << endl;

        outlet = Box2D(x1, x2, y1, y2);
}

void Setup::setCalcPoint(int x, int y){
    xCalc = x;
    yCalc = y;
}

void Setup::setInletOutlet(std::string s){
    if(s == "pressure") inletOutlet = pressure;
    else inletOutlet = velocity;
}

void Setup::setTimeInterval(double t, double i){
    imSave = i;
    maxT = t;
}

void Setup::setSizes(double lx, double ly){
    this->lx = lx;
    this->ly = ly;
}

void Setup::setOutDir(string str){
    outDir = str;
}

void Setup::setOutFileName(string str){
    outFileName = str;
}

void Setup::exec(ElementsMetaData data){
    global::directories().setOutputDir(outDir + PATH_SEPARATOR);
    IncomprFlowParam<T> parameters(
        (T) 2e-2,  // uMax
        (T) 5.,    // Re
        100,        // N
        lx,        // lx //change this: lenght
        ly         // ly //change this: heigth
    );
plint N = 600;
    plint envelopeWidth = 1;
    Nx = 800;
    Ny = 600;
    SparseBlockStructure2D sparseBlock(Nx+1, Ny+1);
//    sparseBlock.addBlock(Box2D(0, 100,      0, 600),   sparseBlock.nextIncrementalId());
//    sparseBlock.addBlock(Box2D(100+1, 500-1, 0, 600/2+1), sparseBlock.nextIncrementalId());
//    sparseBlock.addBlock(Box2D(500, 800,      0, 600),   sparseBlock.nextIncrementalId());
//    pcout << Nx << " " << Ny;
            for(plint i = 0; i < Nx; i+=10)
                for(plint j = 0; j < Ny; j+=10)
                    if(!data.contains(i,j)){//is point in wall
                        if(i == Nx - 10 && j == Ny - 10) sparseBlock.addBlock(Box2D(i, i + 10, j, j + 10),
                                                                           sparseBlock.nextIncrementalId());
                        else if(i == Nx - 10)sparseBlock.addBlock(Box2D(i, i + 10, j, j + 9),
                                                 sparseBlock.nextIncrementalId());
                        else if(j == Ny - 10)sparseBlock.addBlock(Box2D(i, i + 9, j, j + 10),
                                                 sparseBlock.nextIncrementalId());
                        else sparseBlock.addBlock(Box2D(i, i + 9, j, j + 9),
                                                 sparseBlock.nextIncrementalId());
                    }

    // Instantiate the multi-block, based on the created block distribution and
    //   on default parameters.
    MultiBlockLattice2D<T, DESCRIPTOR> lattice (
        MultiBlockManagement2D (
            sparseBlock,
            defaultMultiBlockPolicy2D().getThreadAttribution(), envelopeWidth ),
        defaultMultiBlockPolicy2D().getBlockCommunicator(),
        defaultMultiBlockPolicy2D().getCombinedStatistics(),
        defaultMultiBlockPolicy2D().getMultiCellAccess<T,DESCRIPTOR>(),
        new BGKdynamics<T,DESCRIPTOR>(parameters.getOmega())
    );

    OnLatticeBoundaryCondition2D<T,DESCRIPTOR>*
        boundaryCondition = createLocalBoundaryCondition2D<T,DESCRIPTOR>();

    channelSetup(lattice, parameters, *boundaryCondition);

    std::string mkDir = "mkdir -p \"" + outDir + "\"";
    system(mkDir.c_str());

    ofstream myfile;
    myfile.open(outDir + "\\log.txt");
    for (plint iT=0; iT*parameters.getDeltaT()<maxT; ++iT) {
           if (iT%parameters.nStep(imSave)==0) {
                pcout << "Saving Gif ..." << endl;
                writeGif(lattice, iT);
                myfile << "step " << iT
                      << "; t=" << iT*parameters.getDeltaT()
                      << "; RMS error=" << computeRMSerror(lattice, parameters);
                Array<T,2> uCenter;
                lattice.get(xCalc,yCalc).computeVelocity(uCenter);
                myfile << "; velocity=" << uCenter[0]/parameters.getLatticeU();
                myfile << "; pressure=" << lattice.get(xCalc,yCalc).computePressure() << endl;

            }

            // Lattice Boltzmann iteration step.
            lattice.collideAndStream();
        }
    myfile.close();

    delete boundaryCondition;

    std::stringstream imStream;
    imStream << "convert -delay " << imSave * 100 << " \""
             << global::directories().getImageOutDir()
             << "*.ppm\" \""
             << global::directories().getImageOutDir()
             << outFileName << "\"";
    plint errorConv = system(imStream.str().c_str());

    pcout << imStream.str().c_str();
    if (errorConv != 0) plbWarning("Error in using ImageMagick convert command: " + imStream.str());

    #ifdef OS_WINDOWS
    std::string rmCommand =
        std::string("del /Q /F /S \"") +
        global::directories().getImageOutDir() + "*.ppm\"";
    #else
        std::string rmCommand =
            std::string("/bin/rm \"") +
            global::directories().getImageOutDir() + "*.ppm\"";
    #endif
        plint errorRm = system(rmCommand.c_str());
        if (errorRm != 0) plbWarning("Error in removing temporary ppm file.");
}

SparseBlockStructure2D Setup::sparseSetup(ElementsMetaData data){
    SparseBlockStructure2D sparseBlock(Nx, Ny);
    sparseBlock.addBlock(Box2D(0, 600, 0, 800),
                               sparseBlock.nextIncrementalId());

    return sparseBlock;
}

void Setup::channelSetup( MultiBlockLattice2D<T,DESCRIPTOR>& lattice,
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

    if (inletOutlet == pressure) {
            // Note: pressure boundary conditions are currently implemented
            //   only for edges of the boundary, and not for corner nodes.
            boundaryCondition.setPressureConditionOnBlockBoundaries (
                    lattice, inlet );
            boundaryCondition.setPressureConditionOnBlockBoundaries (
                    lattice, outlet, boundary::neumann );
        }
        else {
            boundaryCondition.setVelocityConditionOnBlockBoundaries (
                    lattice, inlet );
            boundaryCondition.setVelocityConditionOnBlockBoundaries (
                    lattice, outlet, boundary::neumann );
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

void Setup::writeGif(MultiBlockLattice2D<T,DESCRIPTOR>& lattice, plint iter){
    ImageWriter<T> imageWriter("leeloo");
    imageWriter.writeScaledPpm(createFileName("u", iter, 6),
                           *computeVelocityNorm(lattice));
}

