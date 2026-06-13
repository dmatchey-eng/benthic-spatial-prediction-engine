#include "CyclicalCipher.hpp"
#include <cmath>
#include <algorithm>

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
    
    // Initialize empty accumulation tensor matrix
    std::vector<std::vector<double>> inferredTopography(rows, std::vector<double>(cols, 0.0));
    double angleStep = (2.0 * M_PI) / m_rotationSteps;

    // Run rotating lens loops
    for (int step = 0; step < m_rotationSteps; ++step) {
        double currentAngle = step * angleStep;

        for (size_t r = 1; r < rows - 1; ++r) {
            for (size_t c = 1; c < cols - 1; ++c) {
                // Compute primary central differences (gradients)
                double dx = (sshGrid[r][c + 1] - sshGrid[r][c - 1]) / 2.0;
                double dy = (sshGrid[r + 1][c] - sshGrid[r - 1][c]) / 2.0;
                
                // Extract directional components to cancel dynamic fluid vectors
                double directionalSignal = std::abs(computeDirectionalGradient(dx, dy, currentAngle));
                inferredTopography[r][c] += directionalSignal;
            }
        }
    }

    // Final Haruhi normalization pass converting gravity fields to bathymetric scales
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            inferredTopography[r][c] = (inferredTopography[r][c] / m_rotationSteps) * m_gamma;
        }
    }

    return inferredTopography;
}
