#!/bin/bash

for i in {1..10}; do
	echo "\ntest {i}\n"
	./philo 4 60 60 60 
	sleep 0.1
done
