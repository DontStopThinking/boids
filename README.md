# Boids Simulation

A Boids artificial life simulation based on the [1986 paper by Craig Reynolds](https://www.red3d.com/cwr/boids/).

# How does it work?
Each "boid", or bird-like object, starts with a random velocity and follows the following rules:

## Cohesion
This rule compels each boid to stay grouped with its neighbours. It does this by finding the midpoint between all its neighbours and navigating towards that point.

## Alignment
This rule encourages each boid to move in a common direction. Each boid will set its facing direction to the average direction of all its neighbours.

## Separation
Helps prevent collisions and overlap of boids. Each boid will check if there are any boids that are very close to it. And if so, then the boid will navigate away from them.
