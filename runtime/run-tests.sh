#!/bin/bash
set -e

echo "Running Python tests"
pytest tests/test_installs.py
