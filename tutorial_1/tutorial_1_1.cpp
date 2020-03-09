/* This file is part of the Palabos library.
 *
 * The Palabos softare is developed since 2011 by FlowKit-Numeca Group Sarl
 * (Switzerland) and the University of Geneva (Switzerland), which jointly
 * own the IP rights for most of the code base. Since October 2019, the
 * Palabos project is maintained by the University of Geneva and accepts
 * source code contributions from the community.
 * 
 * Contact:
 * Jonas Latt
 * Computer Science Department
 * University of Geneva
 * 7 Route de Drize
 * 1227 Carouge, Switzerland
 * jonas.latt@unige.ch
 *
 * The most recent release of Palabos can be downloaded at 
 * <https://palabos.unige.ch/>
 *
 * The library Palabos is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Code 1.1 in the Palabos tutorial
 */

#include "palabos2D.h"
#include "palabos2D.hh"
#include <iostream>
#include <iomanip>

using namespace plb;
using namespace std;

//T��˫���ȸ���������
//double-precision floating type T
typedef double T;

//ʹ��D2Q9���ӱ�ʾ��
//Use D2Q9 Lattice descriptor
#define DESCRIPTOR plb::descriptors::D2Q9Descriptor

// Initialize the lattice at zero velocity and constant density, except
//   for a slight density excess on a square sub-domain.
// �Գ����ܶȺ����ٶȶ���lattice����һС��������ܶ�Ҫ���Ը�һЩ
void defineInitialDensityAtCenter(MultiBlockLattice2D<T,DESCRIPTOR>& lattice)
{
    // The lattice is of size nx-by-ny
// ��plint���峣��nx��ny���ֱ�Ϊ�����x��y����
// Use "plint" to define constant nx and ny, represent the length of x and y of the domain, respectively.
    const plint nx = lattice.getNx();
    const plint ny = lattice.getNy();

    // Create a Box2D which describes the location of cells with a slightly
    //  higher density.
// ����ʹ��plint������������
//use "plint" to define three variables below
//1�����α߳���һ��
//1��The half of the length of the square 
    plint centralSquareRadius = nx/6; 
    
//2������x������֮һ��λ�ã��˴���Ϊ�����ε����ĵ�x������
//2��At the position of 1/3 of x axis, means the x coordinate of square center
    plint centerX = nx/3;
//3) plint the y coordinate of the center
    plint centerY = ny/4;

//ʹ��Box2D������centralSquare��������x1,x2,y1,y2
//����x���x1��x2��y���y1��y2���ĸ���ȷ��2D��
//�˴�x1 = centerX - centralSquareRadius��x2 = centerX + centralSquareRadius
//Use Box2D to define centralSquare, as it needs input of x1,x2,y1,y2. 
//In x axis from x1 to x2, and in y axis from y1 to y2, then 4 points determine a 2D domain
//here x1 = centerX - centralSquareRadius��x2 = centerX + centralSquareRadius
    Box2D centralSquare (
            centerX - centralSquareRadius, centerX + centralSquareRadius,
            centerY - centralSquareRadius, centerY + centralSquareRadius );

    // All cells have initially density rho ...
// ���е�cell��ʼ���ܶ�rho
    T rho0 = 1.;
    // .. except for those in the box "centralSquare" which have density
    //    rho+deltaRho
//Ϊ�����в��ķ�������׼��һ����΢��һ����ܶ�
    T deltaRho = 1.e-4;

//���ģ�ϴ�����ݣ���LBģ�������Ⱥ����������palabos��BlockLattice������
//��Array<T,nDim>һ����С�����ݣ��˴�ΪT�������ͣ��ڳ�����ǰ�Ѷ���Ϊ˫���ȸ����ͣ�
//nDimΪ�ߴ磬�˴�2��ʾ����̶��ߴ��array������Ԫ��
//��Ҳ�����ڶ�����2��3Ԫ��Array��ͬʱ������һЩ����������ȥ
//For large size data, like particles in Lattice Boltzmann simulation, were stored in BlockLattice type of Palabos
//As Array<T,nDim> generally small, here it is of T data type(In the beginning of the code T was difined as double-precision floating point scalars)
//nDim is the size, here is 2, which represents this fixed-size array has two elements
// You can also store small physical vectors, such as velocity, when you define the array
    Array<T,2> u0((T)0,(T)0);

    // Initialize constant density everywhere.
//��ʼ�������ܶȣ�initializeAtEquilibrium(lattice, domain, rho, velocity)���ڸ������cells�г�ʼ��ƽ��ֲ�
//initializeAtEquilibrium(lattice, domain, rho, velocity), initialize constant density rho to the cells in the domain
    initializeAtEquilibrium (
           lattice, lattice.getBoundingBox(), rho0, u0 );

    // And slightly higher density in the central box.
//�м䷽��������ܶ���΢��һЩ
    initializeAtEquilibrium (
           lattice, centralSquare, rho0 + deltaRho, u0 );
//lattice��ʼ��
    lattice.initialize();
}//Here "void defineInitialDensityAtCenter(MultiBlockLattice2D<T,DESCRIPTOR>& lattice)" part is finished
//��ס���defineInitialDensityAtCenter�����滹�����
//Remember this "defineInitialDensityAtCenter", later will reappear


//�������д��룬һ��ͨ��argc��argv��cpp�����ڷ��ʳ������
//�ڳ�����κ�λ�ö����Է���������ȫ�ֶ���
//Usually, we access to parameters in CPP programs through argc and argv
//In anywhere of the program can we access these two global parameters
int main(int argc, char* argv[]) {
    plbInit(&argc, &argv);

//�趨�ļ����λ�ã�����tmp�ļ���
//output file directory, that is to open floder tmp
    global::directories().setOutputDir("./tmp/");

    const plint maxIter = 1000; // Iterate during 1000 steps.��������
    const plint nx = 600;       // Choice of lattice dimensions.����x�᷽��lattice������
    const plint ny = 600;
    const T omega = 1.;        // Choice of the relaxation parameter�ɳ�ʱ��

//��ʼ��һ��nx,ny��BGKģ�͵�T�������ͣ���˫���ȸ����ͣ���D2Q9��DESCRIPTOR�ڳ����������Ѷ��壩��lattice
//initialize a nx,ny size, BGK dynamics, type T(Double-precision floating), D2Q9 lattice
//BGKdynamics: The well known BGK single-relaxation-time model, which works on D2Q9, D3Q15, D3Q19 and D3Q27 ����from Palabos User Guide
    MultiBlockLattice2D<T, DESCRIPTOR> lattice (
           nx, ny, new BGKdynamics<T,DESCRIPTOR>(omega) );

//true��false������ѭ���߽�
//Select true or false to turn on or off the periodic boundaries
    lattice.periodicity().toggleAll(true); // Use periodic boundaries.

//���ǵ����defineInitialDensityAtCenter��
//You remember this defineInitialDensityAtCenter?
    defineInitialDensityAtCenter(lattice);

    // Main loop over time iterations.
// ��Ҫ�ĵ�������
    for (plint iT=0; iT<maxIter; ++iT) {
        if (iT%40==0) {  // Write an image every 40th time step. ��40ȡ��Ϊ0ʱ
            pcout << "Writing GIF file at iT=" << iT << endl;
            // Instantiate an image writer with the color map "leeloo".
            ImageWriter<T> imageWriter("leeloo");
// ��ʼ����ͼ����ɫ��leeloo
// leeloo���Ի���earth, water, air, fire
            // Write a GIF file with colors rescaled to the range of values
            //   in the matrix
// ������ͼ
            imageWriter.writeScaledGif (
                    createFileName("u", iT, 6),
                    *computeVelocityNorm(lattice) );
        }//��ͼ����
        // Execute lattice Boltzmann iteration.
// ִ����ײ������
        lattice.collideAndStream();
    }//��һ��ѭ����ֱ��iT�ﵽmaxIter
}

// Code explanation by Yulan Fang 
// Error correction please send to ahdhfang@hotmail.com
// ����March,9 2020 at Siwa, Egypt.