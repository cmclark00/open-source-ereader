#!/bin/bash
# Build script with clean PATH for buildroot
# Buildroot requires PATH without spaces, tabs, or newlines

# Set a clean PATH with only essential directories
export PATH="/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin"

# Run make with the clean PATH
cd /mnt/c/Users/koolk/Documents/open-source-ereader
exec make "$@"
