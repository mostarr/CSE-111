# dos2unix *.cpp *.h Makefile

# for f in *.cpp
# do
#   dos2unix $f
# done
# for f in *.h
# do
#   dos2unix $f
# done

scp -r *.cpp *.h *.tcc Makefile mostarr@unix.ucsc.edu:~/mostarr/Lab4
