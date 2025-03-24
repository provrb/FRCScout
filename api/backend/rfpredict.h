#pragma once

// ML
#include <mlpack/core.hpp>
#include <mlpack/methods/random_forest/random_forest.hpp>

// Frontend
#include "frontend/mainframe.h"

// Backend
#include "backend/data.h"

class RFPredictor {
public:
    RFPredictor(MainFrame* mainFrame, DataBase* dataBase, const std::string& modelPath);

    uint8_t PredictMatchOutcome(int matchNum);
    uint8_t PredictMatchOutcome(const Match& match);
private:
    bool LoadModel(const std::string& modelPath);
    void TrainModel(
        const std::string& featuresPath, 
        const std::string& labelsPath
    );

    mlpack::RandomForest<> m_rf;
    MainFrame* m_mainFrame;
    DataBase* m_dataBase;
    std::string m_modelPath;
};
