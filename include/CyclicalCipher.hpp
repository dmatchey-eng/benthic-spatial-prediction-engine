#ifndef CYCLICAL_CIPHER_HPP
#define CYCLICAL_CIPHER_HPP

#include <vector>

class CyclicalCipher {
public:
    explicit CyclicalCipher(int rotationSteps = 12, double scaleGamma = -5500.0);
    
    // Processes raw SSH inputs to infer benthic topography boundaries
    std::vector<std::vector<double>> executeEliminationWorkload(
        const std::vector<std::vector<double>>& sshGrid
    );

private:
    int m_rotationSteps;
    double m_gamma;
    
    // Internal function to compute directional derivatives at arbitrary angles
    double computeDirectionalGradient(double dx, double dy, double angleRad);
};

#endif // CYCLICAL_CIPHER_HPP
