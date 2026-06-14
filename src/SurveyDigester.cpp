#include "SurveyDigester.hpp"
#include <sstream>
#include <cmath>
#include <limits>
#include <algorithm>

std::vector<SoundingPoint> SurveyDigester::digestXYZStream(
    const std::string& rawDataPayload, 
    PaletteMetrics& outMetrics) 
{
    std::vector<SoundingPoint> collectedPoints;
    std::stringstream ss(rawDataPayload);
    std::string line;

    // Initialize tracking limits
    double minLon = std::numeric_limits<double>::max();
    double maxLon = -std::numeric_limits<double>::max();
    double minLat = std::numeric_limits<double>::max();
    double maxLat = -std::numeric_limits<double>::max();

    // Loop through lines of the data payload
    while (std::getline(ss, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip comments

        std::stringstream lineStream(line);
        double lon, lat, depth;
        
        // Extract standard structural XYZ coordinates
        if (lineStream >> lon >> lat >> depth) {
            SoundingPoint pt{lon, lat, depth};
            collectedPoints.push_back(pt);

            // Update global spatial constraints
            if (lon < minLon) minLon = lon;
            if (lon > maxLon) maxLon = lon;
            if (lat < minLat) minLat = lat;
            if (lat > maxLat) maxLat = lat;

            if (depth < outMetrics.absoluteMinDepth) outMetrics.absoluteMinDepth = depth;
            if (depth > outMetrics.absoluteMaxDepth) outMetrics.absoluteMaxDepth = depth;
        }
    }

    outMetrics.totalPointsParsed = collectedPoints.size();
    outMetrics.spatialBoundingBox[0] = minLon;
    outMetrics.spatialBoundingBox[1] = maxLon;
    outMetrics.spatialBoundingBox[2] = minLat;
    outMetrics.spatialBoundingBox[3] = maxLat;

    return collectedPoints;
}

std::vector<std::vector<double>> SurveyDigester::buildContinuousPalette(
    const std::vector<SoundingPoint>& soundings,
    size_t gridRows,
    size_t gridCols,
    const double bbox[4]) 
{
    std::vector<std::vector<double>> paletteGrid(gridRows, std::vector<double>(gridCols, -4500.0)); // Default to deep abyssal floor
    
    if (soundings.empty()) return paletteGrid;

    double minLon = bbox[0];
    double maxLon = bbox[1];
    double minLat = bbox[2];
    double maxLat = bbox[3];

    double lonStep = (maxLon - minLon) / (gridCols > 1 ? gridCols - 1 : 1);
    double latStep = (maxLat - minLat) / (gridRows > 1 ? gridRows - 1 : 1);

    // Apply distance mapping across all grid intersections
    for (size_t r = 0; r < gridRows; ++r) {
        double currentLat = minLat + (r * latStep);
        for (size_t c = 0; c < gridCols; ++c) {
            double currentLon = minLon + (c * lonStep);

            double weightedSum = 0.0;
            double weightTotal = 0.0;
            bool exactMatchFound = false;

            // Look through sparse tracks to compute local environmental weights
            for (const auto& sounding : soundings) {
                double distance = std::sqrt(std::pow(sounding.longitude - currentLon, 2) + 
                                            std::pow(sounding.latitude - currentLat, 2));
                
                // Edge case: point sits directly on an actual sonar sounding coordinate
                if (distance < 1e-6) {
                    paletteGrid[r][c] = sounding.measuredDepth;
                    exactMatchFound = true;
                    break;
                }

                // Inverse Distance Weighting parameter (IDW 1/d²)
                double weight = 1.0 / (distance * distance);
                weightedSum += sounding.measuredDepth * weight;
                weightTotal += weight;
            }

            if (!exactMatchFound && weightTotal > 0.0) {
                paletteGrid[r][c] = weightedSum / weightTotal;
            }
        }
    }

    return paletteGrid;
}
