#include <petscdmplex.h>
#include <petscsys.h>

int main(int argc, char **argv) {
    PetscErrorCode ierr;
    DM             dm;
    PetscMPIInt    rank;
    
    ierr = PetscInitialize(&argc, &argv, NULL, "3D Mesh Example with PETSc"); CHKERRQ(ierr);
    ierr = MPI_Comm_rank(PETSC_COMM_WORLD, &rank); CHKERRQ(ierr);


    /* 
       Create a 3D box mesh (a cube). 
       The parameters are:
       
         - The communicator
         - The topological dimension (3 for a 3D mesh)
         - Whether to interpolate (i.e., generate intermediate entities like faces)
         - Optionally, arrays for lower and upper bounds; NULL lets PETSc use defaults.
         - Optionally, an array of cells in each direction; NULL lets PETSc decide.
         - Optionally, an array of periodicity; NULL for non-periodic.
         - Whether to use simplex elements (PETSC_TRUE) or tensor cells (PETSC_FALSE)
         - Pointer to the DM object
    */
ierr = DMPlexCreateBoxMesh(PETSC_COMM_WORLD, 3, PETSC_FALSE,
                             NULL, NULL, NULL, NULL,
                             PETSC_TRUE, 0, PETSC_TRUE, &dm); CHKERRQ(ierr);


    /* 
       View the mesh to a file. PETSc supports various viewers; here we use VTK.
       You can pass options at runtime (e.g., "-dm_view vtk:output.vtu") to change behavior.
       Alternatively, explicitly call DMView with a PETScViewer.
    */
    {
        PetscViewer viewer;
        ierr = PetscViewerVTKOpen(PETSC_COMM_WORLD, "mesh.vtu", FILE_MODE_WRITE, &viewer); CHKERRQ(ierr);
        ierr = DMView(dm, viewer); CHKERRQ(ierr);
        ierr = PetscViewerDestroy(&viewer); CHKERRQ(ierr);
    }

    if (rank == 0) {
        PetscPrintf(PETSC_COMM_SELF, "Mesh written to mesh.vtu. Open this file in ParaView.\n");
    }

    ierr = DMDestroy(&dm); CHKERRQ(ierr);
    ierr = PetscFinalize(); CHKERRQ(ierr);
    return 0;
}
