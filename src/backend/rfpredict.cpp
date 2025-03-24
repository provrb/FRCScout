#include "rfpredict.h"

RFPredictor::RFPredictor(MainFrame* mainFrame, DataBase* dataBase, const std::string& modelPath)
    : m_mainFrame(mainFrame), m_dataBase(dataBase), m_modelPath(modelPath)
{
    if ( !LoadModel(m_modelPath) ) {
        // Train and create a model
        TrainModel("feats.csv", "labels.csv");
    }
}

uint8_t RFPredictor::PredictMatchOutcome(int matchNum) {
    const Match match = m_dataBase->GetMatch(matchNum);
    return PredictMatchOutcome(match);
}

// Need data as such to make an prediction with ~82% accuracy
// Red Win | Red 1 | Red 2 | Red 3 | Red 1 Win % | Red 2 Win % | Red 3 Win % | Blue 1 Win %	| Blue 2 Win % | Blue 3 Win %
// Where red win is the label, and the rest are features
// Returns 1 for red win, 0 for blue win
uint8_t RFPredictor::PredictMatchOutcome(const Match& match) {

}

void RFPredictor::TrainModel(
    const std::string& featuresPath, 
    const std::string& labelsPath
)
{
    arma::mat features;
    arma::Row<size_t> labels;

    mlpack::data::Load(featuresPath, features, true);
    mlpack::data::Load(labelsPath, labels, true);

    arma::mat trainFeatures, testFeatures;
    arma::Row<size_t> trainLabels, testLabels;

    // Split features and labels 
    // 70% of data used for training
    // 30% for testing/validating
    mlpack::data::Split(
        features, labels,
        trainFeatures, testFeatures,
        trainLabels, testLabels,
        0.3
    );

    // Train model
    mlpack::RandomForest<> rf;
    rf.Train(trainFeatures, trainLabels, 100, 50, 3);

    // Predict after training
    arma::Row<size_t> predictions;
    rf.Classify(testFeatures, predictions);

    // Calculate accuracy of predictions
    size_t correct = arma::accu(predictions == testLabels);
    double accuracy = ( double ) correct / ( double ) testLabels.n_elem;
    m_mainFrame->LogMessage("Accuracy: " + std::to_string(accuracy) + "%\n");

    // save to file
    mlpack::data::Save(m_modelPath, "model", rf);
}

bool RFPredictor::LoadModel(const std::string& modelPath) {
    return mlpack::data::Load(modelPath, "model", this->m_rf);
}
