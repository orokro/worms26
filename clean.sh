#!/bin/sh
find . -type f \( -name "*.o" -o -name "*.s" \) -delete
echo "Cleaned all .o and .s files."
