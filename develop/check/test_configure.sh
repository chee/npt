#!/bin/sh

checkerr()
{
	if [ $? -ne 0 ]; then
		echo "$@"
		exit 1
	fi
}

case "$1" in
bsd)     name="bsd";;
freebsd) name="bsd";;
linux)   name="linux";;
*) name="";;
esac

release="$(/bin/ls ../npt-*.*.*.tar.gz 2> /dev/null)"
[ -r "${release}" ]
checkerr "release file error: ${release}"

./make/clean.sh
checkerr "clean.sh error"

cp -n ${release} make/.
checkerr "cp error: ${release}"

[ -n "${name}" ]
checkerr "argument error"
./make/configure.sh ${name} 2>&1
checkerr "configure.sh error"

./make/check.sh
checkerr "check.sh error"

./make/clean.sh
checkerr "clean.sh error"

exit 0

