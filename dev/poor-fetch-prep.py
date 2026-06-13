#This Python script calculates the inverse sampling distribution and generates the exact bounding boxes needed to fetch public satellite radar data (e.g., from the NOAA Open Data Dissemination program or NASA Earthdata APIs).
def calculate_inverse_fetch_allocation(total_target_samples: int = 5000) -> pd.DataFrame:
    """
    Computes coordinate tile allocation per basin.
    Allocation is directly proportional to size (surface area) 
    BUT inversely proportional to the amount of already collected data (mapped_percentage).
    """
    records = []
    total_inverse_weight = 0
    
    # First pass: Calculate individual basin data-poverty weights
    for name, metrics in BASIN_REGISTRY.items():
        # Inverse mapping factor: higher value means more unmapped territory
        unmapped_factor = 100.0 - metrics["mapped_percentage"]
        
        # Combined weight scales up with physical size, scales up with fuzziness
        raw_weight = metrics["surface_area_mkm2"] * unmapped_factor
        total_inverse_weight += raw_weight
        
        records.append({
            "basin_name": name,
            "bounds": (metrics["min_lon"], metrics["max_lon"], metrics["min_lat"], metrics["max_lat"]),
            "raw_weight": raw_weight
        })
        
    df = pd.DataFrame(records)
    
    # Second pass: Normalize weights into absolute image tile fetch targets
    df["sample_allocation"] = ((df["raw_weight"] / total_inverse_weight) * total_target_samples).astype(int)
    
    return df

def generate_random_spatial_mesh(bounds: tuple, sample_count: int, grid_size_deg: float = 1.0) -> list:
    """
    Generates high-variability coordinate boxes inside a target basin boundary.
    Handles longitudinal wrapping across the International Date Line.
    """
    min_lon, max_lon, min_lat, max_lat = bounds
    coordinate_tiles = []
    
    # Adjust for Pacific wrapping parameters if boundaries cross 180/-180
    if min_lon > max_lon: 
        lon_pool = np.concatenate([np.linspace(min_lon, 180, 500), np.linspace(-180, max_lon, 500)])
    else:
        lon_pool = np.linspace(min_lon, max_lon, 1000)
        
    lat_pool = np.linspace(min_lat, max_lat, 1000)
    
    for _ in range(sample_count):
        # Pick highly distributed random anchor points
        center_lon = np.random.choice(lon_pool)
        center_lat = np.random.choice(lat_pool)
        
        # Build standard bounding box representation for API querying
        tile = {
            "west": center_lon - (grid_size_deg / 2),
            "east": center_lon + (grid_size_deg / 2),
            "south": center_lat - (grid_size_deg / 2),
            "north": center_lat + (grid_size_deg / 2)
        }
        coordinate_tiles.append(tile)
        
    return coordinate_tiles

# --- Pipeline Execution ---
if __name__ == "__main__":
    np.random.seed(101)
    
    # 1. Establish the inverse target quota across global waters
    TOTAL_IMAGES_TO_FETCH = 10000 
    allocation_matrix = calculate_inverse_fetch_allocation(TOTAL_IMAGES_TO_FETCH)
    
    print("=== INVERSE DATA FETCHING ALLOCATION MATRIX ===")
    print(allocation_matrix[["basin_name", "sample_allocation"]].to_string(index=False))
    
    # 2. Loop through each basin to extract targeted API query footprints
    api_query_manifest = {}
    
    for idx, row in allocation_matrix.iterrows():
        basin = row["basin_name"]
        quota = row["sample_allocation"]
        boundaries = row["bounds"]
        
        # Generate spatial footprints optimized for high variance
        query_tiles = generate_random_spatial_mesh(boundaries, quota, grid_size_deg=0.5)
        api_query_manifest[basin] = query_tiles
        
    print(f"\n✅ Manifest constructed. Ready to pass {TOTAL_IMAGES_TO_FETCH} query boxes to the satellite data server.")
