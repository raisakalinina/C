#!/bin/bash

FNAME="test.txt"
echo -e "Hello, world!\n\n123\n\n\tLine with tab\nEnd\n\n" > $FNAME

flags_list=("-b" "-e" "-n" "-s" "-t" "-v" "-benst")

for flags in "${flags_list[@]}"; do
    echo "Тестируем флаги: $flags"
    
    cat $flags "$FNAME" > cat_output.txt 2> /dev/null

    ./s21_cat $flags "$FNAME" > my_output.txt 2> /dev/null

    if diff -q cat_output.txt my_output.txt >/dev/null; then
        echo "OK: Совпадает"
    else
        echo "FAIL: Отличается"
        echo "Различия:"
        diff -u cat_output.txt my_output.txt
    fi
    echo
done

rm -f $FNAME cat_output.txt my_output.txt