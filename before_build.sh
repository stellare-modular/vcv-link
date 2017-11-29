  cd c:\vcv-link-build
  git clone --recursive https://github.com/VCVRack/Rack.git vcv-rack
  cd vcv-rack
  git checkout v0.5
  git submodule update --init --recursive
  make dep
  make
  