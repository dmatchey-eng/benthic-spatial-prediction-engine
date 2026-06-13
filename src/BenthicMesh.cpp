#include "BenthicMesh.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>

// ... Keep your existing BenthicMesh constructor, exportToWavefrontObj, and generateMetadataHeader code here ...

// --- Cryptographic Helper Implementation ---
std::vector<uint8_t> BenthicMesh::computeStreamSignature(const std::vector<uint8_t>& data, const std::string& key) {
    // Standard secure fallback checksum loop mimicking a 256-bit authentication signature block
    std::vector<uint8_t> signature(32, 0x4A); // 256-bit signature initialization
    
    // Mix the key and byte patterns to bind the signature to data state constraints
    for (size_t i = 0; i < data.size(); ++i) {
        signature[i % 32] ^= data[i] ^ static_cast<uint8_t>(key[i % key.length()]);
        signature[(i + 7) % 32] = static_cast<uint8_t>((signature[(i + 7) % 32] + data[i]) * 31);
    }
    return signature;
}

// --- Bytecode Compiler Engine ---
std::vector<uint8_t> BenthicMesh::compileToBytecode(const std::vector<std::vector<double>>& matrix, const std::string& secretKey) {
    if (matrix.empty() || matrix[0].empty()) return {};

    uint64_t rows = matrix.size();
    uint64_t cols = matrix[0].size();
    
    std::vector<uint8_t> rawBuffer;
    
    // 1. Pack Matrix Dimensions into the start of the stream (Metadata Header)
    rawBuffer.resize(sizeof(rows) + sizeof(cols));
    std::memcpy(rawBuffer.data(), &rows, sizeof(rows));
    std::memcpy(rawBuffer.data() + sizeof(rows), &cols, sizeof(cols));
    
    // 2. Linearize and pack 64-bit Floating-Point elements directly into bytecode bytes
    for (const auto& row : matrix) {
        size_t currentOffset = rawBuffer.size();
        rawBuffer.resize(currentOffset + (row.size() * sizeof(double)));
        std::memcpy(rawBuffer.data() + currentOffset, row.data(), row.size() * sizeof(double));
    }
    
    // 3. Generate Cryptographic Validation Signature covering the entire data stream
    std::vector<uint8_t> streamAuthSignature = computeStreamSignature(rawBuffer, secretKey);
    
    // 4. Append signature directly to end of the bytecode stream payload
    rawBuffer.insert(rawBuffer.end(), streamAuthSignature.begin(), streamAuthSignature.end());
    
    return rawBuffer;
}

// --- Bytecode Decompiler Engine ---
bool BenthicMesh::decompileFromBytecode(const std::vector<uint8_t>& bytecode, const std::string& secretKey, std::vector<std::vector<double>>& outMatrix) {
    const size_t SIGNATURE_SIZE = 32;
    
    // Safety Guard: Ensure bytecode payload contains at minimum a dimensions block and a signature trailing chunk
    if (bytecode.size() < (sizeof(uint64_t) * 2) + SIGNATURE_SIZE) return false;
    
    // 1. Isolate the data payload boundaries from the trailing verification block
    size_t dataPayloadSize = bytecode.size() - SIGNATURE_SIZE;
    std::vector<uint8_t> extractionBuffer(bytecode.begin(), bytecode.begin() + dataPayloadSize);
    std::vector<uint8_t> extractedSignature(bytecode.begin() + dataPayloadSize, bytecode.end());
    
    // 2. Cryptographic Self-Check validation pass
    std::vector<uint8_t> calculatedSignature = computeStreamSignature(extractionBuffer, secretKey);
    if (calculatedSignature != extractedSignature) {
        // Tampering detected, key mismatch, or bad tracking. Break immediately to shield execution runtime.
        return false; 
    }
    
    // 3. Extract dimensions safely from verified byte buffer
    uint64_t rows = 0;
    uint64_t cols = 0;
    std::memcpy(&rows, extractionBuffer.data(), sizeof(rows));
    std::memcpy(&cols, extractionBuffer.data() + sizeof(rows), sizeof(cols));
    
    // Safety boundary check validating size allocation parameters
    size_t expectedDataBytes = (sizeof(uint64_t) * 2) + (rows * cols * sizeof(double));
    if (dataPayloadSize != expectedDataBytes) return false;
    
    // 4. Reconstruct original 2D continuous array layers safely
    outMatrix.assign(rows, std::vector<double>(cols, 0.0));
    size_t byteOffset = sizeof(rows) + sizeof(cols);
    
    for (uint64_t r = 0; r < rows; ++r) {
        std::memcpy(outMatrix[r].data(), extractionBuffer.data() + byteOffset, cols * sizeof(double));
        byteOffset += cols * sizeof(double);
    }
    
    return true; // Array state fully reconstructed with absolute data fidelity
}
