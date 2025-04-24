#!/bin/bash
set -e

echo "Installing Doxygen..."

OS="$(uname)"
if [[ "$OS" == "Linux" ]]; then
    sudo yum install -y doxygen || sudo apt-get update && sudo apt-get install -y doxygen
elif [[ "$OS" == "Darwin" ]]; then
    brew install doxygen
else
    echo "Manual Doxygen install needed on Windows"
fi

