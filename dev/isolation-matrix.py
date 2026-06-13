# The isolation matrix categorizes unmapped zones by combining their topographical volatility (derived from the spatial gradients) and their uncertainty profiles. This groups regions with similar data distortion together, allowing them to undergo specialized mathematical filtration later.

def generate_priority_isolation_matrix(df: pd.DataFrame, bins: int = 5) -> pd.DataFrame:
    """
    Segments non-ground-truth (fuzzy) zones into an isolation matrix 
    based on gravity gradient magnitude and surface curvature variance.
    """
    # Isolate unmapped sections (Altimetry-only data points)
    fuzzy_mask = df['is_ground_truth'] == 0
    fuzzy_df = df[fuzzy_mask].copy()
    
    # Calculate total spatial gradient magnitude
    fuzzy_df['gradient_magnitude'] = np.sqrt(fuzzy_df['gradient_x']**2 + fuzzy_df['gradient_y']**2)
    
    # Quantize gradient magnitude into priority tiers (Low to Extreme Volatility)
    fuzzy_df['gradient_tier'] = pd.qcut(fuzzy_df['gradient_magnitude'], q=bins, labels=False, duplicates='drop')
    
    # Quantize local curvature variance to isolate structural boundaries
    fuzzy_df['curvature_tier'] = pd.qcut(fuzzy_df['surface_curvature'].abs(), q=bins, labels=False, duplicates='drop')
    
    # Construct Unique Priority Keys for the pass filtration router
    # Format: Tier_[GradientMagnitude][SurfaceCurvature]
    fuzzy_df['isolation_priority_key'] = (
        "Tier_" + 
        fuzzy_df['gradient_tier'].astype(str) + 
        "_" + 
        fuzzy_df['curvature_tier'].astype(str)
    )
    
    return fuzzy_df
