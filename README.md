# Cognitive Radio MAC Module for ns-3

## Overview

This project implements a Cognitive Radio MAC (Medium Access Control) protocol module for the ns-3 network simulator. The module provides comprehensive support for cognitive radio functionality including spectrum sensing, dynamic spectrum access, interference management, and energy-aware operations.

## Features

- **Spectrum Sensing**: Advanced spectrum sensing capabilities with configurable thresholds and sensing periods
- **Dynamic Spectrum Access**: Intelligent channel selection based on spectrum availability
- **Interference Management**: Cognitive spectrum interference modeling and mitigation
- **Energy Management**: Energy-aware operations with customizable energy models
- **MAC Protocol**: Complete MAC layer implementation for cognitive radio networks
- **Routing Support**: Cognitive routing unit for adaptive routing decisions
- **Control Applications**: Cognitive control applications for network management

## Requirements

### System Requirements

- **Operating System**: Linux, macOS, or Windows
- **Compiler**: GCC 7.4+ or Clang 6.0+ (Linux/macOS), Visual Studio 2017+ (Windows)
- **Build System**: CMake 3.16+
- **Python**: Python 3.6+ (for build scripts)

### ns-3 Requirements

- **ns-3 Version**: ns-3.37 or later
- **Required ns-3 Modules**:
  - `core` - Core ns-3 functionality
  - `network` - Network layer support
  - `spectrum` - Spectrum management
  - `energy` - Energy modeling
  - `mobility` - Mobility models
  - `antenna` - Antenna models

### Dependencies

#### Core Dependencies
- **Boost Libraries**: 1.65+ (for smart pointers and utilities)
- **SQLite3**: 3.8+ (for data logging and storage)
- **GTK3**: 3.20+ (for GUI components, optional)
- **Qt5**: 5.12+ (for GUI components, optional)

#### Build Dependencies
- **pkg-config**: For dependency detection
- **libxml2**: 2.9+ (for XML parsing)
- **libsqlite3-dev**: SQLite development headers
- **libgtk-3-dev**: GTK3 development headers (optional)
- **qt5-default**: Qt5 development headers (optional)

#### Optional Dependencies
- **GNU Scientific Library (GSL)**: For mathematical computations
- **OpenMP**: For parallel processing support
- **MPI**: For distributed simulation support

## Installation

### Prerequisites Installation

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake git
sudo apt install libboost-all-dev libsqlite3-dev libxml2-dev
sudo apt install libgtk-3-dev qt5-default
sudo apt install libgsl-dev libopenmpi-dev
```

#### CentOS/RHEL/Fedora
```bash
sudo yum groupinstall "Development Tools"
sudo yum install cmake git
sudo yum install boost-devel sqlite-devel libxml2-devel
sudo yum install gtk3-devel qt5-devel
sudo yum install gsl-devel openmpi-devel
```

#### macOS
```bash
brew install cmake boost sqlite3 libxml2
brew install gtk+3 qt5
brew install gsl open-mpi
```

### ns-3 Installation

1. **Download ns-3**:
```bash
git clone https://gitlab.com/nsnam/ns-3-dev.git
cd ns-3-dev
```

2. **Configure ns-3**:
```bash
./ns3 configure --enable-examples --enable-tests
```

3. **Build ns-3**:
```bash
./ns3 build
```

### Cognitive MAC Module Installation

1. **Clone the repository**:
```bash
git clone <repository-url>
cd Cognitive-Radio
```

2. **Copy module to ns-3**:
```bash
cp -r cognitive-mac /path/to/ns-3-dev/contrib/
```

3. **Reconfigure ns-3 with the module**:
```bash
cd /path/to/ns-3-dev
./ns3 configure --enable-examples --enable-tests
```

4. **Build with the module**:
```bash
./ns3 build
```

## Usage

### Basic Example

```cpp
#include "ns3/core-module.h"
#include "ns3/cognitive-mac-helper.h"

using namespace ns3;

int main(int argc, char* argv[])
{
    bool verbose = true;
    
    CommandLine cmd(__FILE__);
    cmd.AddValue("verbose", "Tell application to log if true", verbose);
    cmd.Parse(argc, argv);
    
    // Create cognitive MAC helper
    CognitiveMacHelper cognitiveMacHelper;
    
    // Configure spectrum sensing parameters
    cognitiveMacHelper.SetSpectrumSensingPeriod(MicroSeconds(100));
    cognitiveMacHelper.SetSensingThreshold(-90.0); // dBm
    
    // Create and configure nodes
    NodeContainer cognitiveNodes;
    cognitiveNodes.Create(5);
    
    // Install cognitive MAC on nodes
    NetDeviceContainer devices = cognitiveMacHelper.Install(cognitiveNodes);
    
    // Run simulation
    Simulator::Run();
    Simulator::Destroy();
    
    return 0;
}
```

### Advanced Configuration

```cpp
// Configure energy model
CognitiveDeviceEnergyModelHelper energyHelper;
energyHelper.Set("InitialEnergy", DoubleValue(100.0)); // Joules
energyHelper.Set("TxCurrent", DoubleValue(0.0174));    // Amperes
energyHelper.Set("RxCurrent", DoubleValue(0.0197));    // Amperes

// Configure spectrum control
SpectrumControlModuleHelper spectrumHelper;
spectrumHelper.Set("SensingRounds", UintegerValue(10));
spectrumHelper.Set("ChannelCount", UintegerValue(8));
spectrumHelper.Set("SensingPeriod", TimeValue(MicroSeconds(50)));
```

## Module Components

### Core Components

- **CognitiveMac**: Main MAC protocol implementation
- **CognitivePhyDevice**: Physical layer device with spectrum sensing
- **CognitiveGeneralNetDevice**: Network device abstraction
- **SpectrumControlModule**: Spectrum sensing and control logic
- **CognitiveSpectrumInterference**: Interference modeling and management

### Helper Classes

- **CognitiveMacHelper**: High-level MAC configuration helper
- **CognitiveNetDeviceHelper**: Network device setup helper
- **CognitiveDeviceEnergyModelHelper**: Energy model configuration

### Energy Management

- **CognitiveRadioEnergyModel**: Energy consumption modeling
- **CognitiveDeviceEnergyModel**: Device-specific energy tracking

### Control and Routing

- **CognitiveControlApplication**: Control plane applications
- **CognitiveRoutingUnite**: Adaptive routing decisions
- **CognitiveControlMessage**: Control message handling

## Configuration Parameters

### Spectrum Sensing
- `SensingPeriod`: Duration of each sensing round
- `SensingThreshold`: Energy threshold for channel detection
- `ChannelCount`: Number of available channels
- `SensingRounds`: Number of sensing iterations

### Energy Management
- `InitialEnergy`: Initial battery energy in Joules
- `TxCurrent`: Transmission current consumption
- `RxCurrent`: Reception current consumption
- `IdleCurrent`: Idle state current consumption

### MAC Parameters
- `SlotTime`: MAC slot duration
- `SIFS`: Short inter-frame spacing
- `DIFS`: DCF inter-frame spacing
- `CWMin`: Minimum contention window
- `CWMax`: Maximum contention window

## Examples

The module includes several example scenarios:

- **Basic Cognitive Radio Network**: Simple spectrum sensing and access
- **Multi-Channel Communication**: Dynamic channel switching
- **Energy-Aware Operations**: Battery-aware transmission scheduling
- **Interference Mitigation**: Advanced interference management

To run examples:
```bash
./ns3 run cognitive-mac-example
```

## Testing

Run the test suite to verify module functionality:
```bash
./ns3 test cognitive-mac-test-suite
```

## Documentation

- **API Documentation**: Generated using Doxygen
- **User Guide**: Available in `doc/` directory
- **Examples**: Located in `examples/` directory

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## License

This project is licensed under the GNU General Public License v3.0.

## Support

For issues and questions:
- Create an issue on the project repository
- Check the documentation in the `doc/` directory
- Review the example implementations

## Acknowledgments

This module builds upon the ns-3 network simulator framework and incorporates research in cognitive radio networks, spectrum sensing, and dynamic spectrum access.

---

**Note**: This README provides a comprehensive overview of the Cognitive Radio MAC module. For detailed API documentation and advanced usage scenarios, please refer to the inline documentation and example files. 