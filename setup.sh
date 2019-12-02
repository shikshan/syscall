#!/bin/bash

echo "Setting up vagrant environment..."
vagrant up
echo "Connecting to VM..."
vagrant ssh
exit 0
