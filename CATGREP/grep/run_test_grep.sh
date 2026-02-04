#!/bin/bash

FNAME="test.txt"
echo -e "Hello world\nHELLO WORLD\n-hello\nworld-\nnothing here\nTest line\neXample\nhello again" > "$FNAME"

flags_list=(
    "-e -hello"
    "-i hello"
    "-v hello"
    "-c hello"
    "-l world"
    "-n Test"
)

for flags in "${flags_list[@]}"; do
    flag=$(echo "$flags" | awk '{print $1}')
    pattern=$(echo "$flags" | cut -d' ' -f2-)
    
    echo "Тестируем: grep $flag '$pattern' $FNAME"
    
    grep $flag "$pattern" "$FNAME" > grep_output.txt 2>/dev/null
    ./s21_grep $flag "$pattern" "$FNAME" > my_output.txt 2>/dev/null

    if diff -q grep_output.txt my_output.txt >/dev/null; then
        echo "OK: Совпадает"
    else
        echo "FAIL: Отличается"
        echo "Различия:"
        diff -u grep_output.txt my_output.txt
    fi
    echo
done

rm -f "$FNAME" grep_output.txt my_output.txt
