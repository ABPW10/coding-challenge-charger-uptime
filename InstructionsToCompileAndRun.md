# Instructions on how to compile and run the solution:

```
sudo apt update
sudo apt install g++
chmod +x build.sh
```

# Run the program (examples)
```
./StationUptime input_1.txt
./StationUptime input_2.txt
```

Without path to input file ( type inputs in `stdin` , end with double new lines )
```
./StationUptime
[Stations]
0 0
1 1

[Charger Availability Reports]
0 10 20 true
0 20 30 false
0 30 40 true
1 0 1 true


```

Invalid input ( displays `ERROR` )
```
./StationUpTime input_1_expected_stdout.txt 
```
