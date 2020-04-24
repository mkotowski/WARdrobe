#!/bin/sh

# automates the process of using pandoc
# to create new gdd document

if [ -f ./build/gdd.pdf ]; then
	echo "DELETING OLD PDF"
	rm ./build/gdd.pdf
fi

if [ -f ./build/gdd.html ]; then
	echo "DELETING OLD HTML"
	rm ./build/gdd.html
fi

echo "MAKING NEW PDF"
pandoc gdd.md \
    --pdf-engine=xelatex \
	--lua-filter ./filters/fenced_divs.lua \
	--lua-filter ./filters/warning.lua \
	--standalone \
    --self-contained \
	--from markdown+task_lists+fancy_lists+fenced_divs+definition_lists \
	--table-of-contents \
	--template ./styles/eisvogel.latex \
		--listings \
	-o ./build/gdd.pdf

echo "MAKING NEW HTML"
pandoc gdd.md \
	--standalone \
    --self-contained \
	--from markdown+task_lists+fancy_lists+fenced_divs+definition_lists \
	--css ./styles/gdd.css \
	-o ./build/gdd.html

echo "DONE"
