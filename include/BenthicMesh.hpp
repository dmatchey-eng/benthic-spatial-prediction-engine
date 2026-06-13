#ifndef BENTHIC_MESH_HPP
#define BENTHIC_MESH_HPP

#include <string>
#include <vector>

struct IngestionMeta {
    std::string timestamp;
    std::string satelliteSource;
    double horizontalResolutionMeters;
};

class BenthicMesh {
public:
    explicit BenthicMesh(double resolution = 500.0);
    
    bool exportToWavefrontObj(
        const std::vector<std::vector<double>>& topoMatrix,
        const std::string& filepath,
        const IngestionMeta& meta
    );

private:
    double m_resolution;
    std::string generateMetadataHeader(
        const std::vector<std::vector<double>>& topoMatrix, 
        const IngestionMeta& meta
    );
};

#endif // BENTHIC_MESH_HPP
