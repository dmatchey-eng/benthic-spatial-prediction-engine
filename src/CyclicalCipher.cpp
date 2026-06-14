#include "CyclicalCipher.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

CyclicalCipher::CyclicalCipher(int rotationSteps, double scaleGamma)
    : m_rotationSteps(rotationSteps), m_gamma(scaleGamma) {}

double CyclicalCipher::computeDirectionalGradient(double dx, double dy, double angleRad) {
    return dx * std::cos(angleRad) + dy * std::sin(angleRad);
}

std::vector<std::vector<double>> CyclicalCipher::executeEliminationWorkload(
    const std::vector<std::vector<double>>& sshGrid) 
{
    if (sshGrid.empty() || sshGrid[0].empty()) return {};
    
    size_t rows = sshGrid.size();
    size_t cols = sshGrid[0].size();
    
    std::vector<std::vector<double>> integratedSignals(rows, std::vector<double>(cols, 0.0));
    std::vector<std::vector<double>> minProfiles(rows, std::vector<double>(cols, 99999.0));
    std::vector<std::vector<double>> maxProfiles(rows, std::vector<double>(cols, -99999.0));

    // GOLDEN RATIO CONSTANT: Used to create high-variability Fibonacci non-uniform angle steps
    const double GOLDEN_ANGLE_RAD = 2.399963229728653; 

    std::cout << "  ├─ [Calculus Evolution] Executing Fibonacci-Spaced Convolution Sweeps..." << std::endl;

    // 1. Run the non-uniform rotating cipher matrix passes
    for (int step = 0; step < m_rotationSteps; ++step) {
        // Distribute scan angles non-linearly to eliminate background pixel noise artifacts
        double currentAngle = fmod(step * GOLDEN_ANGLE_RAD, 2.0 * M_PI);

        // Pre-compute trigonometric weights for our 3x3 Euler convolution kernel
        double kx = std::cos(currentAngle);
        double ky = std::sin(currentAngle);

        for (size_t r = 1; r < rows - 1; ++r) {
            for (size_t c = 1; c < cols - 1; ++c) {
                // 3x3 ROTATING CONVOLUTION KERNEL MATRICES:
                // Evaluates all 8 neighboring coordinates simultaneously along the current angle
                double northWest = sshGrid[r-1][c-1] * (-kx - ky);
                double northEast = sshGrid[r-1][c+1] * (kx - ky);
                double southWest = sshGrid[r+1][c-1] * (-kx + ky);
                double southEast = sshGrid[r+1][c+1] * (kx + ky);
                
                double northSouthGradient = (sshGrid[r-1][c] - sshGrid[r+1][c]) * ky;
                double westEastGradient   = (sshGrid[r][c+1] - sshGrid[r][c-1]) * kx;

                // Combine kernel weights into a singular directional signal vector
                double passSignal = std::abs(northWest + northEast + southWest + southEast + northSouthGradient + westEastGradient);
                
                integratedSignals[r][c] += passSignal;

                // Capture volatility signatures across the angular planes
                if (passSignal < minProfiles[r][c]) minProfiles[r][c] = passSignal;
                if (passSignal > maxProfiles[r][c]) maxProfiles[r][c] = passSignal;
            }
        }
    }

    // 2. Structural Anisotropy Reconstruction Layer
    std::vector<std::vector<double>> finalTopography(rows, std::vector<double>(cols, 0.0));
    
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            if (r == 0 || r == rows - 1 || c == 0 || c == cols - 1) {
                finalTopography[r][c] = m_gamma; 
                continue;
            }

            double meanSignal = integratedSignals[r][c] / m_rotationSteps;
            double structuralVolatility = maxProfiles[r][c] - minProfiles[r][c];

            // Localized scale correction based on the angular variance profile
            double adaptiveScaleCorrection = 1.0 + (structuralVolatility * 3.14);
            
            finalTopography[r][c] = (meanSignal * m_gamma * adaptiveScaleCorrection);
            
            if (finalTopography[r][c] < -6500.0) finalTopography[r][c] = -6500.0;
        }
    }

    return finalTopography;
}
