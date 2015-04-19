#!/bin/sh

sed '1d;3d;s/^[a-z]* //;s/[a-zA-Z]*=//g;s/  */ /g'
