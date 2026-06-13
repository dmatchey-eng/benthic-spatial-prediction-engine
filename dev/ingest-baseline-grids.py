import numpy as np
import pandas as pd
import xarray as xr
from scipy.ndimage import gaussian_filter, laplace

def initialize_spatial_database(filepath: str) -> pd.DataFrame:
    """
    Ingests NetCDF baseline grids and extracts SSH anomalies, 
    source flag masks, and spatial coordinates.
    """
    # Load dataset using xarray
    ds = xr.open_dataset(filepath)
    
    # Extract variables (assuming standard GEBCO/SRTM variable names)
    lon = ds['lon'].values
    lat = ds['lat'].values
    ssh_anomaly = ds['ssh_anomaly'].values  # Sea surface height anomalies
    source_flag = ds['source_flag'].values   # 0 = Altimetry (Fuzzy), 1 = Sonar (Ground Truth)
    
    # Generate coordinate grids
    lon_grid, lat_grid = np.meshgrid(lon, lat)
    
    # Flatten arrays for tabular database integration
    df = pd.DataFrame({
        'longitude': lon_grid.flatten(),
        'latitude': lat_grid.flatten(),
        'ssh_anomaly': ssh_anomaly.flatten(),
        'is_ground_truth': source_flag.flatten()
    })
    
    # Calculate Wyvern Array spatial gradients (1st and 2nd derivatives)
    # This represents the raw surface topography deformation vectors
    ssh_matrix = ssh_anomaly
    grad_y, grad_x = np.gradient(ssh_matrix)
    laplacian = laplace(ssh_matrix)
    
    df['gradient_x'] = grad_x.flatten()
    df['gradient_y'] = grad_y.flatten()
    df['surface_curvature'] = laplacian.flatten()
    
    return df
