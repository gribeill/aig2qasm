# aig2qasm

Convert a logic circuit described by an AIGER file (And-Inverter Graph) to an OpenQASM 2.0 circuit.

This tool is based on the EPFL Logic Synthesis Libraries, and in particular [caterpillar](https://github.com/gmeuli/caterpillar)

## Usage

The first step is generating an AIGER file. There are many ways to do this, but one possibility is to use the open-source [Yosys synthesis tools](https://yosyshq.net/yosys/documentation.html) to convert a Verilog module to an AIG representation. Obviously this will only work for combinatorial circuits! A particularly convenient way to install these is to use the [YoWASP](https://yowasp.org) project.

A basic yosys script to convert an input verilog file to an and-inverter graph is:

```
read_verilog my_module.v
hierarchy -check -top my_module
proc; opt;
techmap; abc; opt;
aigmap; opt;
write_aiger my_module.aig
``` 

`aig2qasm` can then be invoked on the resulting file. 

Unfortunately for now any naming information for module inputs and outputs is lost in translation, and input/output registers and ancillas may not be arranged in a straightforward way. For now, the mapping is indicated in a comment header block at the top of the OpenQASM file.

## Building

From the root directory
```
mkdir build; cd build
cmake ..
make all
``` 

## To do's

Currently SAT solver provided by [MiniSAT](https://minisat.se/Main.html); it should be possible to use Z3 but caterpillar needs to be updated to use the latest versions of Z3. 

Possibly related to the above, the synthesis algorithm tends to choke for circuits with O(1000) nodes in the AIG graph. 

Add XAG graph strategy.