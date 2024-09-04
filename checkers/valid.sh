#!/bin/bash

PROGRAM="./expert_system"

VALID_DIR="resources/valids"

PASSED=0

TOTAL=0

for FILE in "$VALID_DIR"/*; do
    EXPECTED_RESULT=$(tail -n 1 "$FILE" | sed 's/^# Expected result: //')

    OUTPUT=$($PROGRAM "$FILE" | tr -d '\r')
    
    OUTPUT_LAST_LINE=$(echo "$OUTPUT" | tail -n 1)
    
    EXIT_CODE=$?
    
    TOTAL=$((TOTAL + 1))
    
    if [ $EXIT_CODE -eq 0 ] && [ "$OUTPUT_LAST_LINE" == "$EXPECTED_RESULT" ]; then
        echo "Test passed for $FILE"
        PASSED=$((PASSED + 1))
    else
        echo "Test failed for $FILE"
        echo "Expected: $EXPECTED_RESULT"
        echo "Output:   $OUTPUT_LAST_LINE"
    fi

    echo "--------------------------------"
done

echo "--------------------------------"
echo "Passed $PASSED out of $TOTAL tests."

EXIT_CODE=$((TOTAL - PASSED))
exit $EXIT_CODE