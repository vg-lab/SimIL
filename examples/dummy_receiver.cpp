#include <chrono>
#include <fstream>
#include <iostream>

#include <contra/relay.hpp>
#include <contra/zmq/zeromq_transport.hpp>
#include <nesci/consumer/spike_detector_data_view.hpp>
#include <cone/cone.hpp>

int main( int, char** )
{
  auto m_cone = cone::ConnectUsingConfigurationFile(
    "../../examples/cone_conf_files/zeromq_client.json" );

  m_cone.SetSpikeDetectorCallback( []( const auto& data ) {
    auto timesteps = data.GetTimesteps( );
    auto neuron_ids = data.GetNeuronIds( );

    unsigned int num_spikes = timesteps.number_of_elements( );
    std::cout << "num of " << timesteps.number_of_elements( );
    for ( unsigned int spike; spike < num_spikes; ++spike )
    {
      std::cout << "{" << timesteps[ spike ] << "," << neuron_ids[ spike ]
                << "}" << std::endl;
    }
  } );

  while ( true )
  {
    m_cone.Poll( 1 /* optional timeout*/ );
  }
}
