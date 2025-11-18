FROM alpine:latest
RUN apk add --no-cache build-base
COPY . .
RUN make run