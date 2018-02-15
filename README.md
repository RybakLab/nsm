# nsm
NSM is a simulation environment specially developed for multiscale modeling and computational analysis of cross-level integration of: 
* the intrinsic biophysical properties of single neurons (at the level of ionic channel kinetics, dynamical changes of ionic concentrations, synaptic processes); 
* population properties (synaptic interactions between neurons within populations with random distributions of neuronal parameters); 
* network properties (connectivity and type of synaptic interactions between populations with random distribution of connections), 
* morpho-physiological structure (organization in interacting modules/compartments). 

NSM runs on standard x86-based computing systems and has special tools for simulation of various in vivo and in vitro experimental approaches (suppression of different ionic channels and transmitter systems, lesions, transections, various stimulations applied to particular neural populations, etc). 

The NSM package allows the user to explicitly construct neural models at the level of neural populations, yet having each neuron modeled using the Hodgkin-Huxley formalisms. The user defines the average values of cellular and channel parameters and their variances within each population, as well as the connection probabilities and average values and variances of synaptic weights between interacting populations. Then the program assigns parameters of individual channels, neurons and connections, using a random number generator within the range of the assigned average values and variances. Randomization of parameters within neural populations provides for robust behavior of simulated neural networks and simultaneously allows realistic simulation of single neuron types and their populations. The package has a convenient user interface, allowing simultaneous monitoring of the dynamics of sub-cellular, cellular, population and system-level variables and analysis of their relationships. 
