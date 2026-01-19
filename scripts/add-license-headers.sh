#!/bin/bash
#
# add-license-headers.sh - Add GPL v3 license headers to all source files
#
# This script updates license headers in all C source (.c) and header (.h) files
# to include proper GPL v3 copyright and license information.
#
# Usage: ./add-license-headers.sh
#

set -e

# License header template
read -r -d '' LICENSE_HEADER << 'EOF' || true
 * Copyright (C) 2024-2026 Open Source E-Reader Project Contributors
 *
 * This file is part of the Open Source E-Reader project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
EOF

# Function to update a single file
update_file() {
    local file="$1"
    echo "Processing: $file"

    # Check if file already has GPL license
    if grep -q "GNU General Public License" "$file"; then
        echo "  -> Already has GPL header, skipping"
        return
    fi

    # Create temporary file
    local tmpfile=$(mktemp)

    # Extract the first comment block (file description)
    local in_comment=0
    local found_license=0
    local header_end_line=0
    local line_num=0

    while IFS= read -r line; line_num=$((line_num + 1)); do
        # Start of comment
        if [[ "$line" =~ ^[[:space:]]*\/\* ]]; then
            in_comment=1
        fi

        # Check for old license line
        if [[ $in_comment -eq 1 ]] && [[ "$line" =~ License:.*Open\ Source ]]; then
            found_license=1
            # Replace with GPL header
            echo " *" >> "$tmpfile"
            echo "$LICENSE_HEADER" >> "$tmpfile"
        elif [[ $found_license -eq 0 ]]; then
            # Keep original header content until we find license line
            echo "$line" >> "$tmpfile"
        else
            # After replacing license, keep remaining content
            echo "$line" >> "$tmpfile"
        fi

        # End of comment
        if [[ "$line" =~ \*\/ ]]; then
            in_comment=0
            if [[ $found_license -eq 1 ]]; then
                header_end_line=$line_num
                break
            fi
        fi
    done < "$file"

    # If we found and replaced the license, append rest of file
    if [[ $found_license -eq 1 ]]; then
        tail -n +$((header_end_line + 1)) "$file" >> "$tmpfile"
        mv "$tmpfile" "$file"
        echo "  -> Updated successfully"
    else
        echo "  -> No 'License: Open Source' found, skipping"
        rm "$tmpfile"
    fi
}

# Find all C source and header files
echo "Adding GPL v3 license headers to source files..."
echo "================================================"

# Process source files in src/ereader
find src/ereader -type f \( -name "*.c" -o -name "*.h" \) | while read -r file; do
    update_file "$file"
done

# Process test files
find src/display-test src/button-test -type f \( -name "*.c" -o -name "*.h" \) 2>/dev/null | while read -r file; do
    update_file "$file"
done

echo ""
echo "================================================"
echo "License header update complete!"
echo ""
echo "Files updated with GPL v3 headers."
echo "Please review changes before committing."
