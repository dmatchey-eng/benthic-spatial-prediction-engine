# This block simulates how the system ingests data, updates spatial states, and routes tasks to different computational nodes without discarding any geographic zones.
if __name__ == "__main__":
    # Initialize the spatial workload engine
    orchestrator = WorkloadOrchestrator(target_resolution=4)
    
    # 1. Create a simulated harmonized multi-satellite data stream from the Wyvern Array
    np.random.seed(42)
    stream_size = 5000
    wyvern_output_stream = pd.DataFrame({
        'longitude': np.random.uniform(-40, -20, stream_size), # Simulated South Atlantic chunk
        'latitude': np.random.uniform(-30, -10, stream_size),
        'harmonized_ssh': np.random.normal(14.2, 0.3, stream_size)
    })
    
    # 2. Process data stream into the partition manager
    print("📦 Partitioning incoming data tracks into database cells...")
    task_database = orchestrator.partition_workload_to_database(wyvern_output_stream)
    
    # 3. Read database allocation states
    print(f"✅ Generated {len(task_database)} localized hexagonal task containers.")
    
    # Display a snapshot of the allocated jobs waiting for the filtration pipeline
    sample_cells = list(task_database.keys())[:3]
    for cell in sample_cells:
        meta = task_database[cell]
        print(f"  Hexagon ID: {cell} | Data Density: {meta['data_points_count']} packets | Assigned Route: {meta['routing_target']}")
