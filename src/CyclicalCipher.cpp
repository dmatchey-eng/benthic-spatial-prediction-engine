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
    // Project spatial gradients across our rotating Euler vector angle frame
    return dx * std::cos(angleRad) + dy * std::sin(angleRad);
}

std::vector<std::vector<double>> CyclicalCipher::executeEliminationWorkload(
    const std::vector<std::vector<double>>& sshGrid) 
{
    if (sshGrid.empty() || sshGrid[0].empty()) return {};
    
    size_t rows = sshGrid.size();
    size_t cols = sshGrid[0].size();
    
    // Initialize primary accumulation grids
    std::vector<std::vector<double>> integratedSignals(rows, std::vector<double>(cols, 0.0));
    
    // Evolution Layers: Track structural dynamics across separate filter passes
    std::vector<std::vector<double>> minProfiles(rows, std::vector<double>(cols, 99999.0));
    std::vector<std::vector<double>> maxProfiles(rows, std::vector<double>(cols, -99999.0));

    double angleStep = (2.0 * M_PI) / m_rotationSteps;

    std::cout << "  ├─ [Core Evolution] Executing multi-pass directional feedback sweeps..." << std::endl;

    // 1. Run the rotating cipher lens passes
    for (int step = 0; step < m_rotationSteps; ++step) {
        double currentAngle = step * angleStep;

        for (size_t r = 1; r < rows - 1; ++r) {
            for (size_t c = 1; c < cols - 1; ++c) {
                // Compute central spatial gradients
                double dx = (sshGrid[r][c + 1] - sshGrid[r][c - 1]) / 2.0;
                double dy = (sshGrid[r + 1][c] - sshGrid[r - 1][c]) / 2.0;
                
                // Calculate current angular signal pass
                double passSignal = std::abs(computeDirectionalGradient(dx, dy, currentAngle));
                
                // Integrate baseline signals
                integratedSignals[r][c] += passSignal;

                // Track structural phase variants across the rotating filter passes
                if (passSignal < minProfiles[r][c]) minProfiles[r][c] = passSignal;
                if (passSignal > maxProfiles[r][c]) maxProfiles[r][c] = passSignal;
            }
        }
    }

    // 2. Automated Feature Inference and Depth Reconstruction Layer
    std::vector<std::vector<double>> finalTopography(rows, std::vector<double>(cols, 0.0));
    
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            if (r == 0 || r == rows - 1 || c == 0 || c == cols - 1) {
                finalTopography[r][c] = m_gamma; // Set boundary baseline depth
                continue;
            }

            double meanSignal = integratedSignals[r][c] / m_rotationSteps;
            
            // Calculate the directional volatility signature (Anisotropy metric)
            double structuralVolatility = maxProfiles[r][c] - minProfiles[r][c];

            // Adaptive Scale Correction: If an area exhibits high structural variance across passes,
            // it indicates sharp fault features. We dynamically amplify the inversion response factor.
            double adaptiveScaleCorrection = 1.0 + (structuralVolatility * 2.5);
            
            // Reconstruct the final depth profile
            finalTopography[r][c] = (meanSignal * m_gamma * adaptiveScaleCorrection);
            
            // Keep depth realistic by capping values to deep ocean basin baselines (-6500m)
            if (finalTopography[r][c] < -6500.0) finalTopography[r][c] = -6500.0;
        }
    }

    return finalTopography;
}
