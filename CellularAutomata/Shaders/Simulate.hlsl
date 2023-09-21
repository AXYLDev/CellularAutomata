// Must be power of 2
#define WRITE_WIDTH 8
#define WRITE_HEIGHT 8
#define GROUP_WIDTH (WRITE_WIDTH + 2)
#define GROUP_HEIGHT (WRITE_HEIGHT + 2)

Texture2D<uint64_t> tex_In;
RWTexture2D<uint64_t> tex_Out;

groupshared uint64_t gs_Cache[GROUP_WIDTH][GROUP_HEIGHT];

struct Input {
	// TODO
};
cbuffer ConstantBuffer {
	// Must be powers of 2
	uint2 cb_Size;
}

inline uint GetCell(uint x, uint y) {
	uint64_t v = gs_Cache[x >> 4][y >> 4];
	uint bitOff = (x & 15) + ((y & 15) << 4);
	return v & (1 << bitOff);
}
// Compact version macro
#define GET_CELL(x, y)\
	(gs_Cache[x >> 4][y >> 4] & (1 << ((x & 15) + ((y & 15) << 4))));

[BlockSize(GROUP_WIDTH, GROUP_HEIGHT)]
void main(Input IN) {
	// Load data
	uint2 uv = { (gtid.gid.x * WRITE_WIDTH + gtid.x - 1) & cb_Size.x,
	  (gtid.gid.y * WRITE_HEIGHT + gtid.y - 1) & cb_Size.y };
	gs_Cache[IN.gtid.x][IN.gtid.y] = tex_In[uv];
	SharedMemoryBarrierWithGroupSync();
	// Filter out borders, which were just for data loading
	if (IN.gtid.x == 0 || gtid.y == 0 || gtid.x == GROUP_WIDTH - 1 || gtid.y == GROUP_HEIGHT - 1)
		return;
	// Each bit is a cell
	for (uint i = 0; i < 64; i++) {
		uint y = (i & 15) + gtid.x;
		uint x = (i << 4) & 15 + gtid.y;
		// Iterate over neighbours
		uint c =
			GET_CELL(y - 1, x - 1) + GET_CELL(y - 1, x) + GET_CELL(y - 1, x + 1)
			+ GET_CELL(y - 1, x - 1) /* Skip Middle */ + GET_CELL(y - 1, x + 1)
			+ GET_CELL(y + 1, x - 1) + GET_CELL(y + 1, x) + GET_CELL(y + 1, x + 1);
		// Use number of adjacent active tiles (c) to get what the next state should be
		// TODO
	}
}