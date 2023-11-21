#ifndef PRECICE_NO_MPI

#include "testing/Testing.hpp"

#include <boost/test/tools/detail/per_element_manip.hpp>
#include <precice/Participant.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(Integration)
BOOST_AUTO_TEST_SUITE(Serial)
BOOST_AUTO_TEST_SUITE(Compositional)
BOOST_AUTO_TEST_CASE(TwoActivatedMuscles)
{
  PRECICE_TEST("M1SM"_on(1_rank), "M2SM"_on(1_rank), "M1"_on(1_rank), "M2"_on(1_rank));

  std::cout << "Before constructor" << std::endl;
  precice::Participant participant(context.name, context.config(), context.rank, context.size);

  const std::vector<double> surfaceCoords{1, 0, 2, 0};
  const std::vector<double> signalCoords{0, 0};

  std::vector<int> surfaceVertexIDs(2);
  std::vector<int> activationVertexIDs(1);
  std::vector<int> stretchVertexIDs(1);
  std::vector<int> crossStretchVertexIDs(1);

  double timestepSize = 1.0;

  if (context.isNamed("M1SM")) {

    participant.setMeshVertices("Surface_Mesh1", surfaceCoords , surfaceVertexIDs );

    participant.setMeshVertices("Activation_M1SM_Mesh", signalCoords , activationVertexIDs );

    participant.setMeshVertices("Stretch_M1SM_Mesh", signalCoords , stretchVertexIDs );

  } else if (context.isNamed("M2SM")) {

    participant.setMeshVertices("Surface_Mesh2", surfaceCoords , surfaceVertexIDs );

    participant.setMeshVertices("Activation_M2SM_Mesh", signalCoords , activationVertexIDs );

    participant.setMeshVertices("Stretch_M2SM_Mesh", signalCoords , stretchVertexIDs );

  } else if (context.isNamed("M1")) {

    participant.setMeshVertices("Stretch_M1_Mesh", signalCoords , stretchVertexIDs );

    participant.setMeshVertices("Stretch_M1_Cross_Mesh", signalCoords , crossStretchVertexIDs );

    participant.setMeshVertices("Activation_M1_Mesh", signalCoords , activationVertexIDs );

  } else {
    BOOST_TEST(context.isNamed("M2"));

    participant.setMeshVertices("Stretch_M2_Mesh", signalCoords , stretchVertexIDs );

    participant.setMeshVertices("Stretch_M2_Cross_Mesh", signalCoords , crossStretchVertexIDs );

    participant.setMeshVertices("Activation_M2_Mesh", signalCoords , activationVertexIDs );

  }

  std::cout << "Before initialize" << std::endl;
  participant.initialize();

  for (int timestep = 0; timestep < 2; ++timestep) {

    std::vector<double> tractions{1.2, 3.4};
    std::vector<double> displacements{4.2, 1.4};

    if (context.isNamed("M1SM")) {

      participant.writeData("Surface_Mesh1","Displacement1", surfaceVertexIDs , displacements );

    } else if (context.isNamed("M2SM")) {

      std::vector<double> receivedDisplacements{0.0, 0.0};
      participant.readData( "SurfaceTendon_Mesh1","Displacement1", surfaceVertexIDs, timestepSize, receivedDisplacements );
      BOOST_TEST(receivedDisplacements == displacements, boost::test_tools::per_element());

    } else if (context.isNamed("M1")) {

    } else {
      BOOST_TEST(context.isNamed("M2"));
    }

    std::cout << "Before advance" << std::endl;
    participant.advance(timestepSize);
  }
}

BOOST_AUTO_TEST_SUITE_END() // Integration
BOOST_AUTO_TEST_SUITE_END() // Serial
BOOST_AUTO_TEST_SUITE_END() // Compositional

#endif // PRECICE_NO_MPI
