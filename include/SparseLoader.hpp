#ifndef SPARSE_LOADER_HPP
#define SPARSE_LOADER_HPP

#include <vector>
#include <string>

struct IngestionDiagnostics {
    size_t totalCellsRead = 0;
    size_t missingDataGapsPatched = 0;
    double rawDataMeanValue = 0.0;
    bool processingSuccess = false;
};

class SparseLoader {
public:
    // Configures the boundary threshold limits for outlier rejection
    explicit SparseLoader(double lowerBound = -50.0, double upperBound = 50.0);

    // Parses a delimited text matrix string (simulating open-source CSV downloads)
    std::vector<std::vector<double>> parseRawStream(
        const std::string& rawTextPayload, 
        size_t expectedRows, 
        size_t expectedCols,
        IngestionDiagnostics& outDiagnostics
    );

    // Reconstructs and fills sparse empty tracks using spatial diffusion
    void healMissingTracks(
        std::vector<std::vector<double>>& grid, 
        const std::vector<std::vector<bool>>& maskMatrix,
        int maxDiffusionPasses = 100
    );

private:
    double m_lowerBound;
    double m_upperBound;
};

#endif // SPARSE_LOADER_HPP
