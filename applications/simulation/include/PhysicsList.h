//
// Created by lobis on 25/11/2021.
//

#ifndef RADIATION_TRANSPORT_PHYSICSLIST_H
#define RADIATION_TRANSPORT_PHYSICSLIST_H

#include <PhysicsListConfig.h>

#include <G4VModularPhysicsList.hh>

class PhysicsList : public G4VModularPhysicsList {
   public:
    explicit PhysicsList(const PhysicsListConfig&);
    ~PhysicsList() override = default;

    virtual void SetCuts() override;
    virtual void ConstructParticle() override;
    virtual void ConstructProcess() override;

   private:
    const PhysicsListConfig fConfig;

    PhysicsList& operator=(const PhysicsList& right);
    PhysicsList(const PhysicsList&);

    G4VPhysicsConstructor* fEmPhysicsList{};
    G4VPhysicsConstructor* fDecayPhysicsList{};
    G4VPhysicsConstructor* fRadioactiveDecayPhysicsList{};

    std::vector<G4VPhysicsConstructor*> fHadronPhys;
};

#endif  // RADIATION_TRANSPORT_PHYSICSLIST_H
