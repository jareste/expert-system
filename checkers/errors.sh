#!/bin/bash

# Path to the expert_system program
PROGRAM="./expert_system"

# Directory containing the test files
ERROR_DIR="resources/errors"

# Counter for passed tests
PASSED=0

# Counter for total tests
TOTAL=0

# Array to store failed tests
FAILED_TESTS=()

# Loop over all files in the errors directory
for FILE in "$ERROR_DIR"/*; do
    # Run the program with the file
    $PROGRAM "$FILE"
    
    # Capture the exit code
    EXIT_CODE=$?
    
    # Increase the total tests counter
    TOTAL=$((TOTAL + 1))
    
    # Check if the exit code is non-zero (indicating an error)
    if [ $EXIT_CODE -ne 0 ]; then
        echo "Test passed for $FILE (exit code: $EXIT_CODE)"
        PASSED=$((PASSED + 1))
    else
        echo "Test failed for $FILE (exit code: $EXIT_CODE)"
        FAILED_TESTS+=("$FILE")
    fi
done

# Summary of results
echo "--------------------------------"
echo "Passed $PASSED out of $TOTAL tests."

# Print the names of the failed tests, if any
if [ ${#FAILED_TESTS[@]} -ne 0 ]; then
    echo "The following tests failed:"
    for FAILED in "${FAILED_TESTS[@]}"; do
        echo "$FAILED"
    done
else
    echo "All tests passed."
fi

# Exit with the number of failed tests as the exit code
EXIT_CODE=$((TOTAL - PASSED))
exit $EXIT_CODE
