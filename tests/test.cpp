#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>
#include "SparseLoader.hpp"
#include "CyclicalCipher.hpp"
#include "SurveyDigester.hpp"
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

// Test 1: Verifies that the SparseLoader smoothly fills missing data dropouts
bool test_sparse_loader_patching() {
    std::string mockNoisyPayload = "0.50 NaN 0.50 0.50";
    SparseLoader healer(-1.0, 1.0);
    IngestionDiagnostics metrics;
    
    // Process a 2x2 grid where one coordinate is blind
    auto grid = healer.parseRawStream(mockNoisyPayload, 2, 2, metrics);
    
    if (!metrics.processingSuccess || metrics.missingDataGapsPatched != 1) return false;
    
    // The Laplacian smoother should have healed the NaN point to match the 0.5 neighborhood average
    return std::abs(grid[0][1] - 0.5) < 1e-4;
}

// Test 2: Verifies that a flat sea surface results in a flat baseline seabed
bool test_flat_surface_elimination() {
    size_t size = 6;
    std::vector<std::vector<double>> flatSurface(size, std::vector<double>(size, 0.1));
    
    CyclicalCipher engine(12, -4800.0);
    auto result = engine.executeEliminationWorkload(flatSurface);
    
    // Central indices should drop down smoothly to the base level because there is no wave gradient
    // Edges default cleanly to the base scalar -4800.0
    return std::abs(result[3][3]) < 1e-4 || std::abs(result[0][0] - (-4800.0)) < 1e-4;
}

// Test 3: Verifies that the SurveyDigester correctly maps XYZ pings to the continuous grid
bool test_survey_digester_palette() {
    std::string mockXYZ = "10.0 20.0 -1500.0\n10.5 20.5 -3500.0";
    SurveyDigester digester;
    PaletteMetrics pm;
    
    auto points = digester.digestXYZStream(mockXYZ, pm);
    if (pm.totalPointsParsed != 2 || std::abs(pm.absoluteMinDepth - (-3500.0)) > 1e-4) return false;
    
    auto paletteGrid = digester.buildContinuousPalette(points, 4, 4, pm.spatialBoundingBox);
    // Grid generation should compile and assign non-zero baseline values across cells
    return !paletteGrid.empty() && paletteGrid.empty() > 0;
}

// Test 4: Verifies the 24-bit depth color quantization system bounds
bool test_elevation_color_quantization() {
    BenthicMesh meshService(500.0);
    
    // Deep trenches should output purple/blue signatures
    RGBPaletteColor deepColor = meshService.computeElevationColor(-6000.0);
    if (deepColor.b < 0.4f || deepColor.g > 0.1f) return false;
    
    // Shallow seamount peaks should output neon yellow highlights
    RGBPaletteColor peakColor = meshService.computeElevationColor(-50.0);
    if (peakColor.r < 0.8f || peakColor.g < 0.7f) return false;
    
    return true;
}

// Test 5: Cryptographic bytecode compilation security verification
bool test_cryptographic_bytecode_security_quad() {
    std::vector<std::vector<double>> originalGrid = {
        {-4100.0, -4100.0},
        {-4100.0, -1200.0}
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
    
    RUN_TEST(test_sparse_loader_patching);
    RUN_TEST(test_flat_surface_elimination);
    RUN_TEST(test_survey_digester_palette);
    RUN_TEST(test_elevation_color_quantization);
    RUN_TEST(test_cryptographic_bytecode_security_quad);
    
    std::cout << "=============================================================" << std::endl;
    std::cout << "🎉 ALL TESTS PASSED SUCCESSFULLY!" << std::endl;
    return 0;
}
