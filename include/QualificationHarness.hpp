#ifndef QUALIFICATION_HARNESS_HPP
#define QUALIFICATION_HARNESS_HPP

#include <vector>
#include <string>

struct OcclusionReport {
    size_t totalOccludedCells = 0;
    double rootMeanSquareError = 0.0;
    double structuralSimilarityScore = 0.0; // Structural alignment (0.0 to 1.0)
    bool qualificationPassed = false;
};

class QualificationHarness {
public:
    QualificationHarness() = default;

    // Erases a specific region of a known grid and builds a blurred satellite radar surface
    std::vector<std::vector<double>> generateBlindSatelliteOcclusion(
        const std::vector<std::vector<double>>& trueBenthicGrid,
        size_t startRow, size_t endRow,
        size_t startCol, size_t endCol,
        double attenuationFactor = 0.0003
    );

    // Compares the blind inversion output explicitly against the erased truth area
    OcclusionReport executeQualificationTest(
        const std::vector<std::vector<double>>& blindInferredGrid,
        const std::vector<std::vector<double>>& trueBenthicGrid,
        size_t startRow, size_t endRow,
        size_t startCol, size_t endCol
    );
};

#endif // QUALIFICATION_HARNESS_HPP
