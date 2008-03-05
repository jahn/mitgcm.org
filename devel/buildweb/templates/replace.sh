#!/bin/bash

# $Header:  $

sed "s/_UPDATE_REPLACE_/Last update `date +%F`/" $1 | \
sed "s/_COPYRIGHT_REPLACE_/Copyright © 2006/"
