# Gstreamer Tutorials

This is a public repository that contains Gstreamer Tutorials and some makefiles

## Installation

The examples that doesn't need the screen can be run inside the docker container provided. Run the following to open the container:
(Container building is not working yet!!)

```bash
docker-compose up -d
docker exec -it gstreamer-container bash
```

## Usage

Compile and run every example using the Makefile provided on Linux. 

## Contributing

Please don't commit binary files. Use always make to compile and create the files inside /bin folder
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.


## License
[MIT](https://choosealicense.com/licenses/mit/)