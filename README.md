# Scheduler

A small project for simulating different scheduling algorithms

## Building

This project uses [raylib](https://github.com/raysan5/raylib) and [raygui](https://github.com/raysan5/raygui) as submodules.
These submodules have to be cloned along with the repo. This can be done using
```console
$ git clone --recurse-submodules git@github.com:FirasCh3/Scheduler.git
```

After having cloned the reop successfully, all you need to do is run
```console
make -j$(nproc)
```
This will build the executable that can be found in `bin/`.

The leftover files can be removed using
```console
make clean
```

## Config file

The program needs a description of the processes to simulate. This can be in the form of any plain text file
format like `.txt`. Whitespace is ignored and comments start with a `#`
This file should contain:
- Name of the process
- Arrival time of the process
- Execution (Burst) time of the process
- Priority of the process when executing priority based policies

### Example file:
```
# name arrival burst priority
  P1              0 3 2
P2 4 2 15

# comments and blank lines allowed
P3 1 2 30
```

This file has to be passed as an argument when running the program. It can later be changed in the GUI
```
USAGE: scheduler -i=config_file
```

## License

This project uses the MIT License. This libraries used in this project respect their permissions.
