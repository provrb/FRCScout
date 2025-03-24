#pragma once

#include <mlpack/core.hpp>
#include <mlpack/methods/random_forest/random_forest.hpp>

#include "frontend/mainframe.h"

class RFPredictor {
public:
    RFPredictor(MainFrame* mainFrame)
        : m_mainFrame(mainFrame)
    {
    }

    ~RFPredictor();

    void PredictMatchOutcome();
    void TrainModel(const std::string& datasetFilePath);
private:
    void ExportModel();
    void LoadModel();

    MainFrame* m_mainFrame;
};
