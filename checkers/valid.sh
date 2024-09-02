#!/bin/bash

# Path to the expert_system program
PROGRAM="./expert_system"

# Directory containing the valid test files
VALID_DIR="resources/valids"

# Counter for passed tests
PASSED=0

# Counter for total tests
TOTAL=0

# Loop over all files in the valid directory
for FILE in "$VALID_DIR"/*; do
    # Extract the expected result from the last line
    EXPECTED_RESULT=$(tail -n 1 "$FILE" | sed 's/^# //')

    # Run the program with the file
    OUTPUT=$($PROGRAM "$FILE")
    
    # Capture the exit code
    EXIT_CODE=$?
    
    # Increase the total tests counter
    TOTAL=$((TOTAL + 1))
    
    # Check if the exit code is zero (indicating success)
    if [ $EXIT_CODE -eq 0 ]; then
        echo "Test passed for $FILE"
        echo "Expected: $EXPECTED_RESULT"
        echo "Output:   $OUTPUT"
        PASSED=$((PASSED + 1))
    else
        echo "Test failed for $FILE (exit code: $EXIT_CODE)"
    fi

    echo "--------------------------------"
done

# Summary of results
echo "--------------------------------"
echo "Passed $PASSED out of $TOTAL tests."

# Exit with the number of failed tests as the exit code
EXIT_CODE=$((TOTAL - PASSED))
exit $EXIT_CODE
