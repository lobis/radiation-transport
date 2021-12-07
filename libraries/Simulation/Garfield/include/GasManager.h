//
// Created by lobis on 06/12/2021.
//

#ifndef RADIATION_TRANSPORT_GASMANAGER_H
#define RADIATION_TRANSPORT_GASMANAGER_H

#include <TROOT.h>

#include "Garfield/FundamentalConstants.hh"
#include "Garfield/MediumMagboltz.hh"

class GasManager {
    ClassDef(GasManager, 1);

   public:
    GasManager();

    std::shared_ptr<Garfield::MediumMagboltz> fGas;

    void Print() const;

    void SetGasPressureInBar(double bar);

    std::string GetGasName() const;

    std::string GenerateGas(const std::string& directory = "");

   public:
    void ReadGasFromFile(const std::string&);

    double GetDriftVelocity(double E /* V/cm */);
    double GetDiffusionCoefficientLongitudinal(double E /* V/cm */);
    double GetDiffusionCoefficientTransversal(double E /* V/cm */);

   private:
    static constexpr double fUnitBar = 750.062;  // in torr, garfield units
};

#endif  // RADIATION_TRANSPORT_GASMANAGER_H
