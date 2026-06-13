# This production module hooks directly into your CyclicalFocusInversionEngine to transform the calculated 2D elevation matrix into a highly optimized, light 3D model file.
import json
import datetime
import numpy as np

class BenthicMeshExporter:
    """
    Transforms 2D bathymetric matrices into optimized, editable 3D meshes (OBJ format)
    with embedded metadata while managing file size through coordinate downsampling.
    """
    def __init__(self, spatial_resolution_meters: float = 500.0):
        self.spatial_res = spatial_resolution_meters

    def _generate_metadata_header(self, grid_shape: tuple, topo_matrix: np.ndarray, source_meta: dict) -> str:
        """
        Creates a comprehensive JSON metadata header to embed into the mesh file comments.
        Tracks baseline data provenance and final inferred topographic dimensions.
        """
        metadata = {
            "export_timestamp_utc": datetime.datetime.utcnow().isoformat(),
            "data_provenance": {
                "input_satellites": source_meta.get("satellites", ["NOAA/NASA", "ESA/Copernicus"]),
                "initial_grid_dimensions": f"{grid_shape[0]}x{grid_shape[1]} cells",
                "horizontal_resolution_meters": self.spatial_res
            },
            "topographical_inferred_metrics": {
                "max_calculated_depth_meters": float(np.min(topo_matrix)),
                "min_calculated_depth_meters": float(np.max(topo_matrix)),
                "estimated_seamount_relief_meters": float(np.max(topo_matrix) - np.min(topo_matrix)),
                "variance_signature": float(np.var(topo_matrix))
            }
        }
        
        # Format as string with individual comment markers (#) for OBJ specification safety
        json_str = json.dumps(metadata, indent=2)
        commented_header = "\n".join([f"# {line}" for line in json_str.split("\n")])
        return f"# =========================================================\n# BENTHIC ANALYSIS METADATA HEADER\n{commented_header}\n# =========================================================\n\n"

    def export_to_wavefront_obj(self, topo_matrix: np.ndarray, output_filepath: str, source_meta: dict, downsample_factor: int = 1) -> str:
        """
        Generates an editable, lightweight .obj mesh. 
        Downsample_factor allows scaling file size dynamically (e.g., 2 skips every second vertex).
        """
        # Apply downsampling filter to keep file size lightweight
        matrix = topo_matrix[::downsample_factor, ::downsample_factor]
        rows, cols = matrix.shape
        
        vertices = []
        faces = []
        
        # 1. Generate Vertices (V) - Scaling coordinates relative to spatial resolution
        for r in range(rows):
            for c in range(cols):
                x = c * self.spatial_res
                y = r * self.spatial_res
                z = matrix[r, c] # Z-axis explicitly maps the deep elevation model
                vertices.append(f"v {x:.2f} {y:.2f} {z:.2f}\n")
                
        # 2. Generate Quad Faces (F) - Linking vertices logically for 3D polygon editors
        for r in range(rows - 1):
            for c in range(cols - 1):
                # Calculate index positions of vertices mapping out a 4-point polygon square
                v1 = r * cols + c + 1
                v2 = r * cols + (c + 1) + 1
                v3 = (r + 1) * cols + (c + 1) + 1
                v4 = (r + 1) * cols + c + 1
                faces.append(f"f {v1} {v2} {v3} {v4}\n")
                
        # 3. Compile everything together with metadata
        header = self._generate_metadata_header(topo_matrix.shape, topo_matrix, source_meta)
        
        with open(output_filepath, 'w') as obj_file:
            obj_file.write(header)
            obj_file.writelines(vertices)
            obj_file.write("\n")
            obj_file.writelines(faces)
            
        print(f"💾 Optimized 3D editable mesh successfully exported to: {output_filepath}")
        print(f"📊 Total Geometry Complexity: {len(vertices)} vertices, {len(faces)} quad faces.")
        return output_filepath
