#!/bin/bash

PROGRAM="./expert_system"

ERROR_DIR="resources/errors"

PASSED=0

TOTAL=0

FAILED_TESTS=()

for FILE in "$ERROR_DIR"/*; do
    $PROGRAM "$FILE"
    
    EXIT_CODE=$?
    
    TOTAL=$((TOTAL + 1))
    
    if [ $EXIT_CODE -ne 0 ]; then
        echo "Test passed for $FILE (exit code: $EXIT_CODE)"
        PASSED=$((PASSED + 1))
    else
        echo "Test failed for $FILE (exit code: $EXIT_CODE)"
        FAILED_TESTS+=("$FILE")
    fi
done

echo "--------------------------------"
echo "Passed $PASSED out of $TOTAL tests."

if [ ${#FAILED_TESTS[@]} -ne 0 ]; then
    echo "The following tests failed:"
    for FAILED in "${FAILED_TESTS[@]}"; do
        echo "$FAILED"
    done
else
    echo "All tests passed."
fi

EXIT_CODE=$((TOTAL - PASSED))
exit $EXIT_CODE
