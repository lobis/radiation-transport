//
// Created by lobis on 06/12/2021.
//

#ifndef RADIATION_TRANSPORT_GASGENERATOR_H
#define RADIATION_TRANSPORT_GASGENERATOR_H

#include <TROOT.h>

#include "Garfield/FundamentalConstants.hh"
#include "Garfield/MediumMagboltz.hh"

class GasGenerator {
    ClassDef(GasGenerator, 1);

   public:
    GasGenerator();

    std::shared_ptr<Garfield::MediumMagboltz> fGas;

    void Print() const;

    void SetGasPressureInBar(const double bar);

   private:
    static constexpr double fUnitBar = 750.062;  // in torr, garfield units
};

#endif  // RADIATION_TRANSPORT_GASGENERATOR_H
