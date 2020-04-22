# Introduction 
The aim of this project is to receive data and store data from Arduino:

# Getting Started
You must have bellow softwares pre-installed:
- dotner core
- Docker engine if you want to run containeraised application

# Build and Test

Run it through Visual Studio 2017 or higher. You will have option for IIS, Docker

### Build and test with Docker
If you're not familiar with Docker, firstly spend some time in order to get familiar. 
You'll know you're familiar with Docker enough once when you able to understand the syntax written in files `./MegaHeart/Dockerfile`, `./MegaHeart/.dockerignore` and if you understand bellow commands for building and running this project image:
```
$ docker build -t mssba/megaheart .
$ docker run -d -p 5000:80  mssba/megaheart
```
# Contribute

#TBD