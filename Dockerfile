FROM gcc:latest
COPY . .
RUN makefile run