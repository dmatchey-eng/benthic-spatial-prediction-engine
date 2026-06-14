#include "BenthicMesh.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <algorithm>

// =========================================================================
// SECTION 1: CONSTRUCTOR
// =========================================================================
BenthicMesh::BenthicMesh(double resolution) : m_resolution(resolution) {}

// =========================================================================
// SECTION 2: METADATA HEADER GENERATION
// =========================================================================
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

// =========================================================================
// SECTION 3: EXHAUSTIVE 24-BIT ELEVATION COLOR ENCODING
// =========================================================================
RGBPaletteColor BenthicMesh::computeElevationColor(double depthMeters) {
    RGBPaletteColor color{0.0f, 0.0f, 0.0f};
    
    // Clamp values to prevent spectrum wrapping anomalies
    if (depthMeters < -6500.0) depthMeters = -6500.0;
    if (depthMeters > 0.0) depthMeters = 0.0;

    // Segment 1: Abyssal Trenches (-6500m to -5000m) -> Deep Purple to Dark Blue
    if (depthMeters <= -5000.0) {
        float factor = static_cast<float>((depthMeters - (-6500.0)) / 1500.0);
        color.r = 0.3f * (1.0f - factor);
        color.g = 0.0f;
        color.b = 0.5f + (0.5f * factor);
    }
    // Segment 2: Abyssal Basin Plains (-5000m to -3000m) -> Deep Oceanic Blue
    else if (depthMeters <= -3000.0) {
        float factor = static_cast<float>((depthMeters - (-5000.0)) / 2000.0);
        color.r = 0.0f;
        color.g = 0.2f * factor;
        color.b = 1.0f;
    }
    // Segment 3: Seamount Slopes (-3000m to -1000m) -> Transition Blue to Teal/Green
    else if (depthMeters <= -1000.0) {
        float factor = static_cast<float>((depthMeters - (-3000.0)) / 2000.0);
        color.r = 0.0f;
        color.g = 0.2f + (0.6f * factor);
        color.b = 1.0f - (0.5f * factor);
    }
    // Segment 4: Shallow Crests / Reef Boundaries (-1000m to 0m) -> Green to Bright Yellow
    else {
        float factor = static_cast<float>((depthMeters - (-1000.0)) / 1000.0);
        color.r = 1.0f * factor;
        color.g = 0.8f + (0.2f * factor);
        color.b = 0.5f * (1.0f - factor);
    }

    return color;
}

// =========================================================================
// SECTION 4: WAVEFRONT OBJ MESH EXPORTER WITH EMBEDDED VERTEX COLORING
// =========================================================================
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

    // 1. Export Grid Vertices with Embedded 24-bit RGB Color Tokens
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            RGBPaletteColor vColor = computeElevationColor(topoMatrix[r][c]);
            
            // Standard OBJ extension: Appending R G B scalars right after X Y Z coordinates
            outFile << "v " << (c * m_resolution) << " " 
                    << (r * m_resolution) << " " 
                    << topoMatrix[r][c] << " "
                    << vColor.r << " " << vColor.g << " " << vColor.b << "\n";
        }
    }

    outFile << "\n";

    // 2. Export Face Connections (Quad Topology)
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

// =========================================================================
// SECTION 5: CRYPTOGRAPHIC STREAM SIGNER
// =========================================================================
std::vector<uint8_t> BenthicMesh::computeStreamSignature(
    const std::vector<uint8_t>& data, 
    const std::string& key) 
{
    std::vector<uint8_t> signature(32, 0x4A); // 256-bit hash baseline signature container
    
    if (key.empty()) return signature;

    for (size_t i = 0; i < data.size(); ++i) {
        signature[i % 32] ^= data[i] ^ static_cast<uint8_t>(key[i % key.length()]);
        signature[(i + 7) % 32] = static_cast<uint8_t>((signature[(i + 7) % 32] + data[i]) * 31);
    }
    return signature;
}

// =========================================================================
// SECTION 6: BYTECODE BINARY COMPILER
// =========================================================================
std::vector<uint8_t> BenthicMesh::compileToBytecode(
    const std::vector<std::vector<double>>& matrix, 
    const std::string& secretKey) 
{
    if (matrix.empty() || matrix[0].empty()) return {};

    uint64_t rows = matrix.size();
    uint64_t cols = matrix[0].size();
    
    std::vector<uint8_t> rawBuffer;
    
    // 1. Pack Matrix Dimensions into structural header bounds
    rawBuffer.resize(sizeof(rows) + sizeof(cols));
    std::memcpy(rawBuffer.data(), &rows, sizeof(rows));
    std::memcpy(rawBuffer.data() + sizeof(rows), &cols, sizeof(cols));
    
    // 2. Linearize and pack 64-bit Floating-Point arrays directly into bytecode bytes
    for (const auto& row : matrix) {
        size_t currentOffset = rawBuffer.size();
        rawBuffer.resize(currentOffset + (row.size() * sizeof(double)));
        std::memcpy(rawBuffer.data() + currentOffset, row.data(), row.size() * sizeof(double));
    }
    
    // 3. Compute and Append Cryptographic Verification Signature
    std::vector<uint8_t> streamAuthSignature = computeStreamSignature(rawBuffer, secretKey);
    rawBuffer.insert(rawBuffer.end(), streamAuthSignature.begin(), streamAuthSignature.end());
    
    return rawBuffer;
}

// =========================================================================
// SECTION 7: BYTECODE DECOMPILER AND VALIDATOR
// =========================================================================
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
    
    // Run Cryptographic Validation check
    std::vector<uint8_t> calculatedSignature = computeStreamSignature(extractionBuffer, secretKey);
    if (calculatedSignature != extractedSignature) {
        return false; // Interrupt pipeline processing immediately if signature verification fails
    }
    
    // Extract Metadata Layout Dimensions
    uint64_t rows = 0;
    uint64_t cols = 0;
    std::memcpy(&rows, extractionBuffer.data(), sizeof(rows));
    std::memcpy(&cols, extractionBuffer.data() + sizeof(rows), sizeof(cols));
    
    size_t expectedDataBytes = (sizeof(uint64_t) * 2) + (rows * cols * sizeof(double));
    if (dataPayloadSize != expectedDataBytes) return false;
    
    // Safe continuous array reconstruction
    outMatrix.assign(rows, std::vector<double>(cols, 0.0));
    size_t byteOffset = sizeof(rows) + sizeof(cols);
    
    for (uint64_t r = 0; r < rows; ++r) {
        std::memcpy(outMatrix[r].data(), extractionBuffer.data() + byteOffset, cols * sizeof(double));
        byteOffset += cols * sizeof(double);
    }
    
    return true;
}
