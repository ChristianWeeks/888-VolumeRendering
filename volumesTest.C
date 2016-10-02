
#include "volume_operators.h"
#include <vector>
#include <stdio.h>
//#include <boost/shared_ptr.hpp>
#include <iostream>
#include <memory>



int main(void){

    lux::Vector testVec(0.5, 0.5, 0.5);
    lux::Vector testVec1(0.75, 0.75, 0.75);

    std::vector<lux::Volume<float>*> testFloatVolumes;
    std::vector<lux::Volume<lux::Vector>*> testVectorVolumes;

    testFloatVolumes.reserve(10);
    testVectorVolumes.reserve(10);


    auto vol1 = std::make_shared<lux::ConstantVolume<float> >(0.5);
    auto vol2 = std::make_shared<lux::ConstantVolume<float> >(0.75);
    auto vol3 = std::make_shared<lux::ConstantVolume<lux::Vector> >(testVec);
    auto vol4 = std::make_shared<lux::ConstantVolume<lux::Vector> >(testVec1);

    printf("Vol1: %f\n", vol1->eval(testVec));
    printf("Vol2: %f\n\n", vol2->eval(testVec));

    testFloatVolumes.push_back(new lux::AddVolume<float>(vol1, vol2));
    testFloatVolumes.push_back(new lux::SubtractVolume<float>(vol1, vol2));
    testFloatVolumes.push_back(new lux::MultVolume<float>(vol1, vol2));
    testFloatVolumes.push_back(new lux::DivideVolume(vol1, vol2));
    testFloatVolumes.push_back(new lux::ExpVolume<float>(vol1));

    testFloatVolumes.push_back(new lux::UnionVolume<float>(vol1, vol2));
    testFloatVolumes.push_back(new lux::IntersectVolume<float>(vol1, vol2));
    testFloatVolumes.push_back(new lux::CutoutVolume<float>(vol1, vol2));

    testFloatVolumes.push_back(new lux::SinVolume(vol1));
    testFloatVolumes.push_back(new lux::CosVolume(vol1));
    testFloatVolumes.push_back(new lux::DotProductVolume(vol3, vol4));

    testVectorVolumes.push_back(new lux::Mult_SV_Volume(vol1, vol3));
    testVectorVolumes.push_back(new lux::Mult_SV_Volume(vol3, vol1));
    //testVectorVolumes.push_back(new lux::SinVolume<lux::Vector>(vol3));

    std::cout << "Add Volume: " <<  testFloatVolumes[0]->eval(testVec) << "\n";
    std::cout << "Subtract Volume: " <<  testFloatVolumes[1]->eval(testVec) << "\n";
    std::cout << "Mult Volume: " <<  testFloatVolumes[2]->eval(testVec) << "\n";
    std::cout << "Divide Volume: " <<  testFloatVolumes[3]->eval(testVec) << "\n";
    std::cout << "Exp Volume: " <<  testFloatVolumes[4]->eval(testVec) << "\n\n";

    std::cout << "Union Volume: " <<  testFloatVolumes[5]->eval(testVec) << "\n";
    std::cout << "Intersect Volume: " <<  testFloatVolumes[6]->eval(testVec) << "\n";
    std::cout << "Cutout Volume: " <<  testFloatVolumes[7]->eval(testVec) << "\n\n";

    std::cout << "Sin Volume: " <<  testFloatVolumes[8]->eval(testVec) << "\n";
    std::cout << "Cos Volume: " <<  testFloatVolumes[9]->eval(testVec) << "\n";
    std::cout << "DotProduct Volume: " <<  testFloatVolumes[9]->eval(testVec) << "\n\n";

    std::cout << "Scalar * Vector = " << testVectorVolumes[0]->eval(testVec) << "\n";
    std::cout << "Vector * Scalar = " << testVectorVolumes[1]->eval(testVec) << "\n";
    //std::cout << "SinVectorVol" << testVectorVolumes[1]->eval(testVec) << "\n";

    for (unsigned int i = 0; i < testFloatVolumes.size(); i++){
        delete testFloatVolumes[i];
    }
    for (unsigned int i = 0; i < testVectorVolumes.size(); i++){
        delete testVectorVolumes[i];
    }

    return 0;
}

