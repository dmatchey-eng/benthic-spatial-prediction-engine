# The script uses established baseline metrics. The North Atlantic is highly mapped, while the South Pacific, Indian Ocean, and Southern Ocean retain the largest "fuzzy" altimetry-dependent zones.
import numpy as np
import pandas as pd

# Global baseline repository tracking current mapped percentages per basin
# Source estimates derived from Nippon Foundation-GEBCO Seabed 2030 metrics
BASIN_REGISTRY = {
    "North_Atlantic": {
        "min_lon": -80.0, "max_lon": 0.0, "min_lat": 0.0, "max_lat": 65.0,
        "mapped_percentage": 48.0, "surface_area_mkm2": 41.5
    },
    "South_Atlantic": {
        "min_lon": -40.0, "max_lon": 20.0, "min_lat": -60.0, "max_lat": 0.0,
        "mapped_percentage": 26.0, "surface_area_mkm2": 40.3
    },
    "North_Pacific": {
        "min_lon": 120.0, "max_lon": -120.0, "min_lat": 0.0, "max_lat": 65.0,
        "mapped_percentage": 32.0, "surface_area_mkm2": 81.5
    },
    "South_Pacific": {
        "min_lon": 150.0, "max_lon": -80.0, "min_lat": -60.0, "max_lat": 0.0,
        "mapped_percentage": 19.0, "surface_area_mkm2": 87.7
    },
    "Indian_Ocean": {
        "min_lon": 20.0, "max_lon": 120.0, "min_lat": -60.0, "max_lat": 25.0,
        "mapped_percentage": 22.0, "surface_area_mkm2": 70.5
    },
    "Southern_Ocean": {
        "min_lon": -180.0, "max_lon": 180.0, "min_lat": -90.0, "max_lat": -60.0,
        "mapped_percentage": 16.0, "surface_area_mkm2": 20.3
    }
}
