#!/bin/sh

checkerr()
{
	if [ $? -ne 0 ]; then
		echo "$@"
		exit 1
	fi
}

name="$1"
[ -n "$name" ]
checkerr "argument error"

##  initialize
cd $(dirname $0)
checkerr "cd error"

rm -f ./lisp.core
checkerr "rm lisp.core error"

rm -rf ./release
checkerr "rm release error"


##  extract
release="$(/bin/ls npt-*.*.*.tar.gz 2> /dev/null)"
[ -r "${release}" ]
checkerr "release file error: ${release}"

tar zxf ${release}
checkerr "tar error"

path="$(ls -d npt-*/)"
[ -d "${path}" ]
checkerr "path error"

mv "${path}" release
checkerr "mv error"


##  compile
(
cd release
checkerr "cd error"

./${name}_release.sh
checkerr "script error"

[ -x npt ]
checkerr "make error"

./npt --version
checkerr "npt error"

) || exit 1


## save
./save.sh
checkerr "save.sh error"

./load.sh
checkerr "load.sh error"

echo OK
exit 0

