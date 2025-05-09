#!/bin/bash
set -e

echo "Installing Doxygen..."

OS="$(uname)"
if [[ "$OS" == "Linux" ]]; then
    sudo apt-get install -y doxygen
elif [[ "$OS" == "Darwin" ]]; then
    brew install doxygen --formula
else
    echo "Manual Doxygen install needed on Windows"
fi

