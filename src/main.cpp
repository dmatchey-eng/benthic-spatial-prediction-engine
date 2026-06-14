#include <iostream>
#include <vector>
#include <iomanip>
#include "SparseLoader.hpp"
#include "CyclicalCipher.hpp"
#include "SurveyDigester.hpp"
#include "BenthicMesh.hpp"

int main() {
    std::cout << "🚀 Initializing Full-Pipeline Benthic Spatial Prediction Engine..." << std::endl;

    // =========================================================================
    // STEP 1: SATELLITE TELEMENTRY INGESTION & HEALING (SPARSE LOAD)
    // =========================================================================
    // Simulate a realistic 10x10 satellite text grid payload.
    // Contains sensor noise, surface elevation bulges, and gaps marked as "NaN" or "-9999".
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

    SparseLoader dataHealer(-2.0, 2.0); 
    IngestionDiagnostics loaderMetrics;

    std::cout << "📡 Parsing and repairing sparse multi-constellation satellite stream..." << std::endl;
    auto healedSshGrid = dataHealer.parseRawStream(rawSatellitePayload, targetRows, targetCols, loaderMetrics);

    if (!loaderMetrics.processingSuccess) {
        std::cerr << "❌ Pipeline Error: Initial telemetry parsing failed." << std::endl;
        return 1;
    }

    std::cout << "  └─ [OK] Patched " << loaderMetrics.missingDataGapsPatched << " corrupted telemetry gaps." << std::endl;

    // =========================================================================
    // STEP 2: CYCLICAL FOCUS MATRIX ANALYSIS (CIPHER CORE EVOLUTION)
    // =========================================================================
    // Executes an adaptive 12-pass rotational matrix scan to calculate structural anisotropy.
    std::cout << "🔄 Activating Adaptive Cyclical Cipher: Scanning directional gradient arrays..." << std::endl;
    CyclicalCipher inversionEngine(12, -4800.0);
    auto inferredBathymetry = inversionEngine.executeEliminationWorkload(healedSshGrid);

    // =========================================================================
    // STEP 3: GROUND TRUTH DIGESTION (SURVEY DIGESTER MEDIA)
    // =========================================================================
    // Simulate an open-source XYZ hydrographic log from an active oceanographic research vessel.
    // Format layout constraints: [Longitude] [Latitude] [True Measured Sonar Sounding Depth]
    std::string rawVesselTrackPayload = 
        "-25.50  -45.00  -4200.0\n"
        "-25.48  -45.02  -3800.0\n"
        "-25.46  -45.04  -1500.0\n" // Research vessel hits the seamount ridge crest
        "-25.44  -45.06  -3750.0\n"
        "-25.42  -45.08  -4150.0\n";

    SurveyDigester digester;
    PaletteMetrics palette;

    std::cout << "🚢 Digesting real-world hydrographic sonar track lines..." << std::endl;
    auto sparsePoints = digester.digestXYZStream(rawVesselTrackPayload, palette);
    auto surveyPaletteMatrix = digester.buildContinuousPalette(
        sparsePoints, 
        targetRows, 
        targetCols, 
        palette.spatialBoundingBox
    );
    
    std::cout << "  └─ [OK] Extested Ground-Truth Points: " << palette.totalPointsParsed << " entries parsed." << std::endl;

    // =========================================================================
    // STEP 4: DATA MODEL PACKAGING & SECURE CRYPTOGRAPHIC VERIFICATION
    // =========================================================================
    double horizontalResolutionMeters = 500.0;
    BenthicMesh meshService(horizontalResolutionMeters);

    IngestionMeta currentRunMeta{
        "2026-06-13T20:30:00Z", // Timestamp metadata token
        "Multi-Constellation Satellite Mesh + Physical Hydrographic Tracks",
        horizontalResolutionMeters
    };

    // Compile the final topography matrix array into signed binary bytecode bytes
    std::string systemSecretToken = "WyvernCoordinationKey_2026";
    std::cout << "🔒 Packaging matrix variables into cryptographically verifiable bytecode..." << std::endl;
    std::vector<uint8_t> secureByteStream = meshService.compileToBytecode(inferredBathymetry, systemSecretToken);
    std::cout << "  └─ [OK] Packed Immutable Byte Stream Size: " << secureByteStream.size() << " bytes." << std::endl;

    // Perform an internal automated cryptographic self-check
    std::vector<std::vector<double>> verifiedGrid;
    bool isStreamValid = meshService.decompileFromBytecode(secureByteStream, systemSecretToken, verifiedGrid);

    if (isStreamValid) {
        std::cout << "✅ Cryptographic integrity pass successful. Token verified." << std::endl;
        
        // =========================================================================
        // STEP 5: EXPORT OPEN GEOMETRY MESH (.OBJ)
        // =========================================================================
        std::string filename = "inferred_seamount_output.obj";
        if (meshService.exportToWavefrontObj(verifiedGrid, filename, currentRunMeta)) {
            std::cout << "💾 Editable 3D topological mesh successfully written to workspace: " << filename << std::endl;
            
            // Output pipeline peak predictions
            std::cout << "\n=== PIPELINE EXECUTION SUMMARY ===" << std::endl;
            std::cout << "🔮 Predicted Topography Peak Relief : " << std::fixed << std::setprecision(1) << verifiedGrid[2][2] << " meters." << std::endl;
            std::cout << "📡 Ground-Truth Target Sonar Sounding: " << surveyPaletteMatrix[2][2] << " meters." << std::endl;
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
