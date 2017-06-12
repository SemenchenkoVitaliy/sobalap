#ifndef LIBR_H
#define LIBR_H
#define PLB_USE_POSIX

#include "palabos2D.h"
#include "palabos2D.hh"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "renderarea.h"

using namespace plb;
using namespace plb::descriptors;
using namespace std;

typedef double T;
#define DESCRIPTOR D2Q9Descriptor

enum InletOutletT {pressure, velocity};

class Setup{
public:
    Setup();
    void setInlet(int x1, int y1, int x2, int y2);
    void setOutlet(int x1, int y1, int x2, int y2);
    void setCalcPoint(int x, int y);
    void setInletOutlet(std::string s);
    void setTimeInterval(double t, double i);
    void setSizes(double lx, double ly);
    void setOutDir(std::string str);
    void setOutFileName(std::string str);
    void exec(ElementsMetaData& data);
private:
    Box2D inlet, outlet;
    plint Nx, Ny;
    plint xCalc, yCalc;
    InletOutletT inletOutlet;
    T imSave, maxT, lx, ly;
    std::string outDir, outFileName;

    SparseBlockStructure2D sparseSetup(ElementsMetaData data);
    void channelSetup( MultiBlockLattice2D<T,DESCRIPTOR>& lattice,
                   IncomprFlowParam<T> const& parameters,
                   OnLatticeBoundaryCondition2D<T,DESCRIPTOR>& boundaryCondition);
    void writeGif(MultiBlockLattice2D<T,DESCRIPTOR>& lattice, plint iter);
};
#endif // LIBR_H
