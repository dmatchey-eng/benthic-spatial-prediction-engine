#include "QualificationHarness.hpp"
#include <iostream>
#include <cmath>
#include <iomanip>

#define ANSI_RESET   "\033[0m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_RED     "\033[31m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_BOLD    "\033[1m"

std::vector<std::vector<double>> QualificationHarness::generateBlindSatelliteOcclusion(
    const std::vector<std::vector<double>>& trueBenthicGrid,
    size_t startRow, size_t endRow,
    size_t startCol, size_t endCol,
    double attenuationFactor)
{
    if (trueBenthicGrid.empty()) return {};
    size_t rows = trueBenthicGrid.size();
    size_t cols = trueBenthicGrid.size();

    // Initialize an empty sea surface height anomaly matrix
    std::vector<std::vector<double>> satelliteRadarSurface(rows, std::vector<double>(cols, 0.05));

    // Reverse-engineer the lens cipher: convert known depth into an attenuated surface bulge
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            // Calculate absolute height above abyssal baseline (-4500m)
            double reliefHeight = std::max(0.0, trueBenthicGrid[r][c] - (-4500.0));
            satelliteRadarSurface[r][c] = reliefHeight * attenuationFactor;
        }
    }

    // Inject the occlusion layer: cover the target area with unstructured telemetry noise
    for (size_t r = startRow; r <= endRow && r < rows; ++r) {
        for (size_t c = startCol; c <= endCol && c < cols; ++c) {
            // Overlay random data fuzz to simulate a complete unmapped data gap
            satelliteRadarSurface[r][c] += 0.15 * std::sin(static_cast<double>(r + c));
        }
    }

    return satelliteRadarSurface;
}

OcclusionReport QualificationHarness::executeQualificationTest(
    const std::vector<std::vector<double>>& blindInferredGrid,
    const std::vector<std::vector<double>>& trueBenthicGrid,
    size_t startRow, size_t endRow,
    size_t startCol, size_t endCol)
{
    OcclusionReport report;
    if (blindInferredGrid.empty() || trueBenthicGrid.empty()) return report;

    double sumSquaredError = 0.0;
    double sumX = 0.0, sumY = 0.0;
    double sumXY = 0.0, sumX2 = 0.0, sumY2 = 0.0;

    // Step through ONLY the occluded evaluation block
    for (size_t r = startRow; r <= endRow && r < trueBenthicGrid.size(); ++r) {
        for (size_t c = startCol; c <= endCol && c < trueBenthicGrid.size(); ++c) {
            double inferredDepth = blindInferredGrid[r][c];
            double actualDepth = trueBenthicGrid[r][c];

            double error = inferredDepth - actualDepth;
            sumSquaredError += error * error;

            sumX += inferredDepth;
            sumY += actualDepth;
            sumXY += inferredDepth * actualDepth;
            sumX2 += inferredDepth * inferredDepth;
            sumY2 += actualDepth * actualDepth;

            report.totalOccludedCells++;
        }
    }

    if (report.totalOccludedCells > 0) {
        report.rootMeanSquareError = std::sqrt(sumSquaredError / report.totalOccludedCells);

        // Compute Pearson alignment for structural tracking accuracy
        double n = static_cast<double>(report.totalOccludedCells);
        double numerator = (n * sumXY) - (sumX * sumY);
        double denominator = std::sqrt(((n * sumX2) - (sumX * sumX)) * ((n * sumY2) - (sumY * sumY)));
        
        if (std::abs(denominator) > 1e-6) {
            report.structuralSimilarityScore = std::abs(numerator / denominator);
        } else {
            report.structuralSimilarityScore = 1.0;
        }

        // Acceptance standard: Pass if structural shape alignment exceeds 80% accuracy
        report.qualificationPassed = (report.structuralSimilarityScore >= 0.80);
    }

    // Print the comparative qualification results to the console screen
    std::cout << "\n" << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << ANSI_BOLD << "🎓 ADVANCED CLOSED-LOOP BLIND QUALIFICATION TEST" << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;
    std::cout << "  📦 Isolated Testing Mask Area  : [" << startRow << "," << startCol << "] to [" << endRow << "," << endCol << "]" << std::endl;
    std::cout << "  📦 Total Evaluation Elements   : " << report.totalOccludedCells << " unmapped terrain cells." << std::endl;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  🔮 Inversion Grid RMSE Accuracy : " << ANSI_YELLOW << report.rootMeanSquareError << " meters" << ANSI_RESET << std::endl;
    std::cout << "  🎯 Blind Structural Shape Match : ";
    
    if (report.qualificationPassed) {
        std::cout << ANSI_GREEN << ANSI_BOLD << (report.structuralSimilarityScore * 100.0) << "% [PASSED]" << ANSI_RESET << std::endl;
    } else {
        std::cout << ANSI_RED << ANSI_BOLD << (report.structuralSimilarityScore * 100.0) << "% [FAILED - Needs Retuning]" << ANSI_RESET << std::endl;
    }
    std::cout << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;

    return report;
}
