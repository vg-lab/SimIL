#include <chrono>
#include <fstream>
#include <iostream>

#include <contra/relay.hpp>
#include <contra/zmq/zeromq_transport.hpp>
#include <nesci/consumer/spike_detector_data_view.hpp>
#include <cone/cone.hpp>

int main( int, char** )
{
  // auto m_cone  = cone::ConnectViaZeroMQ("localhost:5555");
  auto m_cone = cone::ConnectUsingConfigurationFile(
    "../../examples/cone_conf_files/zeromq_client.json" );

  m_cone.SetSpikeDetectorCallback( []( const auto& data ) {
    // Do something...

    auto timesteps = data.GetTimesteps( );

    auto neuron_ids = data.GetNeuronIds( );
    std::cout << "num of " << timesteps.number_of_elements( );
    unsigned int num_spikes = 0;
    while ( num_spikes < timesteps.number_of_elements( ) )
    {
      std::cout << "{" << timesteps[ num_spikes ] << ","
                << neuron_ids[ num_spikes ] << "}" << std::endl;
      ++num_spikes;
    }
  } );

  // 3. Poll Data
  while ( true )
  {
    m_cone.Poll( 1 /* optional timeout*/ );
  }

  /*unsigned int num_spikes = 0;
  while (true) {
    const auto nodes = relay.Receive();

    for (const auto node : nodes) {
      nesci::consumer::SpikeDetectorDataView sd(&node);
      const auto timesteps = sd.GetTimesteps();
      //for (const auto timestep : timesteps) {
        const auto neuron_ids = sd.GetNeuronIds();
        //for (const auto neuron_id : neuron_ids) {
          ++num_spikes;
          std::cin.get();
          std::cout << "{" << timesteps[num_spikes] << "," <<
  neuron_ids[num_spikes] << "}" << std::endl;
        //}
      //}
    }
    if (nodes.size() > 0) {
          std::cout << "Recevied " << num_spikes << " so far!" << std::endl;
    }
  }*/
}
