#!/bin/bash

#TODO loop through all shader source files and output based on extension
glslc.exe ../src/shader.vert -o ../bin/x64/vert.spv
glslc.exe ../src/shader.frag -o ../bin/x64/frag.spv
