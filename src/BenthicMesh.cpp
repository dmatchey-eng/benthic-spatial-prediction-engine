#include "BenthicMesh.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

BenthicMesh::BenthicMesh(double resolution) : m_resolution(resolution) {}

std::string BenthicMesh::generateMetadataHeader(
    const std::vector<std::vector<double>>& topoMatrix, const IngestionMeta& meta) 
{
    double minDepth = 0.0;
    double maxDepth = -99999.0;
    
    for (const auto& row : topoMatrix) {
        for (double val : row) {
            if (val < minDepth) minDepth = val;
            if (val > maxDepth) maxDepth = val;
        }
    }

    std::stringstream ss;
    ss << "# =========================================================\n"
       << "# BENTHIC ANALYSIS METADATA HEADER (JSON-compatible)\n"
       << "# {\n"
       << "#   \"export_timestamp_utc\": \"" << meta.timestamp << "\",\n"
       << "#   \"satellite_sources\": \"" << meta.satelliteSource << "\",\n"
       << "#   \"inferred_metrics\": {\n"
       << "#     \"max_calculated_depth_meters\": " << minDepth << ",\n"
       << "#     \"min_calculated_depth_meters\": " << maxDepth << "\n"
       << "#   }\n"
       << "# }\n"
       << "# =========================================================\n\n";
    return ss.str();
}

bool BenthicMesh::exportToWavefrontObj(
    const std::vector<std::vector<double>>& topoMatrix, 
    const std::string& filepath, 
    const IngestionMeta& meta) 
{
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) return false;

    // Write metadata tracking state data cleanly
    outFile << generateMetadataHeader(topoMatrix, meta);
    outFile << std::fixed << std::setprecision(2);

    size_t rows = topoMatrix.size();
    size_t cols = topoMatrix[0].size();

    // 1. Export Vertices
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            outFile << "v " << (c * m_resolution) << " " 
                    << (r * m_resolution) << " " 
                    << topoMatrix[r][c] << "\n";
        }
    }

    outFile << "\n";

    // 2. Export Quadrilateral Face Indirection Links
    for (size_t r = 0; r < rows - 1; ++r) {
        for (size_t c = 0; c < cols - 1; ++c) {
            size_t v1 = r * cols + c + 1;
            size_t v2 = r * cols + (c + 1) + 1;
            size_t v3 = (r + 1) * cols + (c + 1) + 1;
            size_t v4 = (r + 1) * cols + c + 1;
            
            outFile << "f " << v1 << " " << v2 << " " << v3 << " " << v4 << "\n";
        }
    }

    return true;
}
