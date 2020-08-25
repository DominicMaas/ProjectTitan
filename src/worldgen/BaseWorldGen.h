#pragma once

// Represents a base world generator, all world generators must
// implement this class.
class BaseWorldGen {
public:
    // Initialize the world generator with a seed
    BaseWorldGen(int seed) { _seed = seed; }

    // Get the theoretical block type for the given x, y, z position, returns
    // and unsigned int that relates to a certain block type.
    virtual unsigned int getTheoreticalBlockType(int x, int y, int z) { return 0; };

    // Get the seed that was used for this world
    int getSeed() { return _seed; }

protected:
    // The seed used for this world
    int _seed;
};