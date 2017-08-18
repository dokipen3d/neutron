# neutron
FluidSolver in houdini


This is a fluid simulation plugin for sidefx houdini. 
It uses a sparse eulerian grid and currently supports a basic jacobi pressure solve, maccormack and BFECC advection.

Things left to do.

- Finish the volume rendering (almost done)
- Actually integrate the fuid code from another repository
- Port the boundary condition code (from the 2dfluid project)
- Switch to the hash table that actually allocates itself. Experiment with lock free padding.
- Switch from live allocating to a memory pool.
- Rewrite the simd pressure solve that I lost in a hard drive failure.
- Implement basic GPU mode.
