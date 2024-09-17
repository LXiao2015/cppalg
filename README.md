## Introduction
This program provides the source code for the SFC-D (Service Function Chain - Dynamic) algorithm, which recommends node replacements and traffic routes while accounting for various resource costs in hybrid clouds.
</br>
Please refer to publication [Dynamic Orchestration Mechanism of Service Function Chain in Hybrid NFV Networks](https://ieeexplore.ieee.org/document/8596185) for a comprehensive depiction of the detailed methodology.

## Usage
### Input
input_chains.txt</br>
format per line:</br>
src_node_index sink_node_index service_type_index bandwith_demand

### Output
demandAndPath.txt</br>
explanation per line:</br>
bandwidth_demand [node_index_list]
