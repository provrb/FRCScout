#include "rfpredict.h"

RFPredictor::RFPredictor(MainFrame* mainFrame, DataBase* dataBase)
    : m_mainFrame(mainFrame), m_dataBase(dataBase)
{
    if ( !LoadModel(MODEL_EXPORT_PATH) ) // No model exists at the correct path
        // Train and create a model
        TrainModel(FEATURES_CSV_PATH, LABELS_CSV_PATH);
}

bool RFPredictor::PredictMatchOutcome(int matchNum) {
    if ( !m_available )
        return false;

    const Match match = m_dataBase->GetMatch(matchNum);
    
    // indices 0-2: red team win rates
    // indices 3-5: blue team win rates
    std::vector<double> winRates = {};

    // calculate each teams win %
    for ( const Team& team : match.teams ) {
        double winRate = m_dataBase->GetTeamWinRate(team.teamNum);
        winRates.push_back(winRate);
    }

    return PredictMatchOutcome(match, winRates);
}

// Need data as such to make an prediction with ~82% accuracy
// Red 1 | Red 2 | Red 3 | Blue 1 | Blue 2 | Blue 3 | Red 1 Win % | Red 2 Win % | Red 3 Win % | Blue 1 Win % | Blue 2 Win % | Blue 3 Win %
// Returns 1 for red win, 0 for blue win
bool RFPredictor::PredictMatchOutcome(
    const Match& match, 
    std::vector<double> teamWinRates
) 
{
    // data to supply the model with
    arma::mat features(12, 1);

    // teams
    features(0, 0) = match.Team1().teamNum;
    features(1, 0) = match.Team2().teamNum;
    features(2, 0) = match.Team3().teamNum;
    features(3, 0) = match.Team4().teamNum;
    features(4, 0) = match.Team5().teamNum;
    features(5, 0) = match.Team6().teamNum;

    // win rates for red alliance teams
    features(6, 0) = teamWinRates.at(0);
    features(7, 0) = teamWinRates.at(1);
    features(8, 0) = teamWinRates.at(2);

    // win rates for blue alliance teams
    features(9, 0) = teamWinRates.at(3);
    features(10, 0) = teamWinRates.at(4);
    features(11, 0) = teamWinRates.at(5);
    
    // output. the models prediction
    arma::Row<size_t> prediction; 

    // predict
    m_rf.Classify(features, prediction);

    return prediction(0); // return result
}

void RFPredictor::TrainModel(
    const std::string& featuresPath, 
    const std::string& labelsPath
)
{
    arma::mat features;
    arma::Row<size_t> labels;
    
    try {
        mlpack::data::Load(featuresPath, features, true);
        mlpack::data::Load(labelsPath, labels, true);
    } catch ( std::runtime_error& err ) {
        m_mainFrame->LogErrorMessage("Error loading data to train model. Predictions unavailable.");
        return;
    }

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
    m_rf.Train(trainFeatures, trainLabels, 500, 40, 6);

    // Predict after training
    arma::Row<size_t> predictions;
    m_rf.Classify(testFeatures, predictions);

    // Calculate accuracy of predictions
    size_t correct = arma::accu(predictions == testLabels);
    double accuracy = (( double ) correct / ( double ) testLabels.n_elem) * 100;
    
    // Output the accuracy of the model
    std::string msg = "Trained RF Model with an accuracy of : " + std::to_string(accuracy) + "%";
    m_mainFrame->LogMessage(msg);

    // mark the random forest model as usable
    m_available = true;

    // save to file
    mlpack::data::Save(MODEL_EXPORT_PATH, "model", m_rf);
}

bool RFPredictor::LoadModel(const std::string& modelPath) {
    try {
        m_available = mlpack::data::Load(modelPath, "model", m_rf);
    } catch ( std::runtime_error& err ) {
        m_mainFrame->LogErrorMessage("Error loading model. Predictions unavailable.");
    }
    return m_available;
}
