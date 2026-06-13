#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include "CyclicalCipher.hpp"
#include "BenthicMesh.hpp"

// Simple test orchestration macro to track pass/fail states cleanly
#define RUN_TEST(testFunc) \
    do { \
        std::cout << "[ RUN      ] " << #testFunc << std::endl; \
        if (testFunc()) { \
            std::cout << "[       OK ] " << #testFunc << std::endl; \
        } else { \
            std::cerr << "[  FAILED  ] " << #testFunc << std::endl; \
            return 1; \
        } \
    } while (0)

// Test 1: Verifies that a completely flat sea surface results in a flat seabed
bool test_flat_surface_elimination() {
    size_t size = 10;
    std::vector<std::vector<double>> flatSurface(size, std::vector<double>(size, 0.0));
    
    CyclicalCipher engine(12, -4500.0);
    auto result = engine.executeEliminationWorkload(flatSurface);
    
    // The edges and centers should remain zero because there are no gradients to invert
    for (size_t r = 1; r < size - 1; ++r) {
        for (size_t c = 1; c < size - 1; ++c) {
            if (std::abs(result[r][c]) > 1e-6) return false;
        }
    }
    return true;
}

// Test 2: Verifies that the mesh exporter handles empty or malformed datasets gracefully
bool test_mesh_exporter_empty_guard() {
    std::vector<std::vector<double>> emptyMatrix;
    BenthicMesh exporter(500.0);
    IngestionMeta meta{"2026-06-13T00:00:00Z", "Test_Null", 500.0};
    
    // Should return false cleanly instead of throwing an out-of-bounds error
    bool success = exporter.exportToWavefrontObj(emptyMatrix, "null_output.obj", meta);
    return (success == false);
}

// Test 3: Structural symmetry test for the Cyclical Focus Lens
bool test_directional_gradient_symmetry() {
    size_t size = 15;
    std::vector<std::vector<double>> symmetricSsh(size, std::vector<double>(size, 0.0));
    
    // Inject a perfect isotropic square mound in the center of our workspace grid
    for (size_t r = 6; r <= 8; ++r) {
        for (size_t c = 6; c <= 8; ++c) {
            symmetricSsh[r][c] = 1.0;
        }
    }
    
    CyclicalCipher engine(8, -1000.0); // 8 steps = 45-degree sweeps
    auto result = engine.executeEliminationWorkload(symmetricSsh);
    
    // Opposite quadrants of a perfectly symmetrical feature should yield identical depth inversions
    double quad_top_left = result[4][4];
    double quad_bottom_right = result[10][10];
    
    return std::abs(quad_top_left - quad_bottom_right) < 1e-5;
}

// Test 4: Cryptographic bytecode compilation security verification
bool test_cryptographic_bytecode_security_quad() {
    std::vector<std::vector<double>> originalGrid = {
        {-4100.0, -4100.0, -4100.0},
        {-4100.0, -1200.0, -4100.0},
        {-4100.0, -4100.0, -4100.0}
    };
    
    std::string secretToken = "WyvernCoordinationKey_2026";
    BenthicMesh meshService(500.0);
    
    // Compile array structure into a raw binary bytecode stream
    std::vector<uint8_t> activeBytecodeStream = meshService.compileToBytecode(originalGrid, secretToken);
    
    std::vector<std::vector<double>> unpackedGrid;
    bool normalDecompileStatus = meshService.decompileFromBytecode(activeBytecodeStream, secretToken, unpackedGrid);
    
    // Assert reconstruction integrity
    if (!normalDecompileStatus || std::abs(unpackedGrid[1][1] - (-1200.0)) > 1e-5) return false;
    
    // Vector element bit-flip mutation logic to simulate transmission corruption
    std::vector<uint8_t> corruptedBytecodeStream = activeBytecodeStream;
    for (size_t i = 0; i < corruptedBytecodeStream.size() - 32; ++i) {
        corruptedBytecodeStream[i] ^= 0xFF; // Safely mutate internal payload bytes
    }
    
    std::vector<std::vector<double>> failedGridContainer;
    bool maliciousDecompileStatus = meshService.decompileFromBytecode(corruptedBytecodeStream, secretToken, failedGridContainer);
    
    // The decompiler must identify the data tampering and refuse to parse the bad stream (return false)
    return (maliciousDecompileStatus == false); 
}

int main() {
    std::cout << "============= STARTING BENTHIC ENGINE TEST SUITE =============" << std::endl;
    
    RUN_TEST(test_flat_surface_elimination);
    RUN_TEST(test_mesh_exporter_empty_guard);
    RUN_TEST(test_directional_gradient_symmetry);
    RUN_TEST(test_cryptographic_bytecode_security_quad);
    
    std::cout << "=============================================================" << std::endl;
    std::cout << "🎉 ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
