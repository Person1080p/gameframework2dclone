#!/bin/bash
# Get Submodules
git clone https://github.com/engineerOfLies/gfc.git
cd gfc
git clone https://github.com/engineerOfLies/simple_json.git
git clone https://github.com/engineerOfLies/simple_logger.git
cd ./simple_json/src

# Install Submodules
make
make static
cd ../..
# pwd
cd ./simple_logger/src
make
make static
cd ../..
# pwd
cd ./src
make
make static

# Build total project and run
cd ../..
# pwd
cd ./src
make
echo "PROJECT READY (hopefully) GO TO PROJECT ROOT TO RUN \"gf2d\""

# Run Project
cd ..
./gf2d