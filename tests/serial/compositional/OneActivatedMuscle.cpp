#ifndef PRECICE_NO_MPI

#include "testing/Testing.hpp"

#include <boost/test/tools/detail/per_element_manip.hpp>
#include <precice/Participant.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(Integration)
BOOST_AUTO_TEST_SUITE(Serial)
BOOST_AUTO_TEST_SUITE(Compositional)
BOOST_AUTO_TEST_CASE(OneActivatedMuscle)
{
  PRECICE_TEST("M1SM"_on(1_rank), "M2SM"_on(1_rank), "M1"_on(1_rank), "Tendon"_on(1_rank));

  std::cout << "Before constructor" << std::endl;
  precice::Participant participant(context.name, context.config(), context.rank, context.size);

  const std::vector<double> surfaceCoords{1, 0, 2, 0};
  const std::vector<double> signalCoords{0, 0};

  std::vector<int> surface1VertexIDs(2);
  std::vector<int> surface2VertexIDs(2);
  std::vector<int> activationVertexIDs(1);
  std::vector<int> stretchVertexIDs(1);
  std::vector<int> crossStretchVertexIDs(1);

  double timestepSize = 1.0;

  if (context.isNamed("M1SM")) {

    participant.setMeshVertices("Surface_Mesh1", surfaceCoords , surface1VertexIDs );

    participant.setMeshVertices("Activation_M1SM_Mesh", signalCoords , activationVertexIDs );

    participant.setMeshVertices("Stretch_M1SM_Mesh", signalCoords , stretchVertexIDs );

  } else if (context.isNamed("M2SM")) {

    participant.setMeshVertices("Surface_Mesh2", surfaceCoords , surface2VertexIDs );

    participant.setMeshVertices("Stretch_M2SM_Mesh", signalCoords , stretchVertexIDs );

  } else if (context.isNamed("M1")) {

    participant.setMeshVertices("Stretch_M1_Mesh", signalCoords , stretchVertexIDs );

    participant.setMeshVertices("Stretch_M1_Cross_Mesh", signalCoords , crossStretchVertexIDs );

    participant.setMeshVertices("Activation_M1_Mesh", signalCoords , activationVertexIDs );

  } else {
    BOOST_TEST(context.isNamed("Tendon"));

    participant.setMeshVertices("SurfaceTendon_Mesh1", surfaceCoords , surface1VertexIDs );

    participant.setMeshVertices("SurfaceTendon_Mesh2", surfaceCoords , surface2VertexIDs );

  }

  std::cout << "Before initialize" << std::endl;
  participant.initialize();

  for (int timestep = 0; timestep < 2; ++timestep) {

    std::vector<double> tractions1{1.2, 3.4};
    std::vector<double> displacements1{4.2, 1.4};
    std::vector<double> tractions2{1.2, 3.7};
    std::vector<double> displacements2{4.1, 1.4};

    if (context.isNamed("M1SM")) {

      participant.writeData("Surface_Mesh1","Displacement1", surface1VertexIDs , displacements1 );

    } else if (context.isNamed("Tendon")) {

      std::vector<double> receivedDisplacements{0.0, 0.0};
      participant.readData("SurfaceTendon_Mesh1","Displacement1", surface1VertexIDs, timestepSize, receivedDisplacements );
      BOOST_TEST(receivedDisplacements == displacements1, boost::test_tools::per_element());

    } else if (context.isNamed("M2SM")) {

    } else {
      BOOST_TEST(context.isNamed("M1"));
    }

    std::cout << "Before advance" << std::endl;
    participant.advance(timestepSize);
  }
}

BOOST_AUTO_TEST_SUITE_END() // Integration
BOOST_AUTO_TEST_SUITE_END() // Serial
BOOST_AUTO_TEST_SUITE_END() // Compositional

#endif // PRECICE_NO_MPI
