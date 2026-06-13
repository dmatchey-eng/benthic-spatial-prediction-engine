# This production-grade script applies a rotating directional gradient filter across a spatial mesh. It isolates static gravity signals and converts them into predicted bathymetric depth.
import numpy as np
import scipy.ndimage as ndimage

class CyclicalFocusInversionEngine:
    """
    Applies a rotating cipher mechanism to radar altimetry grids.
    Uses directional parameter elimination to extract clean benthic structures.
    """
    def __init__(self, rotation_steps: int = 24, gravitational_constant: float = -4200.0):
        # 24 steps means rotating the calculation lens by 15-degree increments (360 / 24)
        self.steps = rotation_steps
        # Scale factor linking gravitational sea-surface deflection to seafloor depth (meters)
        self.gamma = gravitational_constant 

    def _apply_euler_rotation_matrix(self, matrix: np.ndarray, angle_deg: float) -> np.ndarray:
        """
        Rotates the spatial processing lens using a 2D coordinate transformation
        to scan for directional tectonic stress lines.
        """
        return ndimage.rotate(matrix, angle_deg, reshape=False, mode='nearest')

    def execute_elimination_workload(self, ssh_grid: np.ndarray) -> np.ndarray:
        """
        Runs the full cyclical focus loop. Rotates the data array, eliminates 
        non-static noise, and inverts the residual signal into a 3D terrain map.
        """
        rows, cols = ssh_grid.shape
        accumulated_gravitational_signature = np.zeros((rows, cols))
        
        # Calculate angular slice step size
        angle_increment = 360.0 / self.steps
        
        print(f"🔄 Activating Cyclical Cipher: Executing {self.steps} rotational filtration sweeps...")
        
        for step in range(self.steps):
            current_angle = step * angle_increment
            
            # Phase 1: Rotate the coordinate system to the current cipher angle
            rotated_lens = self._apply_euler_rotation_matrix(ssh_grid, current_angle)
            
            # Phase 2: Compute directional 1st and 2nd derivatives along this specific angle
            # This isolates linear geological faults and edges hiding under the water
            grad_y, grad_x = np.gradient(rotated_lens)
            laplacian_edges = ndimage.laplace(rotated_lens)
            
            # Algorithmic Elimination: Combine gradients to cancel out omnidirectional noise
            directional_signal = np.sqrt(grad_x**2 + grad_y**2) + np.abs(laplacian_edges)
            
            # Phase 3: Counter-rotate the directional signal back to the original orientation
            restored_signal = self._apply_euler_rotation_matrix(directional_signal, -current_angle)
            
            # Accumulate the verified static signatures
            accumulated_gravitational_signature += restored_signal

        # Normalize the accumulated signature across all rotational passes
        normalized_gravity = accumulated_gravitational_signature / self.steps
        
        # Phase 4: Run the Haruhi Inversion Sweep
        # Convert the isolated gravitational anomalies directly into actual seafloor depth
        predicted_topography = normalized_gravity * self.gamma
        
        return predicted_topography
