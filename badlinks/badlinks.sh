#!/bin/bash

for i in $(find -L $1 -mtime +7); do
  if [ -h $i ] && [ ! -e $i ];
  then
     echo $i
   fi
done;
