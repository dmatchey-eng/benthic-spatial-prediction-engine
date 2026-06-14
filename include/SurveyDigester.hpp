#ifndef SURVEY_DIGESTER_HPP
#define SURVEY_DIGESTER_HPP

#include <vector>
#include <string>

struct SoundingPoint {
    double longitude;
    double latitude;
    double measuredDepth; // Actual true ground-truth sounding (meters)
};

struct PaletteMetrics {
    size_t totalPointsParsed = 0;
    double absoluteMinDepth = 0.0;
    double absoluteMaxDepth = -99999.0;
    double spatialBoundingBox[4]; // [Min_Lon, Max_Lon, Min_Lat, Max_Lat]
};

class SurveyDigester {
public:
    SurveyDigester() = default;

    // Parses open-source XYZ hydrographic track payloads
    std::vector<SoundingPoint> digestXYZStream(
        const std::string& rawDataPayload, 
        PaletteMetrics& outMetrics
    );

    // Continuous palette matrix: maps sparse, unstructured sounding lines 
    // onto our uniform 2D satellite grid layer using inverse distance weighting (IDW)
    std::vector<std::vector<double>> buildContinuousPalette(
        const std::vector<SoundingPoint>& soundings,
        size_t gridRows,
        size_t gridCols,
        const double bbox[4]
    );
};

#endif // SURVEY_DIGESTER_HPP
