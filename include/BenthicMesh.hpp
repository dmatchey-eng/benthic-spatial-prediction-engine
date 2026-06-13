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

class BenthicMesh {
public:
    explicit BenthicMesh(double resolution = 500.0);
    
    bool exportToWavefrontObj(
        const std::vector<std::vector<double>>& topoMatrix,
        const std::string& filepath,
        const IngestionMeta& meta
    );

    // --- New Secure Bytecode Compiler / Decompiler Pair ---

    // Compiles a 2D matrix into a signed, packed binary bytecode stream
    std::vector<uint8_t> compileToBytecode(
        const std::vector<std::vector<double>>& matrix, 
        const std::string& secretKey
    );

    // Decompiles, validates, and reconstructs a matrix from a raw byte stream
    bool decompileFromBytecode(
        const std::vector<uint8_t>& bytecode, 
        const std::string& secretKey, 
        std::vector<std::vector<double>>& outMatrix
    );

private:
    double m_resolution;
    std::string generateMetadataHeader(
        const std::vector<std::vector<double>>& topoMatrix, 
        const IngestionMeta& meta
    );

    // Cryptographic validation helper (Internal simple HMAC fallback)
    std::vector<uint8_t> computeStreamSignature(
        const std::vector<uint8_t>& data, 
        const std::string& key
    );
};

#endif // BENTHIC_MESH_HPP
