# tr -d '\15\32' < Makefile > Makefile

# for f in *.cpp
# do
#   tr -d '\15\32' < $f > $f
# done
# for f in *.h
# do
#   tr -d '\15\32' < $f > $f
# done

scp -r *.cpp *.h *.tcc Makefile mostarr@unix.ucsc.edu:~/mostarr/Lab4
