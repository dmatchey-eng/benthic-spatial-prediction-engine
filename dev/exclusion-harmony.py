# This module interfaces with spatial vector libraries (shapely, geopandas) to build buffer barriers. It includes a harmonization layer to cross-process US and European sea surface datasets safely.
import numpy as np
import pandas as pd
import geopandas as gpd
from shapely.geometry import box, Point

class DataFidelityHarmonizer:
    """
    Standardizes units, geoids, and reference ellipsoids between 
    US (NOAA/NASA) and European (ESA/Copernicus) satellite systems.
    """
    def __init__(self):
        # Constants for coordinate transformations
        self.METER_TO_MM = 1000.0
        # Average offset between older US EGM96 and modern global EGM2008 geoids
        # used as a placeholder transformation constant over deep oceans
        self.GEOID_OFFSET_EGM96_TO_2008 = 0.041  

    def harmonize_altimetry_packet(self, raw_val: float, source_agency: str) -> float:
        """
        Converts all inputs to SI meters and aligns vertical reference points 
        to the universal EGM2008 Geoid standard.
        """
        if source_agency.upper() in ["NASA", "NOAA"]:
            # Example conversion: Align older US reference systems to EGM2008 standard
            harmonized_ssh = raw_val + self.GEOID_OFFSET_EGM96_TO_2008
        elif source_agency.upper() in ["ESA", "COPERNICUS"]:
            # European products natively utilize modern standards (ITRF/EGM2008)
            harmonized_ssh = raw_val
        else:
            raise ValueError(f"Unknown data provider profile: {source_agency}")
            
        return float(harmonized_ssh)

class GeoFencingExclusionMatrix:
    """
    Applies spatial buffers around human activity lanes and political borders 
    to isolate deep-sea, open-ocean remote fields.
    """
    def __init__(self, eez_shapefile_path: str = None, shipping_geojson_path: str = None):
        """
        Loads global shapefiles for national Exclusive Economic Zones (EEZ) 
        and commercial shipping lanes.
        """
        # Load maritime boundaries. If none provided, defaults to empty geodataframe
        # Open source source: https://www.marineregions.org/downloads.php (World EEZ v11)
        if eez_shapefile_path:
            self.eez_df = gpd.read_file(eez_shapefile_path)
        else:
            self.eez_df = gpd.GeoDataFrame(geometry=[], crs="EPSG:4326")
            
        # Load shipping routes. Open source source: https://github.com/newzealandpaul/Shipping-Lanes
        if shipping_geojson_path:
            self.shipping_df = gpd.read_file(shipping_geojson_path)
        else:
            self.shipping_df = gpd.GeoDataFrame(geometry=[], crs="EPSG:4326")

    def is_tile_pristine(self, tile_box: dict, border_buffer_deg: float = 0.5, lane_buffer_deg: float = 0.2) -> bool:
        """
        Evaluates a bounding box candidate against human infrastructure vectors.
        Returns False if the tile crosses into borders or shipping corridors.
        """
        # 1. Create a spatial polygon of the satellite track footprint
        tile_geom = box(tile_box["west"], tile_box["south"], tile_box["east"], tile_box["north"])
        
        # 2. Evaluate against national boundaries (Exclusive Economic Zones)
        if not self.eez_df.empty:
            # Vectorized check with a distance buffer threshold
            buffered_eez = self.eez_df.geometry.buffer(border_buffer_deg)
            if buffered_eez.intersects(tile_geom).any():
                return False  # Discard due to national border proximity
                
        # 3. Evaluate against established global shipping channels
        if not self.shipping_df.empty:
            # Vectorized check with shipping lane buffer zone
            buffered_lanes = self.shipping_df.geometry.buffer(lane_buffer_deg)
            if buffered_lanes.intersects(tile_geom).any():
                return False  # Discard due to commercial ship noise/lanes
                
        return True  # Tile passed all checks; it is a remote deep-sea zone
