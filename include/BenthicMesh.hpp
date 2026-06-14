#ifndef BENTHIC_MESH_HPP
#define BENTHIC_MESH_HPP

#include <string>
#include <vector>
#include <cstdint>

struct IngestionMeta {
    std::string timestamp;
    std::string satelliteSource;
    double horizontalResolutionMeters;
};

// Represents a 24-bit RGB structural material property
struct RGBPaletteColor {
    float r;
    float g;
    float b;
};

class BenthicMesh {
public:
    explicit BenthicMesh(double resolution = 500.0);
    
    bool exportToWavefrontObj(
        const std::vector<std::vector<double>>& topoMatrix,
        const std::string& filepath,
        const IngestionMeta& meta
    );

    std::vector<uint8_t> compileToBytecode(
        const std::vector<std::vector<double>>& matrix, 
        const std::string& secretKey
    );

    bool decompileFromBytecode(
        const std::vector<uint8_t>& bytecode, 
        const std::string& secretKey, 
        std::vector<std::vector<double>>& outMatrix
    );

    // --- New Exhaustive Color-Coding Module ---
    // Converts a precise depth variable into a standardized 24-bit RGB palette vector
    RGBPaletteColor computeElevationColor(double depthMeters);

private:
    double m_resolution;
    std::string generateMetadataHeader(
        const std::vector<std::vector<double>>& topoMatrix, 
        const IngestionMeta& meta
    );

    std::vector<uint8_t> computeStreamSignature(
        const std::vector<uint8_t>& data, 
        const std::string& key
    );
};

#endif // BENTHIC_MESH_HPP
