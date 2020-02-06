/**
 *  @file   test_gnss_dcs.cpp
 *  @author Ryan Watson & Jason Gross
 *  @brief  Factor graph dynamic covariance scaling to enable robust GPS processing. This example processes both pseudorange and carrier-phase observations. For more inforomation on how carrier-phase is incorporated, please see [3].

 * REF::
 * [1] "Batch Measurement Error Covariance Estimation for Robust Localization", Watson, R., Taylor, C., Leishman, R., and Gross, J., ION GNSS+ 2018.

 * [2] "Robust map optimization using dynamic covariance scaling", Agarwal, P., et. al., 2013 IEEE International Conference on Robotics and Automation

 * [3] Watson, Ryan M., and Jason N. Gross. "Evaluation of kinematic precise point positioning convergence with an incremental graph optimizer." Position, Location and Navigation Symposium (PLANS), 2018 IEEE/ION. IEEE, 2018.

 * how to run ::
 * ./test_gnss_dcs -h
 * ./test_gnss_dcs -i data.gtsam --writeENU --dir test


 -----------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------
  MIT License
  Copyright (c) 2019 Ryan M. Watson, Jason N. Gross, Clark N. Taylor, and Robert C. Leishman
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
 -----------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------

 **/
// GTSAM
#include <gtsam/base/Vector.h>
#include <gtsam/base/Matrix.h>
#include <gtsam/inference/Key.h>
#include <gtsam/geometry/Point3.h>
#include <gtsam/base/FastVector.h>
#include <gtsam/inference/Symbol.h>
#include <gtsam/nonlinear/Values.h>
#include <gtsam/slam/PriorFactor.h>
#include <gtsam/linear/NoiseModel.h>
#include <gtsam/slam/BetweenFactor.h>
#include <gtsam/nonlinear/Marginals.h>
#include <gtsam/inference/FactorGraph.h>
#include <gtsam/gnssNavigation/GnssData.h>
#include <gtsam/gnssNavigation/GnssTools.h>
#include <gtsam/nonlinear/DoglegOptimizer.h>
#include <gtsam/gnssNavigation/GNSSFactor.h>
#include <gtsam/nonlinear/NonlinearFactor.h>
#include <gtsam/gnssNavigation/FolderUtils.h>
#include <gtsam/gnssNavigation/GnssPostfit.h>
#include <gtsam/gnssNavigation/gnssStateVec.h>
#include <gtsam/nonlinear/NonlinearFactorGraph.h>
#include <gtsam/nonlinear/GaussNewtonOptimizer.h>
#include <gtsam/nonlinear/NonlinearOptimizer.h>
#include <gtsam/nonlinear/LevenbergMarquardtOptimizer.h>

// BOOST
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

// STANDARD
#include <fstream>
#include <iostream>
#include <tuple>
#include <ios>

// Intel Threading Building Block
#ifdef GTSAM_USE_TBB
  #include <tbb/tbb.h>
  #undef max // TBB seems to include windows.h and we don't want these macros
  #undef min
#endif

using namespace std;
using namespace gtsam;
using namespace boost;

namespace po = boost::program_options;

typedef noiseModel::Diagonal diagNoise;
namespace NM = gtsam::noiseModel;

int main(int argc, char** argv) {

        bool noTrop, switchFactor, switchFactorWBetween, writeGraph, elWeight, using_fs=false;
        bool writeENU, writeECEF, writeBias, loose, tight, robust = false, first_ob = true;
        int currKey=-1, trop=1, startEpoch=0, satKeyPrev=-1, sc=1, nThreads, startKey;
        int num_gps_factors=0, factorCount=-1, lastStep, firstStep, robustIter;
        double rangeWeight, phaseScale, percentFaulty, kernelWidth;
        string gnssFile, insFile, outputFile, residualTxtInit="initResidaul.txt";
        string residualTxtOut="finalResidual.txt",textExtension=".txt", strategy;
        string switchExtension = "Switch.txt", graphExtension=".dot", dir;
        vector<string> satIndexLiteral, timeIndex;
        vector<rnxData> data;
        vector<pair<int,int> > priorFactors;
        vector<pair<int,int> > ambFactors, betweenFactors;
        typedef std::tuple<int, int, int>  tup;
        vector<tup> gnssFactors;
        Values initial_values, optimized_values;
        Eigen::MatrixXd residuals;

        cout.precision(10);
        // define std out print color
        const string red("\033[0;31m");
        const string green("\033[0;32m");
        const string lineBreak = "###########################\n";

        NonlinearFactorGraph graph;

        po::options_description desc("Available options");
        desc.add_options()
                ("help,h", "Print help message")
                ("gpsObs,i", po::value<string>(&gnssFile)->default_value(""),
                "Input GNSS data file")
                ("percentFaulty", po::value<double>(&percentFaulty)->default_value(0.0),
                "Percentage of observations to add faults. (scale [0,1]) )")
                ("outFile,o", po::value<string>(&outputFile)->default_value("initResults"),
                "Write graph and solution to the specified file.")
                ("firstStep,f", po::value<int>(&firstStep)->default_value(0),
                "First step to process from the dataset file")
                ("lastStep,l", po::value<int>(&lastStep)->default_value(-1),
                "Last step to process, or -1 to process until the end of the dataset")
                ("threads", po::value<int>(&nThreads)->default_value(-1),
                "Number of threads, or -1 to use all processors")
                ("noTrop", "Will turn residual troposphere estimation off. Troposphere will still be modeled.")
                ("robustIter",po::value<int>(&robustIter)->default_value(1),
                "Number of iterations before initial postfit data edit")
                ("dir", po::value<string>(&dir)->default_value(""),
                "Total path to store generated data")
                ("elWeight,el", "Elevation angle dependant measuremnt weighting")
                ("rangeWeight", po::value<double>(&rangeWeight)->default_value(2.5),
                "Assumed Pseudorange Noise")
                ("phaseScale", po::value<double>(&phaseScale)->default_value(100),
                "phase noise = range noise * 1/phaseScale")
                ("kernelWidth",po::value<double>(&kernelWidth)->default_value(10.0),
                "Dynamic Covariance Scaling kernel width")
                ("writeGraph",
                "Write graph to text file. Do not write large graphs (i.e. Nodes>=100)")
                ("writeECEF", "write ecef solution to file")
                ("writeENU", "write enu solution to file")
                ("writeBias", "write carrier-phase ambiguity values to file")
        ;

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        po::notify(vm);

        writeGraph = (vm.count("writeGraph")>0);
        writeENU = (vm.count("writeENU")>0);
        writeECEF = (vm.count("writeECEF")>0);
        writeBias = (vm.count("writeBias")>0);
        noTrop = (vm.count("noTrop") > 0);

        if ( gnssFile.empty() ) {
                cout << red << "\n\n GNSS data must be specified\n"
                     << "\n\n" << green << desc << endl;
                exit(1);
        }

        if ( noTrop ) { trop = 0; }

        #ifdef GTSAM_USE_TBB
        std::auto_ptr<tbb::task_scheduler_init> init;
        if(nThreads > 0) {
                cout << "\n\n Using " << nThreads << " threads " << endl;
                init.reset(new tbb::task_scheduler_init(nThreads));
        }
        else
                cout << green << " \n\n Using threads for all processors" << endl;
        #else
        if(nThreads > 0) {
                cout << red <<" \n\n GTSAM is not compiled with TBB, so threading is"
                     << " disabled and the --threads option cannot be used."
                     << endl;
                exit(1);
        }
        #endif

        // set up directory to store all generated data
        if ( dir.empty() ) { dir = getTimestamp(); }
        makeDir( dir );
        chdir( dir.c_str() );

        // Specify the starting location of the platform in ECEF XYZ.
        // Nom. pos. for the phastball dataset.
        // Point3 nomXYZ(820699.8121, -4888612.5490, 4000524.7052);
        // greenhouse
        // Point3 nomXYZ(856514.1467,-4843013.0689, 4047939.8237);
		// Nom. pos. for JRC, Ispra, Italy
        Point3 nomXYZ(4403261.3874,667819.3824,4550797.4440);
        // Nom. pos. for the dec12 dataset.
        // Point3 nomXYZ(856295.3346, -4843033.4111, 4048017.6649);
        Point3 nomNED(0.0, 0.0, 0.0);

        double output_time = 0.0;
        rangeWeight = pow(rangeWeight,2);
        double phaseWeight = pow(rangeWeight*(1/phaseScale), 2);

        phaseBias bias_state(Z_1x1);
        gnssStateVector phase_arc(Z_34x1);
        gnssStateVector bias_counter(Z_34x1);
        for (int i=1; i<34; i++) {bias_counter(i) = bias_counter(i-1) + 10000; }
        nonBiasStates initEst((gtsam::Vector(5) << 0,0,0,0,0).finished());

        using symbol_shorthand::X; // nonBiasStates ( dx, dy, dz, trop, cb )
        using symbol_shorthand::G; // bias states ( Phase Bias )

        noiseModel::Diagonal::shared_ptr nonBias_InitNoise = noiseModel::Diagonal::Variances((gtsam::Vector(5) << 1.0, 1.0, 1.0, 3e6, 1e-1).finished());

        noiseModel::Diagonal::shared_ptr nonBias_ProcessNoise = noiseModel::Diagonal::Variances((gtsam::Vector(5) << 0.5, 0.5, 0.5, 1e3, 1e-3).finished());

        noiseModel::Diagonal::shared_ptr initNoise = noiseModel::Diagonal::Variances((gtsam::Vector(1) << 3e6).finished());

        // Read GNSS data
        try {data = readGNSS_SingleFreq(gnssFile); }
        catch(std::exception& e)
        {
                cout << red << "\n\n Cannot read GNSS data file " << endl;
                exit(1);
        }

        strategy = "GNSS only using max-mixtures";
        cout << green << "\n\n" << lineBreak << " GNSS Data File :: "
             << gnssFile << endl;
        cout << "\n Processing Strategy  :: "  << strategy << endl;
        cout << lineBreak << endl;

        if ( firstStep != 0 ) {
                using_fs = true;
                for( unsigned int i = 0; i < data.size(); i++ ) {
                        if ( firstStep == get<1>(data[i]) ) { break; }
                        ++startEpoch;
                }
        }
        if ( lastStep < 0 ) { lastStep = get<0>(data.back()); }

        // Construct Graph --- Only Pseudorange factors
        for(unsigned int i = startEpoch; i < data.size()-1; i++ ) {

                // Get the current epoch's observables
                double gnssTime = get<0>(data[i]);
                int currKey = get<1>(data[i]);
                if (first_ob || using_fs) {
                        timeIndex.push_back(to_string(gnssTime));
                        startKey = currKey; first_ob=false; using_fs=false; graph.add(PriorFactor<nonBiasStates>(X(currKey), initEst,  nonBias_InitNoise));
                        priorFactors.push_back(make_pair(i,++factorCount));
                        optimized_values.insert(X(currKey), initEst);
                }
                int nextKey = get<1>(data[i+1]);
                int svn = get<2>(data[i]);
                Point3 satXYZ = get<3>(data[i]);
                double rho = get<4>(data[i]);
                double range = get<5>(data[i]);
                double phase = get<6>(data[i]);
                double phase_break = get<7>(data[i]);
                gtsam::Vector2 obs;
                obs << range-rho, phase-rho;

                if ( currKey == startKey || phase_arc[svn]!=phase_break)
                {
                        bias_state[0] = phase-range;
                        if (currKey > startKey) { bias_counter[svn] = bias_counter[svn] +1; }
                        optimized_values.insert(G(bias_counter[svn]), bias_state);
                        graph.add(boost::make_shared<PriorFactor<phaseBias> >(G(bias_counter[svn]), bias_state,  initNoise));
                        phase_arc[svn] = phase_break;
                        ambFactors.push_back(make_pair(i, ++factorCount));
                }

                double rw = elDepWeight(satXYZ, nomXYZ, rangeWeight);
                double pw = elDepWeight(satXYZ, nomXYZ, phaseWeight);
                Eigen::VectorXd noiseVec(2);
                noiseVec << rw, pw;

                NM::Diagonal::shared_ptr model = noiseModel::Diagonal::Variances(noiseVec);

                // Generate phase
                graph.add(boost::make_shared<GNSSFactor>(X(currKey), G(bias_counter[svn]), obs, satXYZ, nomXYZ, model ));

                gnssFactors.push_back( tup(i,++factorCount,bias_counter[svn]));
                satIndexLiteral.push_back(to_string(currKey) + " " + to_string(svn));

                // Add prior and between factors
                if ( currKey != nextKey && nextKey != 0) {
                        if ( lastStep == nextKey ) { break; }
                        timeIndex.push_back(to_string(get<0>(data[i+1])));
                        double scale = (get<0>(data[i+1])-get<0>(data[i]))*10.0;
                        optimized_values.insert(X(nextKey),initEst);
                        nonBias_ProcessNoise = noiseModel::Diagonal::Variances((gtsam::Vector(5) << 0.5*scale, 0.5*scale, 0.5*scale, 1e3*scale, 1e-3*scale).finished());

                        graph.add(boost::make_shared<BetweenFactor<nonBiasStates> >(X(currKey), X(nextKey), initEst, nonBias_ProcessNoise));

                        betweenFactors.push_back(make_pair(i, ++factorCount));
                }
        }
        optimized_values = LevenbergMarquardtOptimizer(graph, optimized_values).optimize();

        for (std::size_t i = 0; i<gnssFactors.size(); i++) {

                int dataInd = std::get<0>(gnssFactors[i]);
                int currKey = get<1>(data[dataInd]);
                int biasKey = get<2>(gnssFactors[i]);
                int svn = get<2>(data[dataInd]);
                Point3 satXYZ = get<3>(data[dataInd]);
                double rho = get<4>(data[dataInd]);
                double range = get<5>(data[dataInd]);
                double phase = get<6>(data[dataInd]);

                gtsam::Vector2 obs;
                obs << range-rho, phase-rho;

                double rw = elDepWeight(satXYZ, nomXYZ, rangeWeight);
                double pw = elDepWeight(satXYZ, nomXYZ, phaseWeight);

                Eigen::VectorXd noiseVec(2);
                noiseVec << rw, pw;

                NM::Diagonal::shared_ptr model = noiseModel::Diagonal::Variances(noiseVec);
                NM::Robust::shared_ptr robustModel = NM::Robust::Create(NM::mEstimator::DCS::Create(kernelWidth), model);

                // Update phase factor
                auto gnssFactor = boost::make_shared<GNSSFactor>(X(currKey), G(biasKey), obs, satXYZ, nomXYZ, robustModel);

                graph.replace(get<1>(gnssFactors[i]), gnssFactor);
        }
        optimized_values = LevenbergMarquardtOptimizer(graph, optimized_values).optimize();

        try {
                // Render to PDF using "fdp pseudorange.dot -Tpdf > graph.phf"
                string enuSol = "enu.sol";
                string ecefSol = "ecef.sol";
                string optimizedGraph = "graph.dot";
                string resultString = "state.values";
                string biasString = "bias.values";
                writeStates( optimized_values, timeIndex, resultString );
                if (writeENU) { ofstream os(enuSol); writeNavFrame( optimized_values, nomXYZ, timeIndex, enuSol ); }
                if (writeGraph) { ofstream os(optimizedGraph); graph.saveGraph(os,optimized_values); }
                if (writeECEF) { ofstream os(ecefSol); writeEarthFrame( optimized_values, nomXYZ, timeIndex, ecefSol ); }
                if (writeBias) {ofstream os(biasString); writeAmbiguity(optimized_values, biasString, satIndexLiteral); }
        }
        catch(std::exception& e) { cout << e.what() << endl; exit(1); }
        return 0;
}
