#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "GEMDetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "QGSP_BERT.hh"
#include "G4SystemOfUnits.hh"
#include "RootOutputManager.hh"
#include <chrono>
#include <iostream>



int main(int argc, char** argv) {
    auto t_start = std::chrono::high_resolution_clock::now();

    auto runManager = new G4RunManager();
    runManager->SetUserInitialization(new GEMDetectorConstruction());
    runManager->SetUserInitialization(new QGSP_BERT());

    auto generator = new PrimaryGeneratorAction();
    runManager->SetUserAction(generator);

    runManager->Initialize();

    // Optional: Configure beam from command line
    if (argc > 1) generator->SetParticleType(argv[1]);
    if (argc > 2) generator->SetEnergy(std::stod(argv[2]) * GeV);

    //only start GUI if no macro is provided
    if (argc >= 4)
    {
        G4UImanager::GetUIpointer()->ApplyCommand(std::string("/control/execute ") + argv[3]);
    } else {
    auto visManager = new G4VisExecutive();
    visManager->Initialize();

    auto ui = new G4UIExecutive(argc, argv);
    auto UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();

    delete ui;
    delete visManager;
    }
    RootOutputManager::Instance()->Finalize();
    delete runManager;
    auto t_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = t_end - t_start;
    std::cout << "Simulation took " << elapsed.count() << " seconds.\n";

    return 0;
}

