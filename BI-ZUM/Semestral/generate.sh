#!/bin/bash

rm -rf approx
mkdir -p approx/smiley-gg approx/smiley-geom approx/smiley-lin

./cli.out -i 50000 -r 60 -f data/smiley.jpg -o out.png --cooldown 10000 --scheduler GnG
mv smiley approx/smiley-gg
mv out.png approx/smiley-gg.png

./cli.out -i 50000 -r 60 -f data/smiley.jpg -o out.png --cooldown 10000 --scheduler Geom
mv smiley approx/smiley-geom
mv out.png approx/smiley-geom.png

./cli.out -i 50000 -r 60 -f data/smiley.jpg -o out.png --cooldown 10000 --scheduler Lin
mv smiley approx/smiley-lin
mv out.png approx/smiley-lin.png

./cli.out -i 30000 -r 15 -w 4 -f data/piet_mondrian_composition.jpg -o out.png --cooldown 10000
mv out.png approx/piet_mondrian_composition.png
mv piet_mondrian_composition/ approx/

./cli.out -i 30000 -r 70 -w 4 -f data/davinci_monalisa.jpg -o out.png --cooldown 10000
mv out.png approx/davinci_monalisa.png
mv davinci_monalisa/ approx/

./cli.out -i 30000 -r 70 -w 4 -f data/van_gogh_starry_night.jpg -o out.png --cooldown 10000
mv out.png approx/van_gogh_starry_night.png
mv van_gogh_starry_night/ approx/
