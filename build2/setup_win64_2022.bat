@echo off

mkdir g4gBuild
cd g4gBuild
cmake -G "Visual Studio 17 2022" ../../src/Project2
cd ..
