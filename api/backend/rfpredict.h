#pragma once

// ML
#include <mlpack/core.hpp>
#include <mlpack/methods/random_forest/random_forest.hpp>

// Frontend
#include "frontend/mainframe.h"

// Backend
#include "backend/data.h"

// Paths regarding model
#define FEATURES_CSV_PATH "./model/feats.csv"
#define LABELS_CSV_PATH "./model/labels.csv"
#define MODEL_EXPORT_PATH "./model/model.xml"

class RFPredictor {
public:
    RFPredictor(MainFrame* mainFrame, DataBase* dataBase);

    bool PredictMatchOutcome(int matchNum);
private:
    bool LoadModel(const std::string& modelPath);
    void TrainModel(
        const std::string& featuresPath, 
        const std::string& labelsPath
    );
    bool PredictMatchOutcome(
        const Match& match,
        std::vector<double> teamWinRates
    );

    mlpack::RandomForest<> m_rf;
    MainFrame* m_mainFrame;
    DataBase* m_dataBase;
};
