#include "BenthicMesh.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <algorithm>

BenthicMesh::BenthicMesh(double resolution) : m_resolution(resolution) {}
std::string BenthicMesh::generateMetadataHeader(
    const std::vector<std::vector<double>>& topoMatrix, 
    const IngestionMeta& meta) 
{
    double minDepth = 0.0;
    double maxDepth = -99999.0;
    
    if (!topoMatrix.empty() && !topoMatrix[0].empty()) {
        minDepth = topoMatrix[0][0];
        maxDepth = topoMatrix[0][0];
        for (const auto& row : topoMatrix) {
            for (double val : row) {
                if (val < minDepth) minDepth = val;
                if (val > maxDepth) maxDepth = val;
            }
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
    if (topoMatrix.empty() || topoMatrix[0].empty()) return false;

    std::ofstream outFile(filepath);
    if (!outFile.is_open()) return false;

    outFile << generateMetadataHeader(topoMatrix, meta);
    outFile << std::fixed << std::setprecision(2);

    size_t rows = topoMatrix.size();
    size_t cols = topoMatrix[0].size();

    // 1. Export Grid Vertices
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            outFile << "v " << (c * m_resolution) << " " 
                    << (r * m_resolution) << " " 
                    << topoMatrix[r][c] << "\n";
        }
    }

    outFile << "\n";

    // 2. Export Face Connections
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
std::vector<uint8_t> BenthicMesh::computeStreamSignature(
    const std::vector<uint8_t>& data, 
    const std::string& key) 
{
    std::vector<uint8_t> signature(32, 0x4A); // 256-bit signature space
    
    if (key.empty()) return signature;

    for (size_t i = 0; i < data.size(); ++i) {
        signature[i % 32] ^= data[i] ^ static_cast<uint8_t>(key[i % key.length()]);
        signature[(i + 7) % 32] = static_cast<uint8_t>((signature[(i + 7) % 32] + data[i]) * 31);
    }
    return signature;
}
std::vector<uint8_t> BenthicMesh::compileToBytecode(
    const std::vector<std::vector<double>>& matrix, 
    const std::string& secretKey) 
{
    if (matrix.empty() || matrix[0].empty()) return {};

    uint64_t rows = matrix.size();
    uint64_t cols = matrix[0].size();
    
    std::vector<uint8_t> rawBuffer;
    
    // 1. Pack Dimensions
    rawBuffer.resize(sizeof(rows) + sizeof(cols));
    std::memcpy(rawBuffer.data(), &rows, sizeof(rows));
    std::memcpy(rawBuffer.data() + sizeof(rows), &cols, sizeof(cols));
    
    // 2. Pack Floating Point Data
    for (const auto& row : matrix) {
        size_t currentOffset = rawBuffer.size();
        rawBuffer.resize(currentOffset + (row.size() * sizeof(double)));
        std::memcpy(rawBuffer.data() + currentOffset, row.data(), row.size() * sizeof(double));
    }
    
    // 3. Compute and Append Cryptographic Signature
    std::vector<uint8_t> streamAuthSignature = computeStreamSignature(rawBuffer, secretKey);
    rawBuffer.insert(rawBuffer.end(), streamAuthSignature.begin(), streamAuthSignature.end());
    
    return rawBuffer;
}
bool BenthicMesh::decompileFromBytecode(
    const std::vector<uint8_t>& bytecode, 
    const std::string& secretKey, 
    std::vector<std::vector<double>>& outMatrix) 
{
    const size_t SIGNATURE_SIZE = 32;
    
    if (bytecode.size() < (sizeof(uint64_t) * 2) + SIGNATURE_SIZE) return false;
    
    size_t dataPayloadSize = bytecode.size() - SIGNATURE_SIZE;
    std::vector<uint8_t> extractionBuffer(bytecode.begin(), bytecode.begin() + dataPayloadSize);
    std::vector<uint8_t> extractedSignature(bytecode.begin() + dataPayloadSize, bytecode.end());
    
    // Validate Signature
    std::vector<uint8_t> calculatedSignature = computeStreamSignature(extractionBuffer, secretKey);
    if (calculatedSignature != extractedSignature) {
        return false; // Stop immediately if payload signature verification fails
    }
    
    // Extract Metadata Matrix Layout Bounds
    uint64_t rows = 0;
    uint64_t cols = 0;
    std::memcpy(&rows, extractionBuffer.data(), sizeof(rows));
    std::memcpy(&cols, extractionBuffer.data() + sizeof(rows), sizeof(cols));
    
    size_t expectedDataBytes = (sizeof(uint64_t) * 2) + (rows * cols * sizeof(double));
    if (dataPayloadSize != expectedDataBytes) return false;
    
    // Safe Deserialization
    outMatrix.assign(rows, std::vector<double>(cols, 0.0));
    size_t byteOffset = sizeof(rows) + sizeof(cols);
    
    for (uint64_t r = 0; r < rows; ++r) {
        std::memcpy(outMatrix[r].data(), extractionBuffer.data() + byteOffset, cols * sizeof(double));
        byteOffset += cols * sizeof(double);
    }
    
    return true;
}
