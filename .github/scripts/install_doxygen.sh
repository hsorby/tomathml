#!/bin/bash
set -e

echo "Installing Doxygen..."

OS="$(uname)"
if [[ "$OS" == "Linux" ]]; then
    # yum install -y doxygen <-- For older manylinux containers.
    apt install -y doxygen # <-- For newer manylinux containers.
elif [[ "$OS" == "Darwin" ]]; then
    brew install doxygen --formula
else
    echo "Manual Doxygen install needed on Windows"
fi

