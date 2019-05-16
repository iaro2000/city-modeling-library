# City Modeling Library
Library for the procedural generation of cities

Uses a cell based generation system. The generator starts with a single cell (convex polygon) representing the boundaries of the city.This is then divided for a number of iterations. After, each cell is assigned a random type and populated with 3d geometry. This can be either pre-generated geometry (trees, houses, etc.) or procedurally generated one - roads, park fences, etc.

Also includes an example application using the Ogre 3D ope source engine for rendering.
