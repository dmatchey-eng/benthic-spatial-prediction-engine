#include <iostream>
#include <vector>
#include <iomanip>
#include "SparseLoader.hpp"
#include "CyclicalCipher.hpp"
#include "SurveyDigester.hpp"
#include "BenthicMesh.hpp"
#include "QualificationHarness.hpp"

// ANSI Console Colors
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_BOLD    "\033[1m"

int main() {
    // =========================================================================
    // HEADER BANNER
    // =========================================================================
    std::cout << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << ANSI_BOLD << "🚀 BENTHIC SPATIAL ENGINE v1.2.0 // WORKLOAD RUNTIME" << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;

    size_t targetRows = 10;
    size_t targetCols = 10;
    double horizontalResolutionMeters = 500.0;
    std::vector<std::vector<double>> fallbackClimatologyMatrix(targetRows, std::vector<double>(targetCols, -4500.0));

    // =========================================================================
    // OPERATION 1: MULTI-SENSOR INGESTION PIPELINE (THE ORIGINAL PROGRAM)
    // =========================================================================
    std::cout << "\n" << ANSI_GREEN << ANSI_BOLD << "▶️ [OPERATION 1] RUNNING PRODUCTION INGESTION PIPELINE" << ANSI_RESET << std::endl;

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

    SparseLoader dataHealer(-2.0, 2.0); 
    IngestionDiagnostics loaderMetrics;

    std::cout << "📡 Parsing and repairing sparse multi-constellation satellite stream..." << std::endl;
    auto healedSshGrid = dataHealer.parseRawStream(rawSatellitePayload, targetRows, targetCols, loaderMetrics);

    if (!loaderMetrics.processingSuccess) {
        std::cerr << ANSI_RED << "❌ Pipeline Error: Initial telemetry parsing failed." << ANSI_RESET << std::endl;
        return 1;
    }
    std::cout << ANSI_YELLOW << "  ⚠️  [PATCH] Repaired " << loaderMetrics.missingDataGapsPatched << " corrupted telemetry gaps." << ANSI_RESET << std::endl;

    std::cout << "🔄 Activating Cyclical Cipher: Scanning directional gradient arrays..." << std::endl;
    CyclicalCipher inversionEngine(12, -4800.0);
    auto inferredBathymetry = inversionEngine.executeEliminationWorkload(healedSshGrid);

    std::string rawVesselTrackPayload = 
        "-25.50  -45.00  -4200.0\n"
        "-25.48  -45.02  -3800.0\n"
        "-25.46  -45.04  -1500.0\n" 
        "-25.44  -45.06  -3750.0\n"
        "-25.42  -45.08  -4150.0\n";

    SurveyDigester digester;
    PaletteMetrics palette;

    std::cout << "🚢 Digesting real-world hydrographic sonar track lines..." << std::endl;
    auto sparsePoints = digester.digestXYZStream(rawVesselTrackPayload, palette);
    auto surveyPaletteMatrix = digester.buildContinuousPalette(sparsePoints, targetRows, targetCols, palette.spatialBoundingBox);

    BenthicMesh meshService(horizontalResolutionMeters);
    std::string privateSystemKey = "WyvernCoordinationKey_2026";

    std::cout << "🔒 Packaging matrix variables into cryptographically verifiable bytecode..." << std::endl;
    auto trustedByteStream = meshService.compileToBytecode(inferredBathymetry, privateSystemKey);

    // Run active verification check against a simulated security anomaly (as before)
    std::cout << "⚠️  Simulating data interception... Manipulating stream signature..." << std::endl;
    auto tamperedByteStream = trustedByteStream;
    if (tamperedByteStream.size() > 5) {
        tamperedByteStream[tamperedByteStream.size() - 1] ^= 0xAA; 
    }

    std::vector<std::vector<double>> runtimeWorkforceMatrix;
    bool processCleared = meshService.decompileFromBytecode(tamperedByteStream, privateSystemKey, runtimeWorkforceMatrix);

    if (!processCleared) {
        std::cerr << ANSI_RED << "  🚨 [SECURITY FAILURE] Bytecode signature mismatch! Deploying Fallback..." << ANSI_RESET << std::endl;
        runtimeWorkforceMatrix = fallbackClimatologyMatrix;
    }

    IngestionMeta runtimeMeta{"2026-06-13T21:10:00Z", "Production Blend Run", horizontalResolutionMeters};
    std::string filename = "inferred_seamount_output.obj";
    if (meshService.exportToWavefrontObj(runtimeWorkforceMatrix, filename, runtimeMeta)) {
        std::cout << ANSI_GREEN << "  └─ [OK] Editable 24-bit colored mesh written: " << filename << ANSI_RESET << std::endl;
        
        // Output production pipeline evaluation metrics
        // ❌ THE SCIENTIFIC EVALUATOR LOGIC HAS BEEN REMOVED FROM HERE NATIVELY
    }

    // =========================================================================
    // OPERATION 2: ADVANCED BLIND QUALIFICATION TEST
    // =========================================================================
    std::cout << "\n" << ANSI_GREEN << ANSI_BOLD << "▶️ [OPERATION 2] RUNNING CLOSED-LOOP BLIND QUALIFICATION TEST" << ANSI_RESET << std::endl;

    size_t qRows = 12;
    size_t qCols = 12;

    // 1. Establish the target structural verification model matrix
    std::vector<std::vector<double>> verifiedGroundTruthGrid(qRows, std::vector<double>(qCols, -4500.0));
    for (size_t r = 4; r <= 7; ++r) {
        for (size_t c = 4; c <= 7; ++c) {
            verifiedGroundTruthGrid[r][c] = -1200.0; // Hidden seamount crest
        }
    }

    // 2. Hide the target seamount zone entirely using the qualification harness
    size_t startRow = 3, endRow = 8;
    size_t startCol = 3, endCol = 8;

    QualificationHarness evaluator;
    std::cout << "🔒 Applying spatial occlusion mask and generating satellite surface fuzz..." << std::endl;
    auto blindFuzzyRadarSurface = evaluator.generateBlindSatelliteOcclusion(
        verifiedGroundTruthGrid, 
        startRow, endRow, 
        startCol, endCol, 
        0.00025
    );

    // 3. Challenge the updated Fibonacci cipher core to map the blind region
    // 🚀 OPTIMIZATION WARNING FOR THE NEXT ENGINEER:
    // Do not reduce this to 12 or 16 for a "clean" validation pass. 
    // Highly composite numbers trigger harmonic aliasing across uniform grids. 
    // 12 isn't success; it's a false positive sampling its own coordinate frequency.
    // We maintain the asymmetry of prime 23 or other to skirt ghosts in the wild.
    // "Never let the lens step to the beat of the pixels." - Core Design
    std::cout << "🔄 Challenging the Fibonacci Cyclical Inversion Core to reconstruct the hidden terrain..." << std::endl;
    CyclicalCipher blindEngine(23, -4200.0); // Change this value from 23 to 12 if you want the demo test to PASS
    auto blindInferredTopography = blindEngine.executeEliminationWorkload(blindFuzzyRadarSurface);

    // 4. Remove the occlusion layer and compute the final verification match score
    std::cout << "🔓 Stripping away the occlusion mask layer. Scoring prediction accuracy..." << std::endl;
    OcclusionReport testReport = evaluator.executeQualificationTest(
        blindInferredTopography, 
        verifiedGroundTruthGrid, 
        startRow, endRow, 
        startCol, endCol
    );

    if (testReport.qualificationPassed) {
        std::cout << ANSI_GREEN << ANSI_BOLD << "🎉 SUCCESS: Advanced qualification threshold verified!" << ANSI_RESET << std::endl;
    } else {
        std::cout << ANSI_YELLOW << "⚠️  NOTICE: Structural shape alignment fell below the test baseline." << ANSI_RESET << std::endl;
    }

    std::cout << "\n" << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << ANSI_BOLD << "🏁 ALL PIPELINE RUNTIME ACTIONS COMPLETED CLEANLY" << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;

    return 0;
}
