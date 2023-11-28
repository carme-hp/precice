#ifndef PRECICE_NO_MPI

#include "testing/Testing.hpp"

#include <boost/test/tools/detail/per_element_manip.hpp>
#include <precice/Participant.hpp>
#include <vector>

BOOST_AUTO_TEST_SUITE(Integration)
BOOST_AUTO_TEST_SUITE(Serial)
BOOST_AUTO_TEST_SUITE(Compositional)
BOOST_AUTO_TEST_CASE(TwoActivatedMusclesImplicit)
{
  PRECICE_TEST("M1SM"_on(1_rank), "M2SM"_on(1_rank));

  precice::Participant participant(context.name, context.config(), context.rank, context.size);

  const std::vector<double> surfaceCoords{1, 0, 2, 0};

  std::vector<int> surfaceVertexIDs(2);

  double timestepSize = 1.0;

  if (context.isNamed("M1SM")) {

    participant.setMeshVertices("Surface_M1SM_Mesh", surfaceCoords , surfaceVertexIDs );

  } else {

    participant.setMeshVertices("Surface_M2SM_Mesh", surfaceCoords , surfaceVertexIDs );

  } 

  participant.initialize();
  
  std::vector<double> tractions{1.2, 3.4};
  std::vector<double> displacements{4.2, 1.4};

  std::vector<double> receivedDisplacements{0.0, 0.0};

  for (int timestep = 0; timestep < 2; ++timestep) {

    if (context.isNamed("M1SM")) {
      participant.writeData("Surface_M1SM_Mesh","Displacement", surfaceVertexIDs , displacements );
    } else {
      BOOST_TEST(context.isNamed("M2SM"));
      participant.readData( "Surface_M2SM_Mesh","Displacement", surfaceVertexIDs, timestepSize, receivedDisplacements );
    } 
    
    if (participant.requiresWritingCheckpoint()) {
    }
    participant.advance(timestepSize);
    if (participant.requiresReadingCheckpoint()){
    }
  }
  
  // Test read and write
  if (context.isNamed("M2SM")) {
    BOOST_TEST(receivedDisplacements == displacements, boost::test_tools::per_element());
  } 

  participant.finalize();
}

BOOST_AUTO_TEST_SUITE_END() // Integration
BOOST_AUTO_TEST_SUITE_END() // Serial
BOOST_AUTO_TEST_SUITE_END() // Compositional

#endif // PRECICE_NO_MPI
