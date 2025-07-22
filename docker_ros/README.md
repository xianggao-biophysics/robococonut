# ROS2 Docker Demo

This project demonstrates how to use Docker to run a simple ROS2 application consisting of a publisher and a subscriber node. The publisher continuously sends "hello world" messages, while the subscriber receives these messages and prints them to the console.

## Project Structure

```
ros2-docker-demo
├── docker
│   └── Dockerfile
├── publisher
│   ├── package.xml
│   └── src
│       └── publisher_node.py
├── subscriber
│   ├── package.xml
│   └── src
│       └── subscriber_node.py
├── docker-compose.yml
└── README.md
```

## Prerequisites

- Docker
- Docker Compose

## Building the Docker Image

To build the Docker image for the ROS2 demo, navigate to the `ros2-docker-demo` directory and run the following command:

```bash
docker-compose build
```

## Running the Demo

After building the image, you can start the publisher and subscriber nodes using Docker Compose:

```bash
docker-compose up
```

This command will start both nodes in separate containers. You should see the subscriber printing "hello world" messages received from the publisher in the console.

## Stopping the Demo

To stop the running containers, you can use:

```bash
docker-compose down
```

## Overview

- **Publisher Node**: Publishes "hello world" messages to a specified topic.
- **Subscriber Node**: Subscribes to the topic and prints the received messages.

This demo serves as a basic introduction to using ROS2 with Docker, showcasing how to set up a simple communication between nodes.