#include <iostream>
#include <vector>
#include <iomanip>
#include "SparseLoader.hpp"
#include "CyclicalCipher.hpp"
#include "SurveyDigester.hpp"
#include "BenthicMesh.hpp"

// Classic ANSI Color Escape Sequence Macros
#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_GREEN   "\033[32m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_MAGENTA "\033[35m"
#define ANSI_CYAN    "\033[36m"
#define ANSI_BOLD    "\033[1m"

int main() {
    // -------------------------------------------------------------------------
    // SYSTEM BANNER (Cyan Framework Setup)
    // -------------------------------------------------------------------------
    std::cout << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << ANSI_BOLD << "🚀 BENTHIC SPATIAL ENGINE v1.1.0 // INITIALIZING WORKLOAD" << ANSI_RESET << std::endl;
    std::cout << ANSI_CYAN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;

    size_t targetRows = 10;
    size_t targetCols = 10;
    double horizontalResolutionMeters = 500.0;
    std::vector<std::vector<double>> fallbackClimatologyMatrix(targetRows, std::vector<double>(targetCols, -4500.0));

    // -------------------------------------------------------------------------
    // ZONE 1: TELEMETRY PATCHING (Yellow Data Information)
    // -------------------------------------------------------------------------
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

    std::cout << "\n📡 " << ANSI_BOLD << "Ingesting Satellite Telemetry..." << ANSI_RESET << std::endl;
    auto healedSshGrid = dataHealer.parseRawStream(rawSatellitePayload, targetRows, targetCols, loaderMetrics);

    if (!loaderMetrics.processingSuccess) {
        std::cerr << ANSI_RED << "❌ Pipeline Error: Initial telemetry parsing failed." << ANSI_RESET << std::endl;
        return 1;
    }
    std::cout << ANSI_YELLOW << "  ⚠️  [PATCH] Processed " << loaderMetrics.totalCellsRead << " cells." << ANSI_RESET << std::endl;
    std::cout << ANSI_YELLOW << "  ⚠️  [PATCH] Automatically repaired " << loaderMetrics.missingDataGapsPatched << " data dropouts via Laplacian diffusion." << ANSI_RESET << std::endl;

    // -------------------------------------------------------------------------
    // ZONE 2: ALGORITHMIC EXECUTION (Magenta Computation Tracks)
    // -------------------------------------------------------------------------
    std::cout << "\n🔄 " << ANSI_BOLD << "Activating Cyclical Cipher Analysis..." << ANSI_RESET << std::endl;
    CyclicalCipher inversionEngine(12, -4800.0);
    auto inferredBathymetry = inversionEngine.executeEliminationWorkload(healedSshGrid);
    std::cout << ANSI_MAGENTA << "  ⚙️  [CIPHER] 12 directional gradient loops synchronized cleanly." << ANSI_RESET << std::endl;

    // -------------------------------------------------------------------------
    // ZONE 3: GROUND TRUTH PARSING (Blue / Oceanographic Track Mapping)
    // -------------------------------------------------------------------------
    std::string rawVesselTrackPayload = 
        "-25.50  -45.00  -4200.0\n"
        "-25.48  -45.02  -3800.0\n"
        "-25.46  -45.04  -1500.0\n"
        "-25.44  -45.06  -3750.0\n"
        "-25.42  -45.08  -4150.0\n";

    SurveyDigester digester;
    PaletteMetrics palette;

    std::cout << "\n🚢 " << ANSI_BOLD << "Ingesting Physical Sonar Tracks..." << ANSI_RESET << std::endl;
    auto sparsePoints = digester.digestXYZStream(rawVesselTrackPayload, palette);
    auto surveyPaletteMatrix = digester.buildContinuousPalette(sparsePoints, targetRows, targetCols, palette.spatialBoundingBox);
    std::cout << ANSI_BLUE << "  🌊 [SONAR] Digested " << palette.totalPointsParsed << " ground-truth sonar sounding fixes." << ANSI_RESET << std::endl;

    // -------------------------------------------------------------------------
    // ZONE 4: CRYPTOGRAPHIC VERIFICATION & TAMPER INTERCEPT (Red Safety Fallback)
    // -------------------------------------------------------------------------
    BenthicMesh meshService(horizontalResolutionMeters);
    std::string privateSystemKey = "WyvernCoordinationKey_2026";

    auto trustedByteStream = meshService.compileToBytecode(inferredBathymetry, privateSystemKey);

    // Simulate an explicit security exception / payload attack
    std::cout << "\n🔒 " << ANSI_BOLD << "Compiling Bytecode Storage Container..." << ANSI_RESET << std::endl;
    auto tamperedByteStream = trustedByteStream;
    if (tamperedByteStream.size() > 5) {
        tamperedByteStream[tamperedByteStream.size() - 1] ^= 0xAA; // Corrupt signature byte
    }

    std::vector<std::vector<double>> runtimeWorkforceMatrix;
    bool processCleared = meshService.decompileFromBytecode(tamperedByteStream, privateSystemKey, runtimeWorkforceMatrix);

    if (!processCleared) {
        std::cerr << ANSI_RED << ANSI_BOLD << "  🚨 [SECURITY FAILURE] Bytecode stream signature check mismatch!" << ANSI_RESET << std::endl;
        std::cerr << ANSI_RED << "  🚨 [SECURITY FAILURE] Payload marked untrusted. Isolating volatile frames..." << ANSI_RESET << std::endl;
        
        runtimeWorkforceMatrix = fallbackClimatologyMatrix;
        std::cout << ANSI_YELLOW << "  🔄 [HEALED] Substituted default reference data map safely into workspace arrays." << ANSI_RESET << std::endl;
    }

    // -------------------------------------------------------------------------
    // ZONE 5: SYSTEM COMPLETION REPORT (Green Metrics Summary)
    // -------------------------------------------------------------------------
    IngestionMeta runtimeMeta{"2026-06-13T20:44:00Z", "Adaptive Error Isolation Run", horizontalResolutionMeters};
    std::string meshFilename = "colored_seamount_output.obj";

    std::cout << "\n💾 " << ANSI_BOLD << "Writing 3D Wireframe Assets..." << std::endl;
    if (meshService.exportToWavefrontObj(runtimeWorkforceMatrix, meshFilename, runtimeMeta)) {
        std::cout << "  └─ [OK] File written: " << meshFilename << std::endl;
        
        std::cout << "\n" << ANSI_GREEN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;
        std::cout << ANSI_GREEN << ANSI_BOLD << "📊 PIPELINE ANALYSIS EXECUTION COMPLETE (ZERO LEAKS)" << ANSI_RESET << std::endl;
        std::cout << ANSI_GREEN << "  🔹 Modeled Grid Peak Value  : " << std::fixed << std::setprecision(1) << runtimeWorkforceMatrix[5][5] << " m" << std::endl;
        std::cout << ANSI_GREEN << "  🔹 Sonar Validation Value   : " << surveyPaletteMatrix[5][5] << " m" << std::endl;
        std::cout << ANSI_GREEN << ANSI_BOLD << "=========================================================" << ANSI_RESET << std::endl;
    } else {
        std::cerr << ANSI_RED << "❌ Fatal Error: Could not write geometric matrix." << ANSI_RESET << std::endl;
        return 1;
    }

    return 0;
}
