## HyperBeta

HyperBeta is a tool for the automatic identification and visualization of beta-structures in molecular dynamics files produced using the MARTINI force field. 
HyperBeta explots graph theory to identify the beta sheets, and was specifically designed to investigate the emergence of regular structures and patterns in self-assembly polypeptides.

# Usage

- Open HyperBeta's GUI by launching the gui/main.py python script. 
- Press "Preprocess GRO molecular dynamics file"
- Open the GROMACS file containing the molecular dynamics
- Specify the number of aminoacids in the peptides and the interval of frames to be processed
- Press PROCESS to launch the analysis on the GROMACS file
- Press RUN to launch the Hyperbeta's Visualization Tool (HVT)

# Installation

As a first step, clone the repository on your machine. 
HyperBeta is composed of two separate softwares:

- the analysis tool, written in python
- HyperBeta's Visualization Tool, written in C++

The latter must be compiled using g++ or a similar compiler. 
A Visual Studio solution file is available in the root of the repository.

# Further information

If you need assistance with HyperBeta please contact: Marco S. Nobile (marco.nobile@unive.it)
