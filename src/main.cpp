#include <iostream>
#include <vector>
#include "SparseLoader.hpp"
#include "CyclicalCipher.hpp"
#include "BenthicMesh.hpp"

int main() {
    std::cout << "🚀 Initializing Full-Pipeline Benthic Wyvern Engine..." << std::endl;

    // 1. Simulate a realistic, noisy 10x10 satellite text grid payload.
    // This string replicates an unmapped ocean basin track containing standard sensor noise, 
    // real surface elevation bulges, and corrupted data gaps marked as "NaN" and "-9999".
    std::string rawSatellitePayload = 
        "0.12  0.15   NaN   0.18  0.11  0.09  0.12  0.14   NaN   0.11 "
        "0.14  0.45  0.45   0.42  0.13  0.12  0.45  0.42  0.41   0.10 "
        "0.11  0.45   NaN   0.45  0.12  0.14  0.45 -9999  0.45   0.12 "
        "0.15  0.42  0.45   0.41  0.15  0.11  0.42  0.45  0.42   0.13 "
        "0.10  0.12  0.14   0.11  0.09  0.08  0.12  0.13  0.11   0.09 "
        "0.11  0.14  0.15   0.12  0.10  0.11  0.15  0.14  0.12   0.11 "
        "0.13  0.45  0.42   0.45  0.14  0.12  0.45  0.45  0.41   0.14 "
        "0.12  0.45  0.45   0.42  0.11  0.15  0.42   NaN  0.45   0.12 "
        "0.14  0.41  0.42   0.45  0.13  0.10  0.45  0.42  0.42   0.15 "
        "0.09  0.11  0.12   0.10  0.08  0.09  0.11  0.12  0.10   0.08 ";

    size_t targetRows = 10;
    size_t targetCols = 10;

    // 2. Step 1: Initialize the Sparse Loader to sanitize and heal incoming tracking tracks.
    // Rejects telemetry outliers outside a safe satellite altitude boundary.
    SparseLoader dataHealer(-2.0, 2.0); 
    IngestionDiagnostics loaderMetrics;

    std::cout << "📡 Parsing and repairing sparse multi-constellation satellite stream..." << std::endl;
    auto healedSshGrid = dataHealer.parseRawStream(rawSatellitePayload, targetRows, targetCols, loaderMetrics);

    if (!loaderMetrics.processingSuccess) {
        std::cerr << "❌ Pipeline Error: Initial telemetry parsing failed." << std::endl;
        return 1;
    }

    std::cout << "  └─ [OK] Parsed " << loaderMetrics.totalCellsRead << " cells." << std::endl;
    std::cout << "  └─ [OK] Patched " << loaderMetrics.missingDataGapsPatched << " corrupted telemetry gaps." << std::endl;
    std::cout << "  └─ [OK] Extracted Baseline SSH Mean: " << loaderMetrics.rawDataMeanValue << " meters." << std::endl;

    // 3. Step 2: Run the Cyclical Lens Elimination core workload on the healed data.
    // Sets up a 12-pass rotational matrix scan to isolate static gravity signatures from ocean noise.
    std::cout << "🔄 Activating Cyclical Cipher: Scanning directional gradient arrays..." << std::endl;
    CyclicalCipher inversionEngine(12, -4800.0);
    auto inferredBathymetry = inversionEngine.executeEliminationWorkload(healedSshGrid);

    // 4. Step 3: Initialize the cryptographic packaging and visualization service.
    double horizontalResolutionMeters = 500.0;
    BenthicMesh meshService(horizontalResolutionMeters);

    IngestionMeta currentRunMeta{
        "2026-06-13T20:15:00Z", // Timestamp metadata token
        "NOAA-Jason3 / ESA-Sentinel6 Harmonized Ingestion Matrix",
        horizontalResolutionMeters
    };

    // 5. Step 4: Compile the inferred 2D terrain data into a signed binary bytecode vector stream.
    std::string systemSecretToken = "WyvernCoordinationKey_2026";
    std::cout << "🔒 Packaging matrix variables into cryptographically verifiable bytecode..." << std::endl;
    std::vector<uint8_t> secureByteStream = meshService.compileToBytecode(inferredBathymetry, systemSecretToken);
    std::cout << "  └─ [OK] Packed Immutable Stream Size: " << secureByteStream.size() << " bytes." << std::endl;

    // 6. Step 5: Execute an internal automated cryptographic integrity check.
    std::vector<std::vector<double>> verifiedGrid;
    bool isStreamValid = meshService.decompileFromBytecode(secureByteStream, systemSecretToken, verifiedGrid);

    if (isStreamValid) {
        std::cout << "✅ Cryptographic self-check complete. Signature verified." << std::endl;
        
        // 7. Step 6: Export the validated database array into a clean 3D Wavefront mesh file.
        std::string filename = "inferred_seamount_output.obj";
        if (meshService.exportToWavefrontObj(verifiedGrid, filename, currentRunMeta)) {
            std::cout << "💾 Editable 3D topological mesh successfully written to workspace: " << filename << std::endl;
            std::cout << "🎉 Full-pipeline execution wrapped up flawlessly." << std::endl;
        } else {
            std::cerr << "❌ System Error: Could not write output mesh file geometry." << std::endl;
            return 1;
        }
    } else {
        std::cerr << "❌ Critical Security Alert: Bytecode payload validation failed or signature is unauthenticated!" << std::endl;
        return 1;
    }

    return 0;
}
