#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/ndnSIM-module.h"
#include "ns3/position-allocator.h"

using namespace std;
namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ndn.WifiExample");

int main(int argc, char* argv[]) {
	// disable fragmentation
	Config::SetDefault("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("2200"));
	Config::SetDefault("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue("OfdmRate24Mbps"));

	CommandLine cmd;
	cmd.Parse(argc, argv);

	WifiHelper wifi ;
	wifi.SetStandard(WIFI_PHY_STANDARD_80211a);
	wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate24Mbps"));

	YansWifiChannelHelper wifiChannel;
	wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	wifiChannel.AddPropagationLoss("ns3::ThreeLogDistancePropagationLossModel");
	wifiChannel.AddPropagationLoss("ns3::NakagamiPropagationLossModel");

	YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
	wifiPhyHelper.SetChannel(wifiChannel.Create());
	wifiPhyHelper.Set("TxPowerStart", DoubleValue(5));
	wifiPhyHelper.Set("TxPowerEnd", DoubleValue(5));

	WifiMacHelper wifiMacHelper ;
	wifiMacHelper.SetType("ns3::AdhocWifiMac");

//	Ptr<UniformRandomVariable> randomizer = CreateObject<UniformRandomVariable>();
//	randomizer->SetAttribute("Min", DoubleValue(10));
//	randomizer->SetAttribute("Max", DoubleValue(100));

	int m_totalNodes = 27;
	NodeContainer nodes;
	nodes.Create(m_totalNodes);

	// 1. Install Wifi
	NetDeviceContainer wifiNetDevices = wifi.Install(wifiPhyHelper, wifiMacHelper, nodes);
	wifiPhyHelper.EnablePcap("wave-simple-80211a", wifiNetDevices);

	MobilityHelper mobility;
	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

	int m_totalx = 3;
	int m_totaly = 3;
	int m_totalz = 3;
	int m_distance = 120;
	int m_x = 0;
	int m_y = 0;
	int m_z = 0;
	for(int i=1; i<=m_totalx; i++){
		for(int j=1; j<=m_totaly; j++){
			for(int k=1; k<=m_totalz; k++){
				positionAlloc->Add(Vector(m_x, m_y, m_z));
				m_z += m_distance;
			}
			m_y += m_distance;
			m_z = 0;
		}
		m_x += m_distance;
		m_y = 0;
	}
	mobility.SetPositionAllocator(positionAlloc);
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

	// 2. Install Mobility model
	mobility.Install(nodes);

	// 3. Install NDN stack
	NS_LOG_INFO("Installing NDN stack");
	ndn::StackHelper ndnHelper;
	ndnHelper.SetOldContentStore("ns3::ndn::cs::Lru", "MaxSize", "1000");
	//ndnHelper.setCsSize(1000);
	//ndnHelper.setPolicy("nfd::cs::lru");
	//ndnHelper.SetDefaultRoutes(true);
	ndnHelper.Install(nodes);

	// Set BestRoute strategy
	ndn::StrategyChoiceHelper::Install(nodes, "/", "/localhost/nfd/strategy/best-route");


	ndn::GlobalRoutingHelper ndnGlobalRoutingHelper;
	ndnGlobalRoutingHelper.Install(nodes);


	// 4. Set up applications
	NS_LOG_INFO("Installing Applications");

	ndn::AppHelper consumerHelper1("ns3::ndn::ConsumerZipfMandelbrot");
	consumerHelper1.SetPrefix("/root");
	consumerHelper1.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper1.Install(nodes.Get(5));

	Ptr<Node> producer = nodes.Get(20);

	ndn::AppHelper producerHelper1("ns3::ndn::Producer");

	producerHelper1.SetAttribute("PayloadSize", StringValue("1024"));
	ndnGlobalRoutingHelper.AddOrigins("/root", producer);
	producerHelper1.SetPrefix("/root");
	producerHelper1.Install(producer);

/*
	ndn::AppHelper consumerHelper2("ns3::ndn::ConsumerCbr");
	consumerHelper2.SetPrefix("/pair-2/");
	consumerHelper2.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper2.Install(nodes.Get(9));

	ndn::AppHelper producerHelper2("ns3::ndn::Producer");
	producerHelper2.SetPrefix("/pair-2/");
	producerHelper2.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper2.Install(nodes.Get(19));


	ndn::AppHelper consumerHelper3("ns3::ndn::ConsumerCbr");
	consumerHelper3.SetPrefix("/pair-3/");
	consumerHelper3.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper3.Install(nodes.Get(71));

	ndn::AppHelper producerHelper3("ns3::ndn::Producer");
	producerHelper3.SetPrefix("/pair-3/");
	producerHelper3.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper3.Install(nodes.Get(81));


	ndn::AppHelper consumerHelper4("ns3::ndn::ConsumerCbr");
	consumerHelper4.SetPrefix("/pair-4/");
	consumerHelper4.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper4.Install(nodes.Get(68));

	ndn::AppHelper producerHelper4("ns3::ndn::Producer");
	producerHelper4.SetPrefix("/pair-4/");
	producerHelper4.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper4.Install(nodes.Get(78));


	ndn::AppHelper consumerHelper5("ns3::ndn::ConsumerCbr");
	consumerHelper5.SetPrefix("/pair-5/");
	consumerHelper5.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper5.Install(nodes.Get(89));

	ndn::AppHelper producerHelper5("ns3::ndn::Producer");
	producerHelper5.SetPrefix("/pair-5/");
	producerHelper5.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper5.Install(nodes.Get(99));


	ndn::AppHelper consumerHelper6("ns3::ndn::ConsumerCbr");
	consumerHelper6.SetPrefix("/pair-6/");
	consumerHelper6.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper6.Install(nodes.Get(32));

	ndn::AppHelper producerHelper6("ns3::ndn::Producer");
	producerHelper6.SetPrefix("/pair-6/");
	producerHelper6.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper6.Install(nodes.Get(43));


	ndn::AppHelper consumerHelper7("ns3::ndn::ConsumerCbr");
	consumerHelper7.SetPrefix("/pair-7/");
	consumerHelper7.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper7.Install(nodes.Get(36));

	ndn::AppHelper producerHelper7("ns3::ndn::Producer");
	producerHelper7.SetPrefix("/pair-7/");
	producerHelper7.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper7.Install(nodes.Get(47));


	ndn::AppHelper consumerHelper8("ns3::ndn::ConsumerCbr");
	consumerHelper8.SetPrefix("/pair-8/");
	consumerHelper8.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper8.Install(nodes.Get(23));

	ndn::AppHelper producerHelper8("ns3::ndn::Producer");
	producerHelper8.SetPrefix("/pair-8/");
	producerHelper8.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper8.Install(nodes.Get(24));


	ndn::AppHelper consumerHelper9("ns3::ndn::ConsumerCbr");
	consumerHelper9.SetPrefix("/pair-9/");
	consumerHelper9.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper9.Install(nodes.Get(49));

	ndn::AppHelper producerHelper9("ns3::ndn::Producer");
	producerHelper9.SetPrefix("/pair-9/");
	producerHelper9.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper9.Install(nodes.Get(58));


	ndn::AppHelper consumerHelper10("ns3::ndn::ConsumerCbr");
	consumerHelper10.SetPrefix("/pair-10/");
	consumerHelper10.SetAttribute("Frequency", DoubleValue(100.0)); // no. of interests per second
	consumerHelper10.Install(nodes.Get(63));

	ndn::AppHelper producerHelper10("ns3::ndn::Producer");
	producerHelper10.SetPrefix("/pair-9/");
	producerHelper10.SetAttribute("PayloadSize", StringValue("1200"));
	producerHelper10.Install(nodes.Get(74));


*/
	// 5. Running simulation
	Simulator::Stop(Seconds(20));

	//ndn::AppDelayTracer::InstallAll("app-delays-trace.txt");
	 ndn::CsTracer::InstallAll("cs-test10.txt", Seconds(1));
	//ndn::L3RateTracer::InstallAll("rate-test10.txt", Seconds(1.0));
	//L2RateTracer::InstallAll("drop-trace.txt", Seconds(0.5));

	  AnimationInterface anim("anim_test.xml");  // 将会产生一个xml文件。
	  anim.SetMobilityPollInterval (Seconds (1));
	  anim.EnablePacketMetadata (true);



	Simulator::Run();
	Simulator::Destroy();

	return 0;
}

}

int main(int argc, char* argv[]) {
	return ns3::main(argc, argv);
}
