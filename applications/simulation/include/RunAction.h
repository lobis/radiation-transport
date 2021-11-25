
#ifndef RADIATION_TRANSPORT_RUNACTION_H
#define RADIATION_TRANSPORT_RUNACTION_H

#include <G4UserRunAction.hh>
#include <vector>

class G4Run;
class OutputManager;

class RunAction : public G4UserRunAction {
   public:
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    RunAction();
    virtual ~RunAction();

   private:
    OutputManager* output;
};

#endif  // RADIATION_TRANSPORT_RUNACTION_H
