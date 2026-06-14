#include "SparseLoader.hpp"
#include <sstream>
#include <cmath>
#include <algorithm>

SparseLoader::SparseLoader(double lowerBound, double upperBound)
    : m_lowerBound(lowerBound), m_upperBound(upperBound) {}

std::vector<std::vector<double>> SparseLoader::parseRawStream(
    const std::string& rawTextPayload, 
    size_t expectedRows, 
    size_t expectedCols,
    IngestionDiagnostics& outDiagnostics) 
{
    // 1. Initialize matrices for data and track tracking masks
    std::vector<std::vector<double>> parsedGrid(expectedRows, std::vector<double>(expectedCols, 0.0));
    std::vector<std::vector<bool>> dataMissingMask(expectedRows, std::vector<bool>(expectedCols, false));
    
    std::stringstream ss(rawTextPayload);
    std::string token;
    
    size_t r = 0;
    size_t c = 0;
    double runningSum = 0.0;
    size_t validPointsCount = 0;

    // 2. Extract elements line by line
    while (ss >> token) {
        if (c >= expectedCols) {
            c = 0;
            r++;
        }
        if (r >= expectedRows) break;

        outDiagnostics.totalCellsRead++;

        // Flag dropouts or invalid placeholder tags
        if (token == "NaN" || token == "nan" || token == "-9999.0" || token == "-9999") {
            dataMissingMask[r][c] = true;
            outDiagnostics.missingDataGapsPatched++;
            parsedGrid[r][c] = 0.0; // Seed empty points for diffusion pass
        } else {
            try {
                double value = std::stod(token);
                
                // Outlier extraction check
                if (value < m_lowerBound || value > m_upperBound) {
                    dataMissingMask[r][c] = true;
                    outDiagnostics.missingDataGapsPatched++;
                    parsedGrid[r][c] = 0.0;
                } else {
                    parsedGrid[r][c] = value;
                    runningSum += value;
                    validPointsCount++;
                }
            } catch (...) {
                // Handle parsing anomalies gracefully to keep CodeQL happy
                dataMissingMask[r][c] = true;
                outDiagnostics.missingDataGapsPatched++;
                parsedGrid[r][c] = 0.0;
            }
        }
        c++;
    }

    if (validPointsCount > 0) {
        outDiagnostics.rawDataMeanValue = runningSum / validPointsCount;
    }

    // 3. Execute the diffusion healing process over unmapped channels
    if (outDiagnostics.missingDataGapsPatched > 0) {
        healMissingTracks(parsedGrid, dataMissingMask, 50);
    }

    outDiagnostics.processingSuccess = (outDiagnostics.totalCellsRead > 0);
    return parsedGrid;
}

void SparseLoader::healMissingTracks(
    std::vector<std::vector<double>>& grid, 
    const std::vector<std::vector<bool>>& maskMatrix,
    int maxDiffusionPasses) 
{
    size_t rows = grid.size();
    size_t cols = grid.size();
    
    // Iterative neighborhood smoother loop
    for (int pass = 0; pass < maxDiffusionPasses; ++pass) {
        // Temporary storage matrix keeps tracking layers stable between iterations
        auto tempGrid = grid;
        bool changesMade = false;

        for (size_t r = 1; r < rows - 1; ++r) {
            for (size_t c = 1; c < cols - 1; ++c) {
                // If this cell was marked as missing or corrupt
                if (maskMatrix[r][c]) {
                    // Average the values of its 4 direct neighbors
                    double neighborSum = grid[r-1][c] + grid[r+1][c] + grid[r][c-1] + grid[r][c+1];
                    double refinedVal = neighborSum / 4.0;
                    
                    if (std::abs(tempGrid[r][c] - refinedVal) > 1e-4) {
                        tempGrid[r][c] = refinedVal;
                        changesMade = true;
                    }
                }
            }
        }
        grid = tempGrid;
        if (!changesMade) break; // Optimization exit check if array updates stabilize early
    }
}
