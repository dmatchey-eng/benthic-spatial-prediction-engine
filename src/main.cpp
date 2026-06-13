#include <iostream>
#include <vector>
#include "CyclicalCipher.hpp"
#include "BenthicMesh.hpp"

int main() {
    std::cout << "🚀 Initializing Benthic Wyvern Processing Engine..." << std::endl;

    // 1. Create a 20x20 mock satellite grid array
    size_t gridSize = 20;
    std::vector<std::vector<double>> mockSatelliteSsh(gridSize, std::vector<double>(gridSize, 0.0));

    // Simulate an underwater feature footprint (Altimetry surface bulge)
    for (size_t r = 5; r < 15; ++r) {
        for (size_t c = 5; c < 15; ++c) {
            mockSatelliteSsh[r][c] = 0.45; 
        }
    }

    // 2. Run the Cyclical Lens Elimination engine workload
    CyclicalCipher engine(12, -4800.0);
    auto inferredBathymetry = engine.executeEliminationWorkload(mockSatelliteSsh);

    // 3. Setup metadata profiling tags
    IngestionMeta currentRunMeta{
        "2026-06-13T16:24:00Z",
        "NOAA-Jason3 / ESA-Sentinel6 Harmonized Matrix",
        500.0
    };

    // 4. Initialize the BenthicMesh service instance
    double horizontalResolutionMeters = 500.0;
    BenthicMesh meshService(horizontalResolutionMeters);

    // 5. Securely compile the multi-pass grid to raw cryptographically signed bytecode
    std::string systemSecretToken = "WyvernCoordinationKey_2026";
    std::cout << "🔒 Compiling raw topography payload into verifiable bytecode..." << std::endl;
    std::vector<uint8_t> secureByteStream = meshService.compileToBytecode(inferredBathymetry, systemSecretToken);
    std::cout << "📦 Compiled Stream Size: " << secureByteStream.size() << " bytes." << std::endl;

    // 6. Run a safe internal decompilation verification loop before disk writes
    std::vector<std::vector<double>> verifiedGrid;
    bool isStreamValid = meshService.decompileFromBytecode(secureByteStream, systemSecretToken, verifiedGrid);

    if (isStreamValid) {
        std::cout << "✅ Cryptographic self-check passed. Bytecode integrity confirmed." << std::endl;
        
        // 7. Export the verified data to an open 3D Wavefront mesh file
        std::string filename = "inferred_seamount_output.obj";
        if (meshService.exportToWavefrontObj(verifiedGrid, filename, currentRunMeta)) {
            std::cout << "💾 3D Mesh successfully exported to text workspace: " << filename << std::endl;
        } else {
            std::cerr << "❌ System Error: Mesh text generation writing failed." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "❌ Critical Security Alert: Bytecode stream decryption or signature check failed." << std::endl;
        return 1;
    }

    return 0;
}
